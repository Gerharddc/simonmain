#ifndef QTSETTINGS_H
#define QTSETTINGS_H

#include <QObject>
#include "globalsettings.h"

#define AUTO_SETTING_PROPERTY(TYPE, NAME, NAMECAP) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAMECAP NOTIFY NAME##Changed ) \
    TYPE NAME() const { return GlobalSettings::##NAMECAP.Get(); }  \
    void set##NAMECAP (TYPE val) { GlobalSettings::##NAMECAP.Set(val); } \
    Q_SIGNAL void NAME##Changed();

// This class wraps GlobalSettings for use by Qt
class QtSettings : public QObject
{
    Q_OBJECT
public:
    explicit QtSettings(QObject *parent = 0);

    Q_PROPERTY(float bedWidth READ bedWidth WRITE setBedWidth NOTIFY bedWidthChanged)
    float bedWidth() { return GlobalSettings::BedWidth.Get(); }
    void setBedWidth(float val) { GlobalSettings::BedWidth.Set(val); }

    Q_PROPERTY(float bedHeight READ bedHeight WRITE setBedHeight NOTIFY bedHeightChanged)
    float bedHeight() { return GlobalSettings::BedHeight.Get(); }
    void setBedHeight(float val) { GlobalSettings::BedHeight.Set(val); }

    Q_PROPERTY(float bedLength READ bedLength WRITE setBedLength NOTIFY bedLengthChanged)
    float bedLength() { return GlobalSettings::BedLength.Get(); }
    void setBedLength(float val) { GlobalSettings::BedLength.Set(val); }

    Q_PROPERTY(float infillDensity READ infillDensity WRITE setInfillDensity NOTIFY infillDensityChanged)
    float infillDensity() { return GlobalSettings::InfillDensity.Get(); }
    void setInfillDensity(float val) { GlobalSettings::InfillDensity.Set(val); }

    Q_PROPERTY(float layerHeight READ layerHeight WRITE setLayerHeight NOTIFY layerHeightChanged)
    float layerHeight() { return GlobalSettings::LayerHeight.Get(); }
    void setLayerHeight(float val) { GlobalSettings::LayerHeight.Set(val); }

    Q_PROPERTY(int skirtLineCount READ skirtLineCount WRITE setSkirtLineCount NOTIFY skirtLineCountChanged)
    float skirtLineCount() { return GlobalSettings::SkirtLineCount.Get(); }
    void setSkirtLineCount(int val) { GlobalSettings::SkirtLineCount.Set(val); }

    Q_PROPERTY(float skirtDistance READ skirtDistance WRITE setSkirtDistance NOTIFY skirtDistanceChanged)
    float skirtDistance() { return GlobalSettings::SkirtDistance.Get(); }
    void setSkirtDistance(float val) { GlobalSettings::SkirtDistance.Set(val); }

    Q_PROPERTY(float printSpeed READ printSpeed WRITE setPrintSpeed NOTIFY printSpeedChanged)
    float printSpeed() { return GlobalSettings::PrintSpeed.Get(); }
    void setPrintSpeed(float val) { GlobalSettings::PrintSpeed.Set(val); }

signals:
    void bedWidthChanged();
    void bedHeightChanged();
    void bedLengthChanged();
    void infillDensityChanged();
    void layerHeightChanged();
    void skirtLineCountChanged();
    void skirtDistanceChanged();
    void printSpeedChanged();
};

// The global instance
extern QtSettings qtSettings;

#endif // QTSETTINGS_H
