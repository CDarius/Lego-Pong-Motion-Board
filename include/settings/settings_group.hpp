#ifndef __GAME_SETTINGS_GROUP_HPP__
#define __GAME_SETTINGS_GROUP_HPP__

#include <Arduino.h>
#include "settings/setting.hpp"

class SettingsGroup {
    public:
        virtual const char* getName() const = 0;
        virtual const char* getTitle() const = 0;

        virtual ISetting** getSettings() = 0;
        virtual uint16_t getSettingsCount() const = 0;

        ISetting* getSetting(const char* name);
};

#endif