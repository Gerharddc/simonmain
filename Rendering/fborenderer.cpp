#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#include "stlrenderer.h"
#include "loadedgl.h"

class STLinFBORenderer : public QQuickFramebufferObject::Renderer
{
public:
    STLinFBORenderer()
    {
        LoadedGL::ActivateGL();
        stl.Init();
        LoadedGL::DeactivateGL();
    }

    void render() {
        LoadedGL::ActivateGL();
        stl.Draw();
        LoadedGL::DeactivateGL();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        //setwindowsize
        stl.UpdateWindowSize(size.width(), size.height());

        return new QOpenGLFramebufferObject(size, format);
    }

    STLRenderer stl;
};

QQuickFramebufferObject::Renderer *FBORenderer::createRenderer() const
{
    return new STLinFBORenderer();
}

