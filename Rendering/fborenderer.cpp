#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#ifndef GLES
#include "loadedgl.h"
#endif

#include "stlrendering.h"
#include "toolpathrenderer.h"
#include "gridrenderer.h"

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
    return new ComboFBORenderer();
}

void FBORenderer::rotateView(float x, float y)
{
    ComboRendering::ApplyRot(x, y);
    update();
}

void FBORenderer::panView(float x, float y)
{
    ComboRendering::Move(x, y);
    update();
}

void FBORenderer::zoomView(float scale)
{
    ComboRendering::Zoom(scale);
    update();
}

void FBORenderer::resetView(bool updateNow)
{
    ComboRendering::ResetView();

    if (updateNow)
        update();
}

void FBORenderer::loadMesh(QString path)
{
    ComboRendering::LoadMesh(path.toStdString().c_str());
    update();

    emit curMeshPosChanged();
}

void FBORenderer::testMouseIntersection(float x, float y)
{
    // Test for mouse intersection with objects and alert the gui
    // if we have moved between a point of no or any mesh selection
    bool needUpdate = false;
    int old = meshesSelected();
    ComboRendering::TestMouseIntersection(x, y, needUpdate);

    if (needUpdate)
        update();

    if (meshesSelected() != old)
        emit meshesSelectedChanged();

    if (old != 1 && meshesSelected() == 1)
    {
        emit curMeshPosChanged();
        emit curMeshScaleChanged();
    }
}

void FBORenderer::setMeshOpacity(float o)
{
    if (o != ComboRendering::MeshesOpacity())
    {
        ComboRendering::SetMeshesOpacity(o);
        update();

        emit meshOpacityChanged();
    }
}

void FBORenderer::setTpOpacity(float o)
{
    if (o != ToolpathRendering::GetOpacity())
    {
        ToolpathRendering::SetOpacity(o);
        update();

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
            update();
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
            // TODO: implement this
            emit curMeshLiftChanged();
            update();
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
            update();
        }
    }
}

void FBORenderer::removeSelectedMeshes()
{
    for (Mesh *mesh : std::set<Mesh*>(ComboRendering::getSelectedMeshes()))
        ComboRendering::RemoveMesh(mesh);

    emit meshesSelectedChanged();
    emit curMeshPosChanged();
    emit curMeshScaleChanged();

    update();
}
