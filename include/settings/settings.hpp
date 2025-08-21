#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "settings_group.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/motorwithreferenceswitch.hpp"
#include "game/game.hpp"
#include "game/game_settings.hpp"
#include "game/encodemultijog.hpp"
#include "axis/settings_axis_group.hpp"
#include "axis_switch_homing/settings_axis_switch_homing_group.hpp"
#include "game_x_axis/settings_game_x_group.hpp"
#include "jog/settings_jog_group.hpp"

class Settings {
    private:
        GameSettings& _gameSettings;
        encoder_multi_jog_config_t& _config;
        Motor& _XMotor;
        MotorWithReferenceSwitch& _YMotor;
        MotorWithReferenceSwitch& _LMotor;
        MotorWithReferenceSwitch& _RMotor;

        SettingsGameAxisXGroup _gameXAxis = SettingsGameAxisXGroup(_gameSettings);
        SettingsJogGroup _jogSettings = SettingsJogGroup(_config);
        SettingsAxisGroup _xSettings = SettingsAxisGroup("x_axis", "X Axis", _XMotor);
        SettingsAxisGroup _ySettings = SettingsAxisGroup("y_axis", "Y Axis", _YMotor);
        SettingsAxisGroup _lSettings = SettingsAxisGroup("l_axis", "L Axis", _LMotor);
        SettingsAxisGroup _rSettings = SettingsAxisGroup("r_axis", "R Axis", _RMotor);
        SettingsAxisSwitchHomingGroup _yHomingSettings = SettingsAxisSwitchHomingGroup("y_axis_homing", "Y Axis Homing", _YMotor);
        SettingsAxisSwitchHomingGroup _lHomingSettings = SettingsAxisSwitchHomingGroup("l_axis_homing", "L Axis Homing", _LMotor);
        SettingsAxisSwitchHomingGroup _rHomingSettings = SettingsAxisSwitchHomingGroup("r_axis_homing", "R Axis Homing", _RMotor);

        SettingsGroup* _groups[8] = {
            &_gameXAxis, 
            &_xSettings, &_ySettings, &_lSettings, &_rSettings, 
            &_yHomingSettings, &_lHomingSettings, &_rHomingSettings
        };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(SettingsGroup*);
        
    public:
        Settings(Game& game, encoder_multi_jog_config_t& config, 
            Motor& xMotor, MotorWithReferenceSwitch& yMotor, MotorWithReferenceSwitch& lMotor, MotorWithReferenceSwitch& rMotor) 
            : _gameSettings(*(game.getSettings())), _config(config), _XMotor(xMotor), _YMotor(yMotor), _LMotor(lMotor), _RMotor(rMotor)
        {            
        }

        SettingsGroup* getGroup(const char* name);

        SettingsGroup** getGroups();
        uint16_t getGroupsCount() const;

        void storeInNVS();
        void restoreFromNVS();
};