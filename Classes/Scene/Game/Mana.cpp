#include "Mana.h"

#define MANA_LEN 70

Mana::Mana(Node* home, int color)
:home(home)
,color(color)
,lastBurger(-1)
,velocity(Point::ZERO)
,prefBurger(-1)
{
}

Mana* Mana::create(Node* home, int color)
{
    auto *sprite = new Mana(home, color);
    auto filename = StringUtils::format("img/game_mana_%d.png", color);
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

Rect Mana::getBound() const
{
    return Rect(getPositionX() - MANA_LEN / 2, getPositionY() - MANA_LEN / 2, MANA_LEN, MANA_LEN);
}
