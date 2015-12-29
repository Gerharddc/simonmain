#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#ifndef GLES
#include "loadedgl.h"
#endif

class STLinFBORenderer : public QQuickFramebufferObject::Renderer
{   
public:
    STLRenderer *stl;

    STLinFBORenderer(Mesh* mesh)
    {
        stl = new STLRenderer(mesh);

#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        stl->Init();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    ~STLinFBORenderer()
    {
        delete stl;
    }

    void render() {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        stl->Draw();
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
        stl->UpdateWindowSize(size.width(), size.height());

        return new QOpenGLFramebufferObject(size, format);
    }

    //STLRenderer stl;
};

// Default value
Mesh* FBORenderer::stlMesh = nullptr;

void FBORenderer::SetSTLMesh(Mesh *mesh)
{
    stlMesh = mesh;
}

QQuickFramebufferObject::Renderer *FBORenderer::createRenderer() const
{
    if (stlMesh != nullptr)
        return new STLinFBORenderer(stlMesh);
    else
        throw "No STL mesh was set";
}

FBORenderer::~FBORenderer()
{
    if (stlMesh != nullptr)
        delete stlMesh;
}

