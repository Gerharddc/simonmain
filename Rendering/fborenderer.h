#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QQuickFramebufferObject>

#include "structures.h"

class FBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ~FBORenderer();
    Renderer *createRenderer() const;
    static void SetSTLMesh(Mesh *mesh);
    static void SetToolpath(Toolpath *path);

private:
    static Mesh *stlMesh;
    static Toolpath *toolPath;
};

#endif // FBORENDERER_H
