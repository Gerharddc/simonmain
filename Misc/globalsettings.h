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

// Explicitly specialize the GS classes
template class GlobalSetting<float>;

// As static class is used instead of a namespace because the compiler does not
// like to combine the nampesace option with this funny template class that is
// implemented in the cpp file
class GlobalSettings
{
public:
    static void LoadSettings();
    static void SaveSettings();

    // The actual settings
    static GlobalSetting<float> BedWidth;
    static GlobalSetting<float> BedLength;
    static GlobalSetting<float> BedHeight;
    static GlobalSetting<float> InfillLineDistance;
};

#endif // GLOBALSETTINGS_H
