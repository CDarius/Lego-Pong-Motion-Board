#include "api_server/api_server.hpp"

const char* ExecutionStatusToString(WebFunctionExecutionStatus status) {
    switch (status) {
        case WebFunctionExecutionStatus::Done:
            return "Done";
        case WebFunctionExecutionStatus::InProgress:
            return "InProgress";
        case WebFunctionExecutionStatus::Failed:
            return "Failed";
        default:
            return "Unknown";
    }
}

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
                JsonArray jprerequisites = jfunction["prerequisites"].to<JsonArray>();
                for(uint16_t k = 0; k < function->getPrerequisitesCount(); k++) {
                    jprerequisites.add(function->getPrerequisiteDescription(k));
                }
            }
        }

        // Serialize the json
        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });

    // Get the value of a setting
    // Uri format: "/webfunctions/{group name}/{function name}/start"
    // Uri format: "/webfunctions/{group name}/{function name}/stop"
    // Uri format: "/webfunctions/{group name}/{function name}/status"
    // Uri format: "/webfunctions/{group name}/{function name}/save_result"
    // Uri format: "/webfunctions/{group name}/{function name}/discard_result"
    // Uri format: "/webfunctions/{group name}/{function name}/prerequisites"
    _server.on("/webfunctions/*", [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String group_name = uriParam(uri, 1);
        String function_name = uriParam(uri, 2);
        String action = uriParam(uri, 3);

        // Try to get the function
        WebFunctionGroup* group = this->_webFunctions->getGroup(group_name.c_str());
        if (!group) {
            return request->reply(404, "application/json", "{\"error\":\"Group not found\"}");
        }
        WebFunction* function = group->getFunction(function_name.c_str());
        if (!function) {
            return request->reply(404, "application/json", "{\"error\":\"Function not found\"}");
        }
    

        // Create an JSON document for the response
        JsonDocument doc;
        doc["group"] = group_name.c_str();
        doc["function"] = function_name.c_str();
        doc["action"] = action.c_str();

        // Execute the action
        if (action == "start") {
            WebFunctionExecutionStatus status = function->start();
            doc["status"] = ExecutionStatusToString(status);
            const char* failureDescription = function->getFailuerDescription();
            if (failureDescription) {
                doc["failure_description"] = failureDescription;
            }
        } else if (action == "stop") {
            function->stop();
        } else if (action == "status") {
            doc["status"] = ExecutionStatusToString(function->getRunningStatus());
            const char* failureDescription = function->getFailuerDescription();
            if (failureDescription) {
                doc["failure_description"] = failureDescription;
            }
        } else if (action == "prerequisites") {
            bool prerequisitesMet[function->getPrerequisitesCount()];
            function->arePrerequisitesMet(prerequisitesMet);
            JsonArray jprerequisites = doc["prerequisites_met"].to<JsonArray>();
            for (uint16_t i = 0; i < function->getPrerequisitesCount(); i++) {
                jprerequisites.add(prerequisitesMet[i]);
            }
        } else {
            return request->reply(400, "application/json", "{\"error\":\"Invalid action\"}");
        }

        // Serialize the json
        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });
}