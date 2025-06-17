#ifndef __PONG_GAME_HPP__
#define __PONG_GAME_HPP__

#include "game_settings.hpp"

class Game {
    private:
        GameSettings _settings;

    public:
        GameSettings *getSettings() {
            return &_settings;
        }
};

#endif