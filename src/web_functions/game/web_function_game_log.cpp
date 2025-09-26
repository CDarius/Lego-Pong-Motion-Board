#include "web_functions/game/web_function_game_log.hpp"

const char* WebFunctionGameLog::getName() const {
    switch (_mode)
    {
    case GameMode::PLAYER_VS_AI:
        return "game_log_pl_ai";
    case GameMode::AI_VS_AI:
        return "game_log_ai_ai";
    default:
        return "game_log_pl_pl";
    }
}

const char* WebFunctionGameLog::getTitle() const {
    switch (_mode)
    {
    case GameMode::PLAYER_VS_AI:
        return "Game Log: Player vs AI";
    case GameMode::AI_VS_AI:
        return "Game Log: AI vs AI";
    default:
        return "Game Log: Player vs Player";
    }
}

const char* WebFunctionGameLog::getDescription() const {
    switch (_mode)
    {
    case GameMode::PLAYER_VS_AI:
        return "Run a game Player vs AI with logging enabled";
    case GameMode::AI_VS_AI:
        return "Run a game AI vs AI with logging enabled";
    default:
        return "Run a game Player vs Player with logging enabled";
    }
}

uint16_t WebFunctionGameLog::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionGameLog::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "All axes must be homed";
    default: return nullptr;
    }
}

void WebFunctionGameLog::arePrerequisitesMet(bool* results) const {
    results[0] = _xAxis.referenced() && _yAxis.referenced() && _lAxis.referenced() && _rAxis.referenced();
}

WebFunctionExecutionStatus WebFunctionGameLog::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis max speed measurement procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionGameLog* self = static_cast<WebFunctionGameLog*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        // Start a game with logging enabled
        self->_game.run(GamePlayer::L, self->_mode, cancel_token, true);
        self->_game.getLogger()->stop();

        self->_status = WebFunctionExecutionStatus::Done;
        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionGameLog::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
