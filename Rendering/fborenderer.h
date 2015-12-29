#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QQuickFramebufferObject>

#include "stlrenderer.h"

class FBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ~FBORenderer();
    Renderer *createRenderer() const;
    static void SetSTLMesh(Mesh *mesh);

private:
    static Mesh *stlMesh;
};

#endif // FBORENDERER_H
