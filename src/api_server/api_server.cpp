#include "api_server/api_server.hpp"

String RemoveQueryString(const String& uri) {
    int queryIndex = uri.indexOf('?');
    if (queryIndex != -1) {
        return uri.substring(0, queryIndex);
    }
    return uri;
}

String ApiRestServer::uriParam(const String& uri, uint8_t position) {
    uint8_t count = 0;
    int start = (uri[0] == '/') ? 1 : 0; // Skip leading '/'
    int end = uri.indexOf('/', start);
    
    while (end != -1) {
        if (count == position) {
            return RemoveQueryString(uri.substring(start, end));
        }
        count++;
        start = end + 1;
        end = uri.indexOf('/', start);
    }
    
    // Last segment (or only segment if no '/'
    if (count == position) {
        return RemoveQueryString(uri.substring(start));
    }
    
    return ""; // Return empty string if position is out of bounds
}

void ApiRestServer::begin(Settings* settings, WebFunctions* webFunctions, Motor* xMotor, Motor* yMotor, Motor* lMotor, Motor* rMotor, GameLogger* gameLogger) {
    _settings = settings;
    _webFunctions = webFunctions;
    _XMotor = xMotor;
    _YMotor = yMotor;
    _LMotor = lMotor;
    _RMotor = rMotor;
    _gameLogger = gameLogger;

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
    setupAxisLogController();
    setupAxisInfoController();
    setupGameLogController();
}

Motor* ApiRestServer::getMotorByName(const char* name) {
    if (strcmp(name, "X") == 0)
        return this->_XMotor;
    else if (strcmp(name, "Y") == 0)
        return this->_YMotor;
    else if (strcmp(name, "L") == 0)
        return this->_LMotor;
    else if (strcmp(name, "R") == 0)
        return this->_RMotor;
    else
        return nullptr;
}
