#include "api_server/api_server.hpp"

String ApiRestServer::uriParam(const String& uri, uint8_t position) {
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

void ApiRestServer::begin(Settings* settings, WebFunctions* webFunctions) {
    _settings = settings;
    _webFunctions = webFunctions;

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
    setupWebFunctionController();
}
