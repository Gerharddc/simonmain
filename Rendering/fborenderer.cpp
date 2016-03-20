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
