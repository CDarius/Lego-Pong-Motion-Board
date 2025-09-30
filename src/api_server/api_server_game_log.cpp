#include "api_server/api_server.hpp"

void ApiRestServer::setupGameLogController() {
    _server.on("/gamelog/read", [this](PsychicRequest *request) {
        // Check if a log exist
        uint32_t logLen = _gameLogger->getLogLen();
        if (logLen == 0)
            return request->reply(204); // No content

        // Start streaming response
        PsychicStreamResponse response(request, "application/json");
        response.beginSend();

        // Write JSON header
        response.print("{\"cols_names\":[\"cycle\",\"subCycle\",\"ballX\",\"ballY\",\"paddleL\",\"paddleR\"],");
        response.print("\"data\":[");
        yield(); // Allow background tasks to run

        bool skipFirstNotCompledtedCycle = true;

        // Write log entries
        for(uint32_t i = 0; i < logLen; i++) {
            GameLogEntry& entry = *_gameLogger->getLogEntry(i);
            if (skipFirstNotCompledtedCycle && entry.baseEntry.subCycle != 0) {
                // Skip the first not completed cycle
                continue;
            }
            skipFirstNotCompledtedCycle = false;

            if (entry.baseEntry.subCycle == 0) {
                // GameLogFirstEntry
                response.print("[");
                response.print(String(entry.firstEntry.cycle));
                response.print(",");
                response.print(String(entry.firstEntry.subCycle));
                response.print(",");
                response.print(String(entry.firstEntry.ballX, 4));
                response.print(",");
                response.print(String(entry.firstEntry.ballY, 4));
                response.print(",");
                response.print(String(entry.firstEntry.paddleL, 4));
                response.print(",");
                response.print(String(entry.firstEntry.paddleR, 4));
                response.print(",");
                response.print(String(entry.firstEntry.timestampMs));
                response.print("]");
            } else {
                response.print("[");
                response.print(String(entry.subEntry.cycle));
                response.print(",");
                response.print(String(entry.subEntry.subCycle));
                response.print(",");
                response.print(String(entry.subEntry.targetX, 4));
                response.print(",");
                response.print(String(entry.subEntry.targetY, 4));
                response.print(",");
                response.print(String(entry.subEntry.targetPaddleL, 4));
                response.print(",");
                response.print(String(entry.subEntry.targetPaddleR, 4));
                response.print(",");
                response.print(String(entry.subEntry.speedX, 4));
                response.print(",");
                response.print(String(entry.subEntry.speedY, 4));
                response.print("]");
            }

            if (i < logLen - 1) {
                response.print(",");
            }
            yield(); // Allow background tasks to run
        }

        response.print("]}");
        return response.endSend();
        
    });

    // Return log running status
    _server.on("/gamelog/running", [this](PsychicRequest *request) {
        // Prepare the answer
        JsonDocument doc;
        doc["running"] = _gameLogger->isLoggingActive();

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });
}