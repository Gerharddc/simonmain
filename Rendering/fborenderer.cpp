#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#ifndef GLES
#include "loadedgl.h"
#endif

#include "stlrenderer.h"
#include "toolpathrenderer.h"
#include "gridrenderer.h"

class ComboFBORenderer : public QQuickFramebufferObject::Renderer
{
public:
    ComboRendering *comb;

    ComboFBORenderer(const ComboRendering *c)
    {
        comb = (ComboRendering*)c;

#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        comb->Init();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    void render() {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        comb->Draw();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        comb->SetViewSize(size.width(), size.height());

        return new QOpenGLFramebufferObject(size, format);
    }
};

QQuickFramebufferObject::Renderer *FBORenderer::createRenderer() const
{
    return new ComboFBORenderer(&comb);
}

void FBORenderer::rotateView(float x, float y)
{
    comb.ApplyRot(x, y);
    update();
}

void FBORenderer::panView(float x, float y)
{
    comb.Move(x, y);
    update();
}

void FBORenderer::zoomView(float scale)
{
    comb.Zoom(scale);
    update();
}

void FBORenderer::resetView(bool updateNow)
{
    comb.ResetView();

    if (updateNow)
        update();
}

void FBORenderer::loadMesh(QString path)
{
    comb.LoadMesh(path.toStdString().c_str());
    update();

    emit curMeshPosChanged();
}

void FBORenderer::testMouseIntersection(float x, float y)
{
    // Test for mouse intersection with objects and alert the gui
    // if we have moved between a point of no or any mesh selection
    bool needUpdate = false;
    int old = meshesSelected();
    int count = (comb.TestMouseIntersection(x, y, needUpdate) != 0);

    if (needUpdate)
        update();

    if (count != old)
        emit meshesSelectedChanged();

    if (old != 1 && count == 1)
    {
        emit curMeshPosChanged();
        emit curMeshScaleChanged();
    }
}

void FBORenderer::setMeshOpacity(float o)
{
    if (o != comb.MeshOpacity())
    {
        comb.SetMeshOpacity(o);
        update();

        emit meshOpacityChanged();
    }
}

void FBORenderer::setTpOpacity(float o)
{
    if (o != comb.TpOpacity())
    {
        comb.SetTpOpacity(o);
        update();

        emit tpOpacityChanged();
    }
}

QPointF FBORenderer::curMeshPos()
{
    if (meshesSelected() == 1)
    {
        auto &v = comb.getMeshData(*comb.getSelectedMeshes().begin()).moveOnMat;
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
            comb.SetMeshPos(*comb.getSelectedMeshes().begin(), pos.x(), pos.y());
            emit curMeshPosChanged();
            update();
        }
    }
}

float FBORenderer::curMeshLift()
{
    if (meshesSelected() == 1)
    {
        auto &v = comb.getMeshData(*comb.getSelectedMeshes().begin()).moveOnMat;
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
        return comb.getMeshData(*comb.getSelectedMeshes().begin()).scaleOnMat;
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
            comb.SetMeshScale(*comb.getSelectedMeshes().begin(), scale);
            emit curMeshScaleChanged();
            update();
        }
    }
}

void FBORenderer::removeSelectedMeshes()
{
    for (Mesh *mesh : std::set<Mesh*>(comb.getSelectedMeshes()))
        comb.RemoveMesh(mesh);

    emit meshesSelectedChanged();
    emit curMeshPosChanged();
    emit curMeshScaleChanged();

    update();
}
