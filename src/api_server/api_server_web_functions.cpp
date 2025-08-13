#include "api_server/api_server.hpp"

void ApiRestServer::setupWebFunctionController() {
    // Get the list of all groups
    _server.on("/webfunctions", [this](PsychicRequest *request)
    {
        WebFunctionGroup** groups = _webFunctions->getGroups();
        uint16_t groupsCount = _webFunctions->getGroupsCount();

        // Create JSON response
        JsonDocument doc;
        JsonArray jfunctions = doc["functions"].to<JsonArray>();
        // Iterate through each group
        for(uint16_t i = 0; i < groupsCount; i++) {
            // Add the group
            WebFunctionGroup* group = groups[i];
            JsonObject jgroup = jfunctions.add<JsonObject>();
            jgroup["name"] = group->getName();
            jgroup["title"] = group->getTitle();

            // Add all functions in the group
            JsonArray jfunctionsInGroup = jgroup["functions"].to<JsonArray>();
            WebFunction** functions = group->getFunctions();
            uint16_t functionsCount = group->getFunctionsCount();
            for(uint16_t j = 0; j < functionsCount; j++) {
                WebFunction* function = functions[j];
                JsonObject jfunction = jfunctionsInGroup.add<JsonObject>();
                jfunction["name"] = function->getName();
                jfunction["title"] = function->getTitle();
                jfunction["description"] = function->getDescription();
            }
        }

        // Serialize the json
        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });
}