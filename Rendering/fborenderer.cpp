#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#include "stlrenderer.h"

#ifndef GLES
#include "loadedgl.h"
#endif

class STLinFBORenderer : public QQuickFramebufferObject::Renderer
{
public:
    STLinFBORenderer()
    {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        stl.Init();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    void render() {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        stl.Draw();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
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

