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

signals:
    void bedWidthChanged();
};

// The global instance
extern QtSettings qtSettings;

#endif // QTSETTINGS_H
