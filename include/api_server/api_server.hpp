#pragma once

#include <PsychicHttp.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings/settings.hpp"
#include "settings/settings_group.hpp"
#include "settings/setting.hpp"
#include "web_functions/web_functions.hpp"
#include "motor_control/motor.hpp"

class ApiRestServer {
    private:
        PsychicHttpServer _server;
        Settings* _settings;
        WebFunctions* _webFunctions;
        Motor* _XMotor;
        Motor* _YMotor;
        Motor* _LMotor;
        Motor* _RMotor;


        String uriParam(const String& uri, uint8_t position);
        void setupSettingController();
        void setupWebFunctionController();
        void setupAxisLogController();

        Motor* getMotorByName(const char* name);

    public:
        void begin(Settings* settings, WebFunctions* webFunctions, Motor* xMotor, Motor* yMotor, Motor* lMotor, Motor* rMotor);
};
