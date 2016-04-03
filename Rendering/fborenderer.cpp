#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#ifndef GLES
#include "loadedgl.h"
#endif

#include "stlrendering.h"
#include "toolpathrendering.h"
#include "gridrendering.h"
#include "Misc/globalsettings.h"

class ComboFBORenderer : public QQuickFramebufferObject::Renderer
{
public:

    ComboFBORenderer()
    {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        ComboRendering::Init();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    ~ComboFBORenderer()
    {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        ComboRendering::FreeMemory();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    void render() {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        ComboRendering::Draw();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        ComboRendering::SetViewSize(size.width(), size.height());

        return new QOpenGLFramebufferObject(size, format);
    }
};

QQuickFramebufferObject::Renderer *FBORenderer::createRenderer() const
{
    auto ren = new ComboFBORenderer();
    return ren;
}

void CallFBOUpdate(void *context)
{
    ((FBORenderer*)context)->update();
}

FBORenderer::FBORenderer()
{
    sliceProcess = new QProcess();
    QObject::connect(sliceProcess, SIGNAL(readyReadStandardError()), this, SLOT(ReadSlicerOutput()));
    QObject::connect(sliceProcess, SIGNAL(finished(int)), this, SLOT(SlicerFinsihed(int)));
    ComboRendering::SetUpdateHandler(CallFBOUpdate, this);
}

FBORenderer::~FBORenderer()
{
    delete sliceProcess;
}

void FBORenderer::rotateView(float x, float y)
{
    ComboRendering::ApplyRot(x, y);
}

void FBORenderer::panView(float x, float y)
{
    ComboRendering::Move(x, y);
}

void FBORenderer::zoomView(float scale)
{
    ComboRendering::Zoom(scale);
}

void FBORenderer::resetView(bool updateNow)
{
    ComboRendering::ResetView(updateNow);
}

void FBORenderer::autoArrangeMeshes()
{
    STLRendering::PackMeshes();
    EmitMeshProps();
}

QString FBORenderer::saveMeshes()
{
    return QString::fromStdString(ComboRendering::SaveMeshes(saveName().toStdString()));
}

// This is a helper method used to refresh all the properties
// relating to the current mesh
void FBORenderer::EmitMeshProps()
{
    emit curMeshPosChanged();
    emit curMeshScaleChanged();
    emit curMeshRotChanged();
}

void FBORenderer::loadMesh(QString path)
{
    ComboRendering::LoadMesh(path.toStdString().c_str());

    EmitMeshProps();
    emit meshCountChanged();
}

int FBORenderer::meshCount()
{
    return ComboRendering::getMeshCount();
}

void FBORenderer::testMouseIntersection(float x, float y)
{
    // Test for mouse intersection with objects and alert the gui
    // if we have moved between a point of no or any mesh selection
    //bool needUpdate = false;
    int old = meshesSelected();
    ComboRendering::TestMouseIntersection(x, y);//, needUpdate);

    if (meshesSelected() != old)
        emit meshesSelectedChanged();

    if (old != 1 && meshesSelected() == 1)
    {
        EmitMeshProps();
    }
}

void FBORenderer::setMeshOpacity(float o)
{
    if (o != STLRendering::GetBaseOpacity())
    {
        STLRendering::SetBaseOpacity(o);

        emit meshOpacityChanged();
    }
}

void FBORenderer::setTpOpacity(float o)
{
    if (o != ToolpathRendering::GetOpacity())
    {
        ToolpathRendering::SetOpacity(o);

        emit tpOpacityChanged();
    }
}

QPointF FBORenderer::curMeshPos()
{
    if (meshesSelected() == 1)
    {
        auto &v = STLRendering::getMeshData(*ComboRendering::getSelectedMeshes().begin()).moveOnMat;
        return QPointF(v.x, v.y);
    }
    else
        return QPointF(0.0f, 0.0f);
}

void FBORenderer::setCurMeshPos(QPointF pos)
{
    if (meshesSelected() == 1)
    {
        auto old = curMeshPos();

        // Filter out some noise
        if (std::abs((pos - old).manhattanLength()) >= 0.05f)
        {
            STLRendering::CentreMesh(*ComboRendering::getSelectedMeshes().begin(), pos.x(), pos.y());
            emit curMeshPosChanged();
        }
    }
}

QVector3D FBORenderer::curMeshRot()
{
    if (meshesSelected() == 1)
    {
        auto &v = STLRendering::getMeshData(*ComboRendering::getSelectedMeshes().begin()).rotOnMat;
        return QVector3D(glm::degrees(v.x), glm::degrees(v.y), glm::degrees(v.z));
    }
    else
        return QVector3D(0.0f, 0.0f, 0.0f);
}

void FBORenderer::setCurMeshRot(QVector3D rot)
{
    if (meshesSelected() == 1)
    {
        QVector3D old = curMeshRot();

        // Filter out some noise
        if (std::abs((rot - old).lengthSquared()) >= 0.05f)
        {
            STLRendering::RotateMesh(*ComboRendering::getSelectedMeshes().begin(), rot.x(), rot.y(), rot.z());
            emit curMeshRotChanged();
        }
    }
}

float FBORenderer::curMeshLift()
{
    if (meshesSelected() == 1)
    {
        auto &v = STLRendering::getMeshData(*ComboRendering::getSelectedMeshes().begin()).moveOnMat;
        return v.z;
    }
    else
        return 0.0f;
}

void FBORenderer::setCurMeshLift(float lift)
{
    if (meshesSelected() == 1)
    {
        float old = curMeshLift();

        // Filter out noise
        if (std::abs(lift - old) >= 0.1)
        {
            STLRendering::LiftMesh(*ComboRendering::getSelectedMeshes().begin(), lift);
            emit curMeshLiftChanged();
        }
    }
}

float FBORenderer::curMeshScale()
{
    if (meshesSelected() == 1)
        return STLRendering::getMeshData(*ComboRendering::getSelectedMeshes().begin()).scaleOnMat;
    else
        return 0.0f;
}

void FBORenderer::setCurMeshScale(float scale)
{
    if (meshesSelected() == 1)
    {
        float old = curMeshScale();

        // Filter out noise
        if (std::abs(scale - old) >= 0.005f)
        {
            STLRendering::ScaleMesh(*ComboRendering::getSelectedMeshes().begin(), scale);
            emit curMeshScaleChanged();
        }
    }
}

void FBORenderer::setSaveName(QString sav)
{
    if (sav != m_saveName)
    {
        if (sav == "")
            sav = "Untitled";

        sav.remove("./|\\~"); // TODO: complete list

        m_saveName = sav;
        emit saveNameChanged();
    }
}

QString FBORenderer::sliceMeshes()
{
    if (m_slicerRunning)
    {
        m_slicerRunning = false;
        m_slicerStatus = "Stopped";
        emit slicerRunningChanged();
        emit slicerStatusChanged();
        return "stopped";
    }
    else if (meshCount() == 0)
        return "no meshes";

    m_slicerRunning = true;
    m_slicerStatus = "Running";
    emit slicerRunningChanged();
    emit slicerStatusChanged();

    QString stlName = QString::fromStdString(ComboRendering::SaveMeshes(saveName().toStdString()));
    gcodePath = QString(stlName);
    gcodePath.replace(".stl", ".gcode");

    const QString program = "/home/Simon/.Cura/CuraEngine";

    QStringList arguments;
    arguments << "slice" << "-v";
    arguments << "-j" << "/home/Simon/.Cura/simon.json";
    arguments << "-o" << gcodePath;
    arguments << "-s" << "infill_sparse_density=" + QString::number(GlobalSettings::InfillDensity.Get());
    arguments << "-s" << "layer_height=" + QString::number(GlobalSettings::LayerHeight.Get());
    arguments << "-s" << "skirt_line_count=" + QString::number(GlobalSettings::SkirtLineCount.Get());
    arguments << "-s" << "skirt_gap=" + QString::number(GlobalSettings::SkirtDistance.Get());
    arguments << "-s" << "speed_print=" + QString::number(GlobalSettings::PrintSpeed.Get());
    arguments << "-s" << "speed_infill=" + QString::number(GlobalSettings::InfillSpeed.Get());
    arguments << "-s" << "speed_topbottom=" + QString::number(GlobalSettings::TopBottomSpeed.Get());
    arguments << "-s" << "speed_travel=" + QString::number(GlobalSettings::TravelSpeed.Get());
    arguments << "-s" << "speed_layer_0=" + QString::number(GlobalSettings::FirstLineSpeed.Get());
    arguments << "-l" << stlName;

    sliceProcess->start(program, arguments);

    return "started";
}

void FBORenderer::ReadSlicerOutput()
{
    QStringList sl = QString(sliceProcess->readAllStandardError()).split("\n");
    m_slicerStatus = sl.front();
    emit slicerStatusChanged();
}

void FBORenderer::SlicerFinsihed(int)
{
    m_slicerRunning = false;
    m_slicerStatus = "Finished";
    emit slicerRunningChanged();
    emit slicerStatusChanged();

    ComboRendering::LoadToolpath(gcodePath.toStdString().c_str());
}

void FBORenderer::removeSelectedMeshes()
{
    for (Mesh *mesh : std::set<Mesh*>(ComboRendering::getSelectedMeshes()))
        ComboRendering::RemoveMesh(mesh);

    emit meshesSelectedChanged();
    emit meshCountChanged();
    EmitMeshProps();
}
