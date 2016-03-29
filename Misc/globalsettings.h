#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

// This is a helper class used to define settings with finite types.
// This is important because the sotrage mechanism is inherintly not typesafe.
template<typename T> class GlobalSetting
{
private:
    const char *Name;

public:
    GlobalSetting(const char *name, T defVal);
    void Set(T value);
    T Get();
};

class GlobalSettings
{
public:
    GlobalSettings();

    static GlobalSetting<float> BedWidth;
    static GlobalSetting<float> BedLength;
    static GlobalSetting<float> BedHeight;
};

// Create a singleton instance
extern GlobalSettings GlobalSettingsClass;

#endif // GLOBALSETTINGS_H
