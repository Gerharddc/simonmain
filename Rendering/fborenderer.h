#ifndef FBORENDERER_H
#define FBORENDERER_H

#include <QQuickFramebufferObject>
#include <QString>

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
    Q_INVOKABLE void zoomView(float scale);
    Q_INVOKABLE void resetView(bool updateNow = true);
    Q_INVOKABLE void loadMesh(QString path);

    Q_PROPERTY(float meshOpacity READ meshOpacity WRITE setMeshOpacity NOTIFY meshOpacityChanged)
    bool meshOpacity() { return comb.MeshOpacity(); }
    void setMeshOpacity(float o);

    Q_PROPERTY(float tpOpacity READ tpOpacity WRITE setTpOpacity NOTIFY tpOpacityChanged)
    bool tpOpacity() { return comb.TpOpacity(); }
    void setTpOpacity(float o);

signals:
   void meshOpacityChanged();
   void tpOpacityChanged();
};

#endif // FBORENDERER_H
