#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "settings_group.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/motorwithreferenceswitch.hpp"
#include "motor_control/motorwithstallreference.hpp"
#include "game/game.hpp"
#include "game/game_settings.hpp"
#include "game/encodermultijog.hpp"
#include "axis/settings_axis_group.hpp"
#include "axis_switch_homing/settings_axis_switch_homing_group.hpp"
#include "axis_stall_homing/settings_axis_stall_homing_group.hpp"
#include "game/game_x_axis/settings_game_x_group.hpp"
#include "game/game_y_axis/settings_game_y_group.hpp"
#include "game/game_lr_axis/settings_game_lr_group.hpp"
#include "ai_player/settings_aiplayer_group.hpp"
#include "jog/settings_jog_group.hpp"

class Settings {
    private:
        GameSettings& _gameSettings;
        encoder_multi_jog_config_t& _config;
        MotorWithStallReference& _XMotor;
        MotorWithReferenceSwitch& _YMotor;
        MotorWithStallReference& _LMotor;
        MotorWithStallReference& _RMotor;

        SettingsGameAxisXGroup _gameXAxis = SettingsGameAxisXGroup(_gameSettings.xAxis);
        SettingsGameAxisYGroup _gameYAxis = SettingsGameAxisYGroup(_gameSettings.yAxis);
        SettingsGameAxisLRGroup _gameLRAxis = SettingsGameAxisLRGroup(_gameSettings.lrAxis);
        SettingsAIPlayerGroup _aiPlayerSettings = SettingsAIPlayerGroup("ai_player", "AI Player", _gameSettings.aiPlayer);
        SettingsJogGroup _jogSettings = SettingsJogGroup(_config);
        SettingsAxisGroup _xSettings = SettingsAxisGroup("x_axis", "X Axis", _XMotor);
        SettingsAxisGroup _ySettings = SettingsAxisGroup("y_axis", "Y Axis", _YMotor);
        SettingsAxisGroup _lSettings = SettingsAxisGroup("l_axis", "L Axis", _LMotor);
        SettingsAxisGroup _rSettings = SettingsAxisGroup("r_axis", "R Axis", _RMotor);
        SettingsAxisStallHomingGroup _xHomingSettings = SettingsAxisStallHomingGroup("x_axis_homing", "X Axis Homing", _XMotor);
        SettingsAxisSwitchHomingGroup _yHomingSettings = SettingsAxisSwitchHomingGroup("y_axis_homing", "Y Axis Homing", _YMotor);
        SettingsAxisStallHomingGroup _lHomingSettings = SettingsAxisStallHomingGroup("l_axis_homing", "L Axis Homing", _LMotor);
        SettingsAxisStallHomingGroup _rHomingSettings = SettingsAxisStallHomingGroup("r_axis_homing", "R Axis Homing", _RMotor);

        SettingsGroup* _groups[13] = {
            &_gameXAxis, &_gameYAxis, &_gameLRAxis, &_jogSettings, &_aiPlayerSettings,
            &_xSettings, &_ySettings, &_lSettings, &_rSettings, 
            &_xHomingSettings, &_yHomingSettings, &_lHomingSettings, &_rHomingSettings
        };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(SettingsGroup*);
        
    public:
        Settings(Game& game, encoder_multi_jog_config_t& config, 
            MotorWithStallReference& xMotor, MotorWithReferenceSwitch& yMotor, MotorWithStallReference& lMotor, MotorWithStallReference& rMotor) 
            : _gameSettings(*(game.getSettings())), _config(config), _XMotor(xMotor), _YMotor(yMotor), _LMotor(lMotor), _RMotor(rMotor)
        {            
        }

        SettingsGroup* getGroup(const char* name);

        SettingsGroup** getGroups();
        uint16_t getGroupsCount() const;

        void storeInNVS();
        void restoreFromNVS();
};