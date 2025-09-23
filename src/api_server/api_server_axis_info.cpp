#include "api_server/api_server.hpp"

void ApiRestServer::setupAxisInfoController() {
    // Get axis info
    _server.on("/axesinfo", [this](PsychicRequest *request)
    {
        // Prepare JSON response
        JsonDocument doc;
        JsonArray jAxes = doc.to<JsonArray>();
        for(uint8_t i = 0; i < 4; i++) {
            Motor* motor = nullptr;
            switch(i) {
                case 0: motor = _XMotor; break;
                case 1: motor = _YMotor; break;
                case 2: motor = _LMotor; break;
                case 3: motor = _RMotor; break;
            }
            if (motor) {
                JsonObject jAxis = jAxes.add<JsonObject>();
                jAxis["name"] = motor->name();
                jAxis["counts_per_unit"] = motor->get_counts_per_unit();
                jAxis["speed_limit"] = motor->get_speed_limit();
                jAxis["acceleration_limit"] = motor->get_acceleration_limit();
                jAxis["actuation_limit"] = motor->get_actuation_limit();
            }
        }

        String response;
        serializeJson(doc, response);
        return request->reply(200, "application/json", response.c_str());
    });
}