#ifndef __FRAMEWORK_HPP__
#define __FRAMEWORK_HPP__

#include "game_objects/game_object.hpp"
#include "game_objects/hockey_striker.hpp"
#include "window/window.hpp"
#include "parser/xml/config_parser.hpp"

#include <memory>

#define WIDTH   700
#define HEIGHT  1100

class Game {
public:
    ~Game();

    static Game* getGame();

    Window* getWindow() { return m_window; }

    void draw();
    void update(float dt);

    bool init();

protected:
    Game();

private:
    void keepObjectInBorder(std::shared_ptr<GameObject> object);
    void hockeyPuckLogic(float dt);
    void hockeyStrikerLogic(std::shared_ptr<HockeyStriker> striker, float dt);
    void playerLogic();
    void botLogic();
    void drawHitbox();
    void setLimits();

    std::tuple<float, float> hockeyPuckHitStrikerLogic(
            std::shared_ptr<HockeyStriker> striker, float dt);
    std::tuple<float, float> hockeyPuckHitBorderLogic(HitBox2D border);
    std::shared_ptr<HockeyStriker> getActiveStriker();

    Window* m_window;

    std::shared_ptr<GameObject> m_striker_1;
    std::shared_ptr<GameObject> m_striker_2;
    std::shared_ptr<GameObject> m_puck;

    HitBox2D m_topBorder;
    HitBox2D m_buttomBorder;
    HitBox2D m_leftBorder;
    HitBox2D m_rightBorder;
    HitBox2D m_centralBorder;

    HitBox2D m_topGate;
    HitBox2D m_bottomGate;

    XMLLoader* m_parser;
    Config m_config;

    static Game* m_game;
};

namespace AirHockey {
    bool init();
    void handleEvent();
    bool update(float dt);
    void close();
}

#endif /* __FRAMEWORK_HPP__ */
