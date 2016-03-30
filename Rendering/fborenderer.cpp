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
    comb.SetMeshPos(*comb.getSelectedMeshes().begin(), pos.x(), pos.y());
}

void FBORenderer::removeSelectedMeshes()
{
    for (Mesh *mesh : comb.getSelectedMeshes())
        comb.RemoveMesh(mesh);

    update();
}
