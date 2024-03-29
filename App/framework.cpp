#include "SDL.h"
#include "framework.hpp"
#include "texture/texture_manager.hpp"
#include "game_objects/hockey_striker.hpp"
#include "game_objects/hockey_puck.hpp"
#include "event/event_manager.hpp"
#include "logger/logger.hpp"
#include "xml/xml_loader.hpp"

#include <random>
#include <ctime>

Game* Game::m_game = nullptr;
bool isRunning = false;

Game::Game()
    :m_buttomBorder(0, 0, 0, 0),
    m_topBorder(0, 0, 0, 0),
    m_leftBorder(0, 0, 0, 0),
    m_rightBorder(0, 0, 0, 0),
    m_centralBorder(0, 0, 0, 0),
    m_topGate(0, 0, 0, 0),
    m_bottomGate(0, 0, 0, 0)
{
    m_window = Window::create("Big Dick Game");
    m_window->resize(WIDTH, HEIGHT);

    m_parser = new ConfigParser();
}

Game::~Game()
{
    delete m_parser;
}

Game* Game::getGame()
{
    if (nullptr == m_game) {
        m_game = new Game();
    }

    return m_game;
}

void Game::setLimits()
{
    float width = 0.0f, height = 0.0f, height_max = 0.0f;

    m_puck->setWidthLimits(0, m_config.global.width);
    m_puck->setHeightLimits(0, m_config.global.height);

    width = m_rightBorder.getX() - m_striker_1->getSize().first;
    m_striker_1->setWidthLimits(m_leftBorder.getWidth(), width);
    height = m_centralBorder.getY() + m_centralBorder.getHeight();
    height_max = m_buttomBorder.getY() - m_striker_1->getSize().second;
    m_striker_1->setHeightLimits(height, height_max);

    width = m_rightBorder.getX() - m_striker_2->getSize().first;
    m_striker_2->setWidthLimits(m_leftBorder.getWidth(), width);
    height = m_centralBorder.getY() - m_striker_2->getSize().second;
    m_striker_2->setHeightLimits(m_topBorder.getHeight(), height);

}

bool Game::init()
{
    bool result = true;
    int width = 0;
    int height = 0;

    result = m_parser->load(CONFIG_FOLDER "/config.xml");
    result &= m_parser->parse();
    if (result != true) {
        LOG_ERROR("Failed to parse config");

        return false;
    }

    m_config = dynamic_cast<ConfigParser*>(m_parser)->getConfig();

    m_window->resize(m_config.global.width, m_config.global.height);

    m_striker_1 = std::make_shared<HockeyStriker>();
    m_striker_2 = std::make_shared<HockeyStriker>();
    m_puck = std::make_shared<HockeyPuck>();

    m_striker_1->resize(m_config.scene.hockeyStriker.width,
                        m_config.scene.hockeyStriker.height);

    m_striker_2->resize(m_config.scene.hockeyStriker.width,
                        m_config.scene.hockeyStriker.height);

    m_puck->resize(m_config.scene.hockeyPuck.width,
                   m_config.scene.hockeyPuck.height);

    height = m_config.global.height - m_config.scene.bottomBorder.height;
    m_buttomBorder.move(0, (float) height);
    m_buttomBorder.resize(m_config.scene.bottomBorder.width,
                          m_config.scene.bottomBorder.height);

    m_topBorder.move(0, 0);
    m_topBorder.resize(m_config.scene.topBorder.width,
                       m_config.scene.topBorder.height);

    m_leftBorder.move(0, 0);
    m_leftBorder.resize(m_config.scene.leftBorder.width,
                        m_config.scene.leftBorder.height);

    width = m_config.global.width - m_config.scene.rightBorder.width;
    m_rightBorder.move((float) width, 0);
    m_rightBorder.resize(m_config.scene.rightBorder.width,
                         m_config.scene.rightBorder.height);

    width = m_config.scene.centralBorder.width;
    height = m_config.global.height / 2;
    height -= m_config.scene.centralBorder.height / 2;
    m_centralBorder.move(0, height);
    m_centralBorder.resize(width, m_config.scene.centralBorder.height);

    setLimits();

    width = m_config.global.width / 2 - m_striker_2->getSize().first / 2;
    height = m_topBorder.getHeight();
    m_striker_2->setPosition(Point2D(width, height));

    width = m_config.global.width / 2 - m_striker_1->getSize().first / 2;
    height = m_buttomBorder.getY() - m_striker_1->getSize().second;
    m_striker_1->setPosition(Point2D(width, height));

    width = m_config.global.width / 2 - m_config.scene.hockeyPuck.width / 2;
    height = m_config.global.height / 2 - m_config.scene.hockeyPuck.height / 2;
    m_puck->setPosition(Point2D((float) width, (float) height));


    EventManager::getManager()->subscribeOnEvent(
            std::make_pair(EventType::SDL_Event, SDL_MOUSEMOTION),
            m_striker_1);
    EventManager::getManager()->subscribeOnEvent(
            std::make_pair(EventType::GameEvent, GameEventType::Tick),
            m_puck);

    std::srand(std::time(nullptr));

    return true;
}

void Game::drawHitbox()
{
    TextureManager* texture_manager = TextureManager::getManager();

    texture_manager->drawRectangle(m_topBorder.getX(),
                                   m_topBorder.getY(),
                                   m_topBorder.getWidth(),
                                   m_topBorder.getHeight());

    texture_manager->drawRectangle(m_buttomBorder.getX(),
                                   m_buttomBorder.getY(),
                                   m_buttomBorder.getWidth(),
                                   m_buttomBorder.getHeight());

    texture_manager->drawRectangle(m_rightBorder.getX(),
                                   m_rightBorder.getY(),
                                   m_rightBorder.getWidth(),
                                   m_rightBorder.getHeight());

    texture_manager->drawRectangle(m_leftBorder.getX(),
                                   m_leftBorder.getY(),
                                   m_leftBorder.getWidth(),
                                   m_leftBorder.getHeight());

    texture_manager->drawRectangle(m_centralBorder.getX(),
                                   m_centralBorder.getY(),
                                   m_centralBorder.getWidth(),
                                   m_centralBorder.getHeight());
}

void Game::draw()
{
    SDL_Renderer* renderer = m_window->getRenderer();
    TextureManager* texture_manager = TextureManager::getManager();

    SDL_RenderClear(renderer);

    texture_manager->drawTextureByID((int)TexturesID::HockeyBackground, 0, 0,
                                     m_config.global.width,
                                     m_config.global.height);
    m_striker_1->draw();
    m_striker_2->draw();
    m_puck->draw();

    drawHitbox();

    SDL_RenderPresent(renderer);

}

void Game::keepObjectInBorder(std::shared_ptr<GameObject> object)
{
    int g_widht = m_config.global.width;
    int g_height = m_config.global.height;
    Point2D position = object->getPosition();

    if ((position.getX() + object->getSize().first) > m_rightBorder.getX()) {
        position.setX(m_rightBorder.getX() - object->getSize().first);
    }

    if (position.getX() < m_leftBorder.getWidth()) {
        position.setX(m_leftBorder.getWidth());
    }

    if ((position.getY() + object->getSize().second) > m_buttomBorder.getY()) {
        position.setY(m_buttomBorder.getY() - object->getSize().second);
    }

    if (position.getY() < m_topBorder.getHeight()) {
        position.setY(m_topBorder.getHeight());
    }

    object->setPosition(position);
}

void Game::hockeyStrikerLogic(std::shared_ptr<HockeyStriker> striker, float dt)
{
    /* TODO: */
}


std::tuple<float, float> Game::hockeyPuckHitStrikerLogic(
    std::shared_ptr<HockeyStriker> striker, float dt)
{
    auto puck = std::dynamic_pointer_cast<HockeyPuck>(m_puck);
    Vector2D accelerationVector = Vector2D(0, 0);
    Vector2D currentVelocity = puck->getVelocity();
    Vector2D directionVector = Vector2D(0, 0);
    Vector2D result = Vector2D(0, 0);
    float angle = 0;
    float velocity = 0;

    accelerationVector = striker->getAcceleration(dt);
    result = currentVelocity + accelerationVector;
    velocity = result.getY();

    directionVector = striker->getDirectionAngle();
    if (0 == directionVector.length()) {
        angle = puck->getAngle().getX();
    } else {
        angle = directionVector.getX();
    }

    return std::make_tuple(velocity, angle);
}

std::shared_ptr<HockeyStriker> Game::getActiveStriker()
{
    std::shared_ptr<HockeyStriker> striker;
    float height = 0;

    height = m_puck->getPosition().getY() + m_puck->getSize().second;

    if (height < m_centralBorder.getY()) {
        striker = std::dynamic_pointer_cast<HockeyStriker>(m_striker_2);
    } else {
        striker = std::dynamic_pointer_cast<HockeyStriker>(m_striker_1);
    }

    return striker;
}

void Game::hockeyPuckLogic(float dt)
{
    bool isHit = false;
    const float cAngle = 10;
    const float cFriction = 1;
    const float cVelocity = 40;
    const float velocityLimit = 90;
    float angle = 0;
    float friction = 0;
    float velocity = 0;
    std::shared_ptr<HockeyStriker> striker;
    std::shared_ptr<HockeyPuck> puck;

    striker = getActiveStriker();
    puck = std::dynamic_pointer_cast<HockeyPuck>(m_puck);

    if (puck->isHit(striker->getTopHitBox())) {
        isHit = true;

        std::tie(velocity, angle) = hockeyPuckHitStrikerLogic(striker, dt);

        if (velocity > 0) {
            velocity *= -1;
        }

        if (velocity < (velocityLimit * -1)) {
            velocity = velocityLimit * -1;
        }

        friction = cFriction;

        LOG_DEBUG("Hit strikerTop. Angle = %f\n", angle);
    } else if (puck->isHit(striker->getMiddleHitBox())) {
        isHit = true;

        std::tie(velocity, angle) = hockeyPuckHitStrikerLogic(striker, dt);

        if (velocity < 0 && velocity < (velocityLimit * -1)) {
            velocity = velocityLimit * -1;
        } else if (velocity > velocityLimit) {
            velocity = velocityLimit;
        }

        friction = puck->getFriction().getY();

        LOG_DEBUG("Hit strikerMiddle. Angle = %f\n", angle);
    } else if (puck->isHit(striker->getBottomHitBox())) {
        isHit = true;

        std::tie(velocity, angle) = hockeyPuckHitStrikerLogic(striker, dt);

        if (velocity < 0) {
            velocity *= -1;
        }

        if (velocity > velocityLimit) {
            velocity = velocityLimit;
        }

        friction = puck->getFriction().getY();
        if (friction > 0) {
            friction *= -1;
        } else if (0 == friction) {
            friction = cFriction * -1;
        }

        LOG_DEBUG("Hit strikerBottom. Angle = %f\n", angle);
    }

    if (puck->isHit(m_topBorder)) {
        isHit = true;
        velocity = puck->getVelocity().getY();
        if (velocity < 0) {
            velocity = velocity * -1;
        }

        friction = puck->getFriction().getY();
        if (friction > 0) {
            friction *= -1;
        }

        angle = puck->getAngle().getX();
        LOG_DEBUG("Hit topBorder\n");
    }

    if (puck->isHit(m_buttomBorder)) {
        isHit = true;
        velocity = puck->getVelocity().getY();
        if (velocity > 0) {
            velocity = velocity * -1;
        }

        friction = puck->getFriction().getY();
        if (friction < 0) {
            friction *= -1;
        }

        angle = puck->getAngle().getX();
        LOG_DEBUG("Hit bottomBorder\n");
    }

    if (puck->isHit(m_rightBorder)) {
        isHit = true;
        angle = puck->getAngle().getX() * -1;
        velocity = puck->getVelocity().getY();
        friction = puck->getFriction().getY();
        LOG_DEBUG("Hit left border\n");
    }

    if (puck->isHit(m_leftBorder)) {
        isHit = true;
        angle = puck->getAngle().getX() * -1;
        velocity = puck->getVelocity().getY();
        friction = puck->getFriction().getY();
        LOG_DEBUG("Hit left border\n");
    }

    if (isHit) {
        LOG_DEBUG("Friction = %f\n", friction);
        puck->setAngle(Vector2D(angle, 0));
        puck->setFriction(Vector2D(0, friction));
        puck->setVelocity(Vector2D(0, velocity));
    }
}

void Game::update(float dt)
{
    m_striker_1->update(dt);

    m_puck->update(dt);
    hockeyPuckLogic(dt);

    hockeyStrikerLogic(std::dynamic_pointer_cast<HockeyStriker>(m_striker_1), dt);
}

bool AirHockey::init()
{
    Game* game = Game::getGame();
    TextureManager* manager = TextureManager::getManager();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    manager->setRenderer(game->getWindow()->getRenderer());

    isRunning = true;
    isRunning &= manager->addTextureByPath((int)TexturesID::HockeyBackground,
                                           ASSETS_FOLDER "/BackGround.png");
    isRunning &= manager->addTextureByPath((int)TexturesID::HockeyStriker,
                                           ASSETS_FOLDER "/Striker.png");
    isRunning &= manager->addTextureByPath((int)TexturesID::HockeyPuck,
                                           ASSETS_FOLDER "/Puck.png");

    isRunning &= game->init();

    return isRunning;
}

void AirHockey::handleEvent()
{
    SDL_Event sdl_event;
    std::shared_ptr<Event> event = nullptr;

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT:
                AirHockey::close();

                isRunning = false;

                break;
            default:
                event = std::make_shared<Event>();
                event->type = EventType::SDL_Event;
                event->data.sdl_event = sdl_event;

                EventManager::pushEvent(event);

                break;
        }
    }

    event = std::make_shared<Event>();
    event->type = EventType::GameEvent;
    event->data.game_event.type = GameEventType::Tick;
    EventManager::pushEvent(event);
}

bool AirHockey::update(float dt)
{
    Game* game = Game::getGame();

    game->update(dt);
    game->draw();

    return isRunning;
}

void AirHockey::close()
{
    EventManager::getManager()->stop();
    SDL_Quit();
}
