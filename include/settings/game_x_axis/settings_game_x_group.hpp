#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_x_ballstartspeed.hpp"

class SettingsGameAxisXGroup : public SettingsGroup {
    private:
        GameSettings& _gameSetttings;

        GameXBallStartSpeedSetting _gameBallStartSpeedX = GameXBallStartSpeedSetting(_gameSetttings);

        ISetting* _settings[1] = {&_gameBallStartSpeedX};

    public:
        SettingsGameAxisXGroup(GameSettings& _gameSetttings);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};