#ifndef QTSETTINGS_H
#define QTSETTINGS_H

#include <QObject>
#include "globalsettings.h"

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

signals:
    void bedWidthChanged();
    void bedHeightChanged();
    void bedLengthChanged();
};

// The global instance
extern QtSettings qtSettings;

#endif // QTSETTINGS_H
