#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QQuickFramebufferObject>

class STLRenderer;

class FBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    Renderer *createRenderer() const;
};

#endif // FBORENDERER_H
