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
};

#endif // FBORENDERER_H
