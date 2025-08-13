#pragma once

#include <PsychicHttp.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings/settings.hpp"
#include "settings/settings_group.hpp"
#include "settings/setting.hpp"
#include "web_functions/web_functions.hpp"

class ApiRestServer {
    private:
        PsychicHttpServer _server;
        Settings* _settings;
        WebFunctions* _webFunctions;

        String uriParam(const String& uri, uint8_t position);
        void setupSettingController();
        void setupWebFunctionController();

    public:
        void begin(Settings* settings, WebFunctions* webFunctions);
};
