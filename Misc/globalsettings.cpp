#include "globalsettings.h"

#include <cstddef>
#include <map>
#include <set>
#include <cstring>
#include <memory>

#include <QDebug>

GlobalSettings GlobalSettingsClass = GlobalSettings();

struct SettingValue
{
    // We use a shared ptr because std::map likes to copy this thing like crazy
    // and it's not worth the effort of doing anything more advanced to combat that here
    std::shared_ptr<unsigned char> bytes;
    std::size_t byteCnt = 0;

    SettingValue() { }

    SettingValue(const void *_bytes, std::size_t _byteCnt)
    {
        bytes = std::shared_ptr<unsigned char>(new unsigned char(_byteCnt));
        memcpy(bytes.get(), _bytes, _byteCnt);
        byteCnt = _byteCnt;
    }

    SettingValue& operator =(const SettingValue& o)
    {
        bytes = o.bytes;
        byteCnt = o.byteCnt;
        return *this;
    }
};

std::set<const char*> queuedSettings;
// TODO: maybe a hashed map will deliver better performance
std::map<const char*, SettingValue> settingsMap;

const char* const SettingsFilePath = "settings.bin";

// TODO: implement a bg thread to save the settings
void SaveSettings()
{
    for (auto name : queuedSettings)
    {
        // TODO: implement this
    }
}

void LoadSettings()
{
    // TODO: implement this
}

GlobalSettings::GlobalSettings()
{
    LoadSettings();
}

// This function saves a setting by converting it to a byte array
template<typename T> void SetGlobalSetting(const char *name, T value, bool enqueue = true)
{
    // Assign the settings
    settingsMap[name] = SettingValue(&value, sizeof(value));

    // Queue long-term storage saving of the setting
    if (enqueue)
        queuedSettings.insert(name);
}

// This function reads a setting by converting it from a byte array
template<typename T> T GetGlobalSetting(const char *name)
{
    if (settingsMap.count(name) == 0)
        return T();

    // NOTE: This thing is seriously unsafe and should only be used by GlobalSetting objects

    return *reinterpret_cast<T*>(settingsMap[name].bytes.get());
}

template<typename T> GlobalSetting<T>::GlobalSetting(const char *name, T defVal)
{
    Name = name;
    SetGlobalSetting(Name, defVal, false);
}

template<typename T> void GlobalSetting<T>::Set(T value)
{
    SetGlobalSetting(Name, value);
}

template<typename T> T GlobalSetting<T>::Get()
{
    return GetGlobalSetting<T>(Name);
}

// Init the settings
GlobalSetting<float> GlobalSettings::BedWidth = GlobalSetting<float>("BedWidth", 100.0f);
GlobalSetting<float> GlobalSettings::BedLength = GlobalSetting<float>("Bedlength", 100.0f);
GlobalSetting<float> GlobalSettings::BedHeight = GlobalSetting<float>("BedHeight", 100.0f);

// Explicitly specialize the GS classes
template class GlobalSetting<float>;
