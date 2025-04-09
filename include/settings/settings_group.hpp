#ifndef __GAME_SETTINGS_GROUP_HPP__
#define __GAME_SETTINGS_GROUP_HPP__

#include <Arduino.h>
#include "settings/setting.hpp"

class SettingsGroup {
    public:
        virtual const char* getName() const;
        virtual const char* getTitle() const;

        virtual ISetting** getSettings();
        virtual uint16_t getSettingsCount() const;

        ISetting* getSetting(const char* name);
};

#endif