#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QQuickFramebufferObject>

#include "structures.h"
#include "comborendering.h"

class FBORenderer : public QQuickFramebufferObject
{
    Q_OBJECT
private:
    ComboRendering comb;

public:
    Renderer *createRenderer() const;
    Q_INVOKABLE void rotateView(float x, float y);
    Q_INVOKABLE void panView(float x, float y);
};

#endif // FBORENDERER_H
