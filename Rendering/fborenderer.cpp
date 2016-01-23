#include "fborenderer.h"

#include <QOpenGLFramebufferObject>

#ifndef GLES
#include "loadedgl.h"
#endif

#include "stlrenderer.h"
#include "toolpathrenderer.h"
#include "gridrenderer.h"

class STLinFBORenderer : public QQuickFramebufferObject::Renderer
{   
public:
    STLRenderer *stl;
    ToolpathRenderer *tp;
    GridRenderer *grid;

    STLinFBORenderer(Mesh* mesh, Toolpath *toolPath)
    {
        stl = new STLRenderer(mesh);
        tp = new ToolpathRenderer(toolPath);
        grid = new GridRenderer(100, 100, 100, 10);

#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        stl->Init();
        tp->Init();
        grid->Init();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
    }

    ~STLinFBORenderer()
    {
        delete stl;
        delete tp;
        delete grid;
    }

    void render() {
#ifndef GLES
        LoadedGL::ActivateGL();
#endif
        //stl->Draw();
        //tp->Draw();
        grid->Draw();
#ifndef GLES
        LoadedGL::DeactivateGL();
#endif
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        stl->UpdateWindowSize(size.width(), size.height());
        tp->UpdateWindowSize(size.width(), size.height());
        grid->UpdateWindowSize(size.width(), size.height());

        return new QOpenGLFramebufferObject(size, format);
    }
};

// Default value
Mesh* FBORenderer::stlMesh = nullptr;
Toolpath* FBORenderer::toolPath = nullptr;

void FBORenderer::SetSTLMesh(Mesh *mesh)
{
    stlMesh = mesh;
}

void FBORenderer::SetToolpath(Toolpath *path)
{
    toolPath = path;
}

QQuickFramebufferObject::Renderer *FBORenderer::createRenderer() const
{
    if (stlMesh != nullptr && toolPath != nullptr)
        return new STLinFBORenderer(stlMesh, toolPath);
    else
        throw "No STL mesh was set";
}

FBORenderer::~FBORenderer()
{
    if (stlMesh != nullptr)
        delete stlMesh;

    if (toolPath != nullptr)
        delete toolPath;
}

