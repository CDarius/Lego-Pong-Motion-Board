#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_aiplayer_maxspeed.hpp"
#include "setting_aiplayer_maxerror.hpp"
#include "setting_aiplayer_errorupdatetime.hpp"
#include "game/game_settings.hpp"
#include "game/game.hpp"

class SettingsAIPlayerGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _title;

        GameSettings& _gameSettings;
        AIPlayerMaxSpeedSetting _maxSpeedEasy = AIPlayerMaxSpeedSetting(_gameSettings.aiPlayerEasy, GameAILevel::EASY);
        AIPlayerMaxErrorSetting _maxErrorEasy = AIPlayerMaxErrorSetting(_gameSettings.aiPlayerEasy, GameAILevel::EASY);
        AIPlayerErrorUpdateTimeSetting _errorUpdateTimeEasy = AIPlayerErrorUpdateTimeSetting(_gameSettings.aiPlayerEasy, GameAILevel::EASY);

        AIPlayerMaxSpeedSetting _maxSpeedMedium = AIPlayerMaxSpeedSetting(_gameSettings.aiPlayerMedium, GameAILevel::MEDIUM);
        AIPlayerMaxErrorSetting _maxErrorMedium = AIPlayerMaxErrorSetting(_gameSettings.aiPlayerMedium, GameAILevel::MEDIUM);
        AIPlayerErrorUpdateTimeSetting _errorUpdateTimeMedium = AIPlayerErrorUpdateTimeSetting(_gameSettings.aiPlayerMedium, GameAILevel::MEDIUM);

        AIPlayerMaxSpeedSetting _maxSpeedHard = AIPlayerMaxSpeedSetting(_gameSettings.aiPlayerHard, GameAILevel::HARD);
        AIPlayerMaxErrorSetting _maxErrorHard = AIPlayerMaxErrorSetting(_gameSettings.aiPlayerHard, GameAILevel::HARD);
        AIPlayerErrorUpdateTimeSetting _errorUpdateTimeHard = AIPlayerErrorUpdateTimeSetting(_gameSettings.aiPlayerHard, GameAILevel::HARD);

        ISetting* _settings[9] = {
            &_maxSpeedEasy, &_maxErrorEasy, &_errorUpdateTimeEasy, 
            &_maxSpeedMedium, &_maxErrorMedium, &_errorUpdateTimeMedium, 
            &_maxSpeedHard, &_maxErrorHard, &_errorUpdateTimeHard
        };

    public:
        SettingsAIPlayerGroup(const char* name, const char* title, GameSettings& gameSettings);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};