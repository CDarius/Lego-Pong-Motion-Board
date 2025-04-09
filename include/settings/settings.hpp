#ifndef __GAME_AND_AXIS_SETTINGS_HPP__
#define __GAME_AND_AXIS_SETTINGS_HPP__

#include <Arduino.h>
#include <Preferences.h>
#include "settings_group.hpp"
#include "motor_control/motor.hpp"
#include "game/game.hpp"
#include "game/game_settings.hpp"
#include "axis/settings_axis_group.hpp"
#include "game_x_axis/settings_game_x_group.hpp"

class Settings {
    private:
        GameSettings& _gameSettings;
        Motor& _XMotor;
        Motor& _YMotor;
        Motor& _LMotor;
        Motor& _RMotor;        

        SettingsGameAxisXGroup _gameXAxis = SettingsGameAxisXGroup(_gameSettings);
        SettingsAxisGroup _xSettings = SettingsAxisGroup("x_axis", "X Axis", _XMotor);
        SettingsAxisGroup _ySettings = SettingsAxisGroup("y_axis", "Y Axis", _YMotor);
        SettingsAxisGroup _lSettings = SettingsAxisGroup("l_axis", "L Axis", _LMotor);
        SettingsAxisGroup _rSettings = SettingsAxisGroup("r_axis", "R Axis", _RMotor);

        SettingsGroup* _groups[5] = {&_gameXAxis, &_xSettings, &_ySettings, &_lSettings, &_rSettings};
        uint16_t _groupsCount = sizeof(_groups) / sizeof(SettingsGroup*);
        
    public:
        Settings(Game& game, Motor& xMotor, Motor& yMotor, Motor& lMotor, Motor& rMotor) : _gameSettings(*(game.getSettings())), _XMotor(xMotor), _YMotor(yMotor), _LMotor(lMotor), _RMotor(rMotor)
        {            
        }

        SettingsGroup* getSetting(const char* name);

        SettingsGroup** getGroups();
        uint16_t getGroupsCount() const;

        void storeInNVS();
        void restoreFromNVS();
};

#endif