#include "qtsettings.h"

// The global instance
QtSettings qtSettings;

namespace QtSettingsInternal {
    // Because we can't directly convert a function pointer to a void*,
    // we need to create a structure that encapsulates the pointer,
    // we can then pass pointers to these structures
    struct SignalWrapper {
        typedef void(QtSettings::*signal)();
        signal sig;

        SignalWrapper(signal sig_)
            : sig(sig_) {}

        void operator ()()
        {
            // This signal will be emitted from the global/singleton object
            emit (qtSettings.*sig)();
        }
    };

    // We use a generic function to envoke the signal contained in the struct
    template <typename T>
    void HandleWithSignal(void *context, T value)
    {
        ((SignalWrapper*)(context))->operator ()();
    }

    // We need to create a wrapper for each signal
    SignalWrapper bedWidthSignal(&QtSettings::bedWidthChanged);
}

using namespace QtSettingsInternal;

QtSettings::QtSettings(QObject *parent) : QObject(parent)
{
    // Connect the qt signals to the the settings backend
    GlobalSettings::BedWidth.RegisterHandler(HandleWithSignal<float>, &bedWidthSignal);
}
