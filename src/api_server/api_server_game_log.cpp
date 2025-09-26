#include "api_server/api_server.hpp"

void ApiRestServer::setupGameLogController() {
    _server.on("/game/log", [this](PsychicRequest *request) {
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
            if (skipFirstNotCompledtedCycle && entry.subCycle != 0) {
                // Skip the first not completed cycle
                continue;
            }
            skipFirstNotCompledtedCycle = false;

            response.print("[");
            response.print(String(entry.cycle));
            response.print(",");
            response.print(String(entry.subCycle));
            response.print(",");
            response.print(String(entry.ballX, 4));
            response.print(",");
            response.print(String(entry.ballY, 4));
            response.print(",");
            response.print(String(entry.paddleL, 4));
            response.print(",");
            response.print(String(entry.paddleR, 4));
            response.print("]");
            if (i < logLen - 1) {
                response.print(",");
            }
            yield(); // Allow background tasks to run
        }

        response.print("]}");
        return response.endSend();
        
    });
}