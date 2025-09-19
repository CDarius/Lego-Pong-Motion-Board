#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_aiplayer_maxspeed.hpp"
#include "setting_aiplayer_maxerror.hpp"
#include "setting_aiplayer_updatetime.hpp"
#include "game/game_settings.hpp"

class SettingsAIPlayerGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _title;

        GameAIPlayerSettings& _aiSettings;
        AIPlayerMaxSpeedSetting _maxSpeed = AIPlayerMaxSpeedSetting(_aiSettings);
        AIPlayerMaxErrorSetting _maxError = AIPlayerMaxErrorSetting(_aiSettings);
        AIPlayerUpdateTimeSetting _updateTime = AIPlayerUpdateTimeSetting(_aiSettings);

        ISetting* _settings[3] = {&_maxSpeed, &_maxError, &_updateTime};

    public:
        SettingsAIPlayerGroup(const char* name, const char* title, GameAIPlayerSettings& aiSettings);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};