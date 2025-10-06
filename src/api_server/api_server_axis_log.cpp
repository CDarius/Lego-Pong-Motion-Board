#include "api_server/api_server.hpp"

void ApiRestServer::setupAxisLogController() {
    // Start logging
    _server.on("/axislog/start/*", [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        Motor* motor = getMotorByName(axis.c_str());
        if (!motor)
            return request->reply(400);
        
        // Get optional parameters
        uint32_t duration = 10 * 1000; // default 10s
        uint32_t div = 1; // default log every sample
        if (request->hasParam("duration")) {
            duration = request->getParam("duration")->value().toInt() * 1000; // convert s to ms
            if (duration < 1)
                return request->reply(400);

        }
        if (request->hasParam("div")) {
            div = request->getParam("div")->value().toInt();
            if (div < 1)
                return request->reply(400);
        }

        // Start the log
        pbio_error_t err = pbio_logger_start(motor->get_log(), duration, div);
        if (err != PBIO_SUCCESS)
            return request->reply(500);

        return request->reply(200);
    });

    // Stop logging
    _server.on("/axislog/stop/*", [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        Motor* motor = getMotorByName(axis.c_str());
        if (!motor)
            return request->reply(400);

        pbio_logger_stop(motor->get_log());
        return request->reply(200);
    });

    // Read the log
    _server.on("/axislog/read/*", [this](PsychicRequest *request)
    {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        Motor* motor = getMotorByName(axis.c_str());
        if (!motor)
            return request->reply(400);

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
        yield(); // Allow background tasks to run

        // Write column names
        response.print(",\"col_names\":[");
        for (uint32_t c = 0; c < cols; c++) {
            const char* col_name = pbio_logger_col_name(motor->get_log(), c);
            if (col_name) {
                response.print("\"");
                response.print(col_name);
                response.print("\"");
            }
            else {
                response.print("null");
            }
            if (c < cols - 1) {
                response.print(",");
            }
        }
        response.print("]");
        yield(); // Allow background tasks to run

        // Write column units
        response.print(",\"col_units\":[");
        for (uint32_t c = 0; c < cols; c++) {
            const char* col_unit = pbio_logger_col_unit(motor->get_log(), c);
            if (col_unit) {
                response.print("\"");
                response.print(col_unit);
                response.print("\"");
            }
            else {
                response.print("null");
            }
            if (c < cols - 1) {
                response.print(",");
            }
        }
        response.print("]");
        yield(); // Allow background tasks to run

        // Write the data array
        response.print(",\"data\":[");
        int32_t row_buffer[cols];
        int8_t yield_counter = 0;
        for (uint32_t r = 0; r < rows; r++) {
            // Get a log row
            pbio_error_t result = pbio_logger_read(motor->get_log(), r, row_buffer);
            if (result != PBIO_SUCCESS) {
                response.endSend();
                return request->reply(500);
            }

            // Write the row as a JSON array
            response.print("[");
            for (uint32_t c = 0; c < cols; c++) {
                int32_t value = row_buffer[c];
                response.print(value);
                if (c < cols - 1) {
                    response.print(",");
                }
            }
            response.print("]");
            if (r < rows - 1) {
                response.print(",");
            }

            // Yield every 10 rows to allow background tasks to run
            if (++yield_counter >= 10) {
                yield_counter = 0;
                yield(); // Allow background tasks to run
            }
        }
        response.print("]}");

        return response.endSend();
    });

    // Return log running status
    _server.on("/axislog/running/*", [this](PsychicRequest *request) {
        // Extract parameters from the URL
        String uri = request->uri();
        String axis = uriParam(uri, 2);

        // Validate mandatory "axis" parameter
        axis.toUpperCase();
        Motor* motor = getMotorByName(axis.c_str());
        if (!motor)
            return request->reply(400);
        
        // Prepare the answer
        JsonDocument doc;
        doc["axis"] = axis;
        doc["running"] = pbio_logger_is_active(motor->get_log());

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        return request->reply(200, "application/json", jsonResponse.c_str());
    });
}
