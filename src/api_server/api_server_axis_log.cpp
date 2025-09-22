#include "api_server/api_server.hpp"

void ApiRestServer::setupAxisLogController() {
    // Start logging
    _server.on("/axislog/start", [this](PsychicRequest *request)
    {
        // Parse the JSON body
        String body = request->body();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, body);
        if (error)
            return request->reply(400);

        // Validate mandatory "axis" parameter
        const char* axis = doc["axis"];
        Motor* motor = getMotorByName(axis);
        if (!motor) {
            return request->reply(400);
        }

        // Get optional parameters
        int32_t duration = doc["duration"] | 10000; // default 10s
        int32_t div = doc["div"] | 1; // default log every sample

        // Start the log
        pbio_error_t err = pbio_logger_start(motor->get_log(), duration, div);
        if (err != PBIO_SUCCESS)
            return request->reply(500);

        return request->reply(200);
    });

    // Stop logging
    _server.on("/axislog/stop", [this](PsychicRequest *request)
    {
        // Parse the JSON body
        String body = request->body();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, body);
        if (error)
            return request->reply(400);

        // Validate mandatory "axis" parameter
        const char* axis = doc["axis"];
        Motor* motor = getMotorByName(axis);
        if (!motor) {
            return request->reply(400);
        }

        pbio_logger_stop(motor->get_log());
        return request->reply(200);
    });

    // Read the log
    _server.on("/axislog/read", [this](PsychicRequest *request)
    {
        // Parse the JSON body
        String body = request->body();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, body);
        if (error)
            return request->reply(400);

        // Validate mandatory "axis" parameter
        const char* axis = doc["axis"];
        Motor* motor = getMotorByName(axis);
        if (!motor) {
            return request->reply(400);
        }

        int32_t rows = pbio_logger_rows(motor->get_log());
        int32_t cols = pbio_logger_cols(motor->get_log());

        if (rows == 0 || cols == 0)
            return request->reply(204); // No content

        // Start streaming response
        PsychicStreamResponse response(request, "application/json");
        response.beginSend();

        // Write JSON header
        response.print("{\"rows\":");
        response.print(String(rows));
        response.print(",\"cols\":");
        response.print(String(cols));
        response.print(",\"data\":[");
        
        // Write the data array
        uint32_t log_array_index = 0;
        for (uint32_t r = 0; r < rows; r++) {
            response.print("[");
            for (uint32_t c = 0; c < cols; c++) {
                uint32_t value = motor->get_log()->data[log_array_index++];
                response.print(value);
                if (c < cols - 1) {
                    response.print(",");
                }
            }
            response.print("]");
            if (r < rows - 1) {
                response.print(",");
            }

            yield(); // Allow background tasks to run
        }
        response.print("]}");

        return response.endSend();
    });
}
