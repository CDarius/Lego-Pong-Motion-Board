#include "api_server.hpp"

String uriParam(const String& uri, uint8_t position) {
    uint8_t count = 0;
    int start = (uri[0] == '/') ? 1 : 0; // Skip leading '/'
    int end = uri.indexOf('/', start);
    
    while (end != -1) {
        if (count == position) {
            return uri.substring(start, end);
        }
        count++;
        start = end + 1;
        end = uri.indexOf('/', start);
    }
    
    // Last segment (or only segment if no '/'
    if (count == position) {
        return uri.substring(start);
    }
    
    return ""; // Return empty string if position is out of bounds
}

void ISettingToJsonValue(JsonDocument& doc, const char* key, ISetting& setting) {
    switch (setting.getType())
    {
    case SettingType::FloatType:
    {
        Setting<float>& fsetting = (Setting<float>&)setting;
        doc[key] = fsetting.getValue();
        break;
    }
    case SettingType::UInt8:
    {
        Setting<uint8_t>& usetting = (Setting<uint8_t>&)setting;
        doc[key] = usetting.getValue();
        break;
    }
    
    default:
        doc[key] = nullptr;
        break;
    }
}

void stringValueToISetting(String& string, ISetting& setting) {
    switch (setting.getType())
    {
    case SettingType::FloatType:
    {
        Setting<float>& fsetting = (Setting<float>&)setting;
        fsetting.setValue(string.toFloat());
        break;
    }
    case SettingType::UInt8:
    {
        Setting<uint8_t>& usetting = (Setting<uint8_t>&)setting;
        usetting.setValue((uint8_t)string.toInt());
        break;
    }
    
    default:
        break;
    }
}

void ApiRestServer::begin(Settings* settings) {
    _settings = settings;

    // Configure the webserver
    _server.listen(5000);

    // CORS headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTION");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "rigin, X-Requested-With, Content-Type, Accept, Authorization");

    // Enable OPTION method for all endpoints
    _server.on("*", HTTP_OPTIONS, [](PsychicRequest *request) {
        return request->reply(200);
    });

    setupSettingController();
}

void ApiRestServer::setupSettingController() {
    // Get the list of all settings
    _server.on("/settings", [this](PsychicRequest *request)
    {
        SettingsGroup** groups = _settings->getGroups();
        uint16_t groupsCount = _settings->getGroupsCount();

        // Create JSON response
        JsonDocument doc;
        JsonArray jgroups = doc["groups"].to<JsonArray>();
        for(uint16_t i = 0; i < groupsCount; i++) {
            // Add the group
            SettingsGroup* group = groups[i];
            JsonObject jgroup = jgroups.add<JsonObject>();
            jgroup["name"] = group->getName();
            jgroup["title"] = group->getTitle();

            // Add all group settings
            JsonArray jsettings = jgroup["settings"].to<JsonArray>();
            ISetting** settings = group->getSettings();
            uint16_t settingsCount = group->getSettingsCount();
            for(uint16_t j = 0; j < settingsCount; j++) {
                ISetting* setting = settings[j];
                JsonObject jsetting = jsettings.add<JsonObject>();
                jsetting["name"] = setting->getName();
                jsetting["title"] = setting->getTitle();
                jsetting["description"] = setting->getDescription();
                jsetting["unit"] = setting->getUnit();
                if (setting->hasMinValue())
                    jsetting["minValue"] = "3";
                else
                    jsetting["minValue"] = nullptr;
                switch (setting->getType())
                {
                case SettingType::FloatType:
                    jsetting["type"] = "float"; 
                    {
                        Setting<float>* fsetting = (Setting<float>*)setting;
                        if (setting->hasMinValue())
                            jsetting["minValue"] = fsetting->getMinValue();
                        if (setting->hasMaxValue())
                            jsetting["maxValue"] = fsetting->getMaxValue();
                        if (setting->hasChangeStep())
                            jsetting["stepChange"] = fsetting->getChangeStep();    
                    }
                    break;
                case SettingType::UInt8:
                    jsetting["type"] = "int"; 
                    {
                        Setting<uint8_t>* usetting = (Setting<uint8_t>*)setting;
                        if (setting->hasMinValue())
                            jsetting["minValue"] = usetting->getMinValue();
                        if (setting->hasMaxValue())
                            jsetting["maxValue"] = usetting->getMaxValue();
                        if (setting->hasChangeStep())
                            jsetting["stepChange"] = usetting->getChangeStep();    
                    }
                    break;

                default:
                    jsetting["type"] = "???";
                    break;
                }

                if (!setting->hasMinValue())
                    jsetting["minValue"] = nullptr;
                if (!setting->hasMaxValue())
                    jsetting["maxValue"] = nullptr;
                if (!setting->hasChangeStep())
                    jsetting["stepChange"] = nullptr;
            }
        }

        // Serialize the json
        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });

    // Store the setting to NVS
    _server.on("/settings/storetonvs", [this](PsychicRequest *request)
    {
        _settings->storeInNVS();

        return request->reply(200);
    });

    // Restore the setting from NVS
    _server.on("/settings/restorefromnvs", [this](PsychicRequest *request)
    {
        _settings->restoreFromNVS();

        return request->reply(200);
    });
    
    // Get the value of a setting
    // Uri format: "/get-value/group name/param name"
    _server.on("/settings/*", [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String group_name = uriParam(uri, 1);
        String setting_name = uriParam(uri, 2);

        // Try to get the setting        
        SettingsGroup* group = this->_settings->getSetting(group_name.c_str());
        ISetting* setting = group == nullptr ? nullptr : group->getSetting(setting_name.c_str());

        if (setting == nullptr)
            return request->reply(404);

        // Create JSON response
        JsonDocument doc;
        doc["group"] = group_name;
        doc["name"] = setting_name;
        ISettingToJsonValue(doc, "value", *setting);

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });

    // Set the value of a setting
    // Uri format: "/get-value/group name/param name"
    // Value as a plain text in the body
    _server.on("/settings/*", HTTP_PUT, [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String group_name = uriParam(uri, 1);
        String setting_name = uriParam(uri, 2);
 
        // Try to get the setting        
        SettingsGroup* group = this->_settings->getSetting(group_name.c_str());
        ISetting* setting = group == nullptr ? nullptr : group->getSetting(setting_name.c_str());

        if (setting == nullptr)
            return request->reply(404);

        // Get the value from the body
        String value_str = request->body();
        if (value_str.length() == 0)
            return request->reply(400);
        
        stringValueToISetting(value_str, *setting);

        // Create JSON response
        JsonDocument doc;
        doc["group"] = group_name;
        doc["name"] = setting_name;
        ISettingToJsonValue(doc, "value", *setting);

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });
}