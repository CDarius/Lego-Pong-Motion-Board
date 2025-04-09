#ifndef __API_REST_SERVER_HPP__
#define __API_REST_SERVER_HPP__

#include <PsychicHttp.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings/settings.hpp"
#include "settings/settings_group.hpp"
#include "settings/setting.hpp"

class ApiRestServer {
    private:
        PsychicHttpServer _server;
        Settings* _settings;

        void setupSettingController();

    public:
        void begin(Settings* settings);
};

#endif