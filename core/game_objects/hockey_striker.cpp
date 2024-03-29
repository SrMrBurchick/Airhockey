#include "hockey_striker.hpp"
#include "texture/texture_manager.hpp"

#define HITBOX_TOP_BOTTOM_HEIGHT 20
#define HITBOX_MIDDLE_HEIGHT (HITBOX_TOP_BOTTOM_HEIGHT << 1)

HockeyStriker::HockeyStriker()
    : PhysicObject(), GameObject(TexturesID::HockeyStriker, 0, 0),
    m_topHitBox(0, 0, 0, 0),
    m_middleHitBox(0, 0, 0, 0),
    m_buttomHitBox(0, 0, 0, 0),
    m_nextPosition(0, 0),
    m_previousPosition(0, 0),
    m_directionStartPosition(0, 0)
{
    TextureManager* manager = TextureManager::getManager();
    Texture const* texture =
        manager->getTextureByID((int)TexturesID::HockeyStriker);

    if (nullptr != texture) {
        int heigth = texture->getHeight();
        int width = texture->getWidth();
        m_topHitBox.resize(width, HITBOX_TOP_BOTTOM_HEIGHT);
        m_buttomHitBox.resize(width, HITBOX_TOP_BOTTOM_HEIGHT);
        m_middleHitBox.resize(width, heigth - HITBOX_MIDDLE_HEIGHT);
    }
}

void HockeyStriker::drawHitbox()
{

    TextureManager::getManager()->drawRectangle(m_topHitBox.getX(),
                                                m_topHitBox.getY(),
                                                m_topHitBox.getWidth(),
                                                m_topHitBox.getHeight());
    TextureManager::getManager()->drawRectangle(m_middleHitBox.getX(),
                                                m_middleHitBox.getY(),
                                                m_middleHitBox.getWidth(),
                                                m_middleHitBox.getHeight());
    TextureManager::getManager()->drawRectangle(m_buttomHitBox.getX(),
                                                m_buttomHitBox.getY(),
                                                m_buttomHitBox.getWidth(),
                                                m_buttomHitBox.getHeight());
}

void HockeyStriker::setPosition(Point2D position)
{
    int width, heigth;
    std::tie(width, heigth) = getSize();
    Point2D currentPosition = getPosition();

    if (m_previousPosition == position || currentPosition == position) {
        m_directionStartPosition = m_previousPosition;

        return;
    }

    m_previousPosition = getPosition();

    position = limitPosition(position);

    m_topHitBox.move(position.getX(), position.getY());
    m_middleHitBox.move(position.getX(),
                        position.getY() + HITBOX_TOP_BOTTOM_HEIGHT);
    m_buttomHitBox.move(position.getX(),
                        position.getY() + heigth - HITBOX_TOP_BOTTOM_HEIGHT);

    GameObject::setPosition(position);
}

bool HockeyStriker::resize(int width, int height)
{
    m_topHitBox.resize(width, HITBOX_TOP_BOTTOM_HEIGHT);
    m_middleHitBox.resize(width, height - HITBOX_MIDDLE_HEIGHT);
    m_buttomHitBox.resize(width, HITBOX_TOP_BOTTOM_HEIGHT);

    return GameObject::resize(width, height);
}

void HockeyStriker::update(float dt)
{
    setPosition(m_nextPosition);

    PhysicObject::update(dt);
}

void HockeyStriker::handleSDLEvent(SDL_Event& event)
{
    switch (event.type) {
        case SDL_MOUSEMOTION:
            m_nextPosition.setX(event.motion.x);
            m_nextPosition.setY(event.motion.y);


            break;
        default:
            break;
    }
}

void HockeyStriker::handleEvent(Event* event)
{
    switch (event->type) {
        case EventType::SDL_Event:
            handleSDLEvent(event->data.sdl_event);

            break;
        default:
            break;
    }
}

Vector2D HockeyStriker::getDirectionAngle() const
{
    Point2D currentPosition = getPosition();
    Point2D zero = Point2D(0, 0);
    Vector2D directionVector = currentPosition - m_previousPosition;

    if (currentPosition == m_previousPosition) {
        LOG_DEBUG("Equal!\n");
        return Vector2D(0, 0);
    }

    return directionVector.normalize();
}

Vector2D HockeyStriker::getAcceleration(float dt) const
{
    Vector2D acceleration = Vector2D(0, 0);
    Point2D currentPosition = getPosition();

    acceleration = currentPosition - m_directionStartPosition;
    acceleration /= dt;

    LOG_DEBUG("Acceleration: x = %f, y = %f\n", acceleration.getX(),
              acceleration.getY());

    return acceleration;
}
