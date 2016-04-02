#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <map>
#include <set>
#include <string>

// This is a helper class used to define settings with finite types.
// This is important because the sotrage mechanism is inherintly not typesafe.
template<typename T> class GlobalSetting
{
private:
    std::string Name;

public:
    GlobalSetting(const std::string name, T defVal);
    void Set(T value);
    T Get();
};

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
