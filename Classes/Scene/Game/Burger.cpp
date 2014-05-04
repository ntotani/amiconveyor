#include "Burger.h"

Burger::Burger(IRandom *rnd)
{
    correctColors.push_back(rnd->next() % 3);
    correctColors.push_back(rnd->next() % 3);
}

Burger* Burger::create(const std::string& filename, IRandom *rnd)
{
    auto *sprite = new Burger(rnd);
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        auto cc = sprite->correctColors;
        auto str = StringUtils::format("%d,%d", cc[0], cc[1]);
        auto lab = LabelTTF::create(str, "", 12);
        lab->setColor(Color3B::YELLOW);
        sprite->addChild(lab);
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool Burger::validate()
{
    if (correctColors.size() != manas.size()) {
        return false;
    }
    for (int i = 0; i < manas.size(); i++) {
        if (correctColors[i] != manas[i]->color) {
            return false;
        }
    }
    return true;
}
