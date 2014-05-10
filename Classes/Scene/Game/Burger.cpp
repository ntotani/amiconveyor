#include "Burger.h"

Burger::Burger(vector<int> colors, bool isPotato)
:correctColors(colors)
,isPotato(isPotato)
,manas(vector<Mana*>())
,icons(vector<Sprite*>())
,popup(nullptr)
,burgerId(rand())
{
};

Burger* Burger::create(vector<int> colors, bool isPotato)
{
    auto *sprite = new Burger(colors, isPotato);
    if (sprite && sprite->initWithFile(isPotato ? "img/game_dish.png" : "img/game_bread_under.png"))
    {
        sprite->autorelease();
        sprite->popup = Scale9Sprite::create(Rect(2, 67, 192, 60), "img/game_recipe.png");
        auto popup = sprite->popup;
        auto cc = sprite->correctColors;
        int col = (cc.size() + 1) / 2;
        if (cc.size() == 2) {
            col = 2;
        }
        float wid = 26;
        float margin = 5;
        if (isPotato) {
            auto mana = Sprite::create("img/game_icon_mana_6.png");
            float x = wid * 0.5f + margin;
            float y = 46;
            mana->setPosition(Point(x, y));
            popup->addChild(mana);
            auto count = LabelTTF::create(StringUtils::format("x%d", (int)cc.size()), "", 10);
            count->setPositionX(mana->getContentSize().width);
            count->setColor(Color3B::BLACK);
            mana->addChild(count);
            sprite->icons.push_back(mana);
            col = 2;
        } else {
            for (int i = 0; i < cc.size(); i++) {
                auto mana = Sprite::create(StringUtils::format("img/game_icon_mana_%d.png", cc[i]));
                float x = wid * ((i % col) + 0.5f) + margin;
                float y = i < col ? 46 : 23;
                mana->setPosition(Point(x, y));
                popup->addChild(mana);
                auto num = LabelTTF::create(StringUtils::format("%02d", i + 1), "", 10);
                num->enableStroke(Color3B::BLACK, 3);
                num->setPosition(3, 3);
                num->setColor(Color3B::BLACK);
                mana->addChild(num);
                sprite->icons.push_back(mana);
            }
        }
        popup->setContentSize(Size(wid * col + (margin + 2) * 2, popup->getContentSize().height));
        auto size = popup->getContentSize();
        auto arrow = Sprite::create("img/game_arrow.png");
        arrow->setPosition(Point(sprite->getContentSize().width / 2, size.height + arrow->getContentSize().height / 2 - 2));
        popup->addChild(arrow);
        popup->setPosition(Point(size.width / 2, -size.height / 2 - arrow->getContentSize().height / 2));
        sprite->addChild(popup);
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool Burger::addMana(Mana *mana)
{
    bool ok = false;
    manas.push_back(mana);
    if (isPotato) {
        drawPotatoCount();
        mana->setPosition(Point(62.0f * rand() / RAND_MAX, 49.0f * rand() / RAND_MAX));
        ok = mana->color == 6 && manas.size() <= correctColors.size();
    } else {
        int idx = manas.size() - 1;
        if (idx < icons.size()) {
            auto icon = icons[idx];
            ok = correctColors[idx] == mana->color;
            auto mark = Sprite::create(StringUtils::format("img/game_icon_%s.png", ok ? "good" : "bad"));
            mark->setPosition(Point(icon->getContentSize()) / 2);
            icon->addChild(mark);
        }
        mana->setPosition(nextPoint());
    }
    mana->setOrderOfArrival(manas.size());
    mana->retain();
    mana->removeFromParent();
    addChild(mana);
    mana->release();
    mana->lastBurger = burgerId;
    mana->setLocalZOrder(0);
    return ok;
}

void Burger::drawPotatoCount()
{
    auto counter = static_cast<LabelTTF*>(icons[0]->getChildren().at(0));
    int cnt = 0;
    for (auto m : manas) {
        if (m->color == 6) {
            cnt++;
        }
    }
    counter->setString(StringUtils::format("x%d", (int)correctColors.size() - cnt));
}

Mana* Burger::popMana(Node* parent)
{
    auto mana = manas.back();
    mana->retain();
    mana->removeFromParent();
    parent->addChild(mana);
    mana->release();
    manas.pop_back();
    if (isPotato) {
        drawPotatoCount();
    } else {
        if (manas.size() < icons.size()) {
            auto icon = icons[manas.size()];
            icon->removeChild(icon->getChildren().at(icon->getChildrenCount() - 1), true);
        }
    }
    return mana;
}

bool Burger::validate()
{
    if (correctColors.size() != manas.size()) {
        return false;
    }
    if (isPotato) {
        for (auto m : manas) {
            if (m->color != 6) {
                return false;
            }
        }
        return true;
    }
    for (int i = 0; i < manas.size(); i++) {
        if (correctColors[i] != manas[i]->color) {
            return false;
        }
    }
    return true;
}

void Burger::jet()
{
    popup->runAction(Spawn::create(ScaleTo::create(0.2f, 0), MoveBy::create(0.2f, Point(0, popup->getContentSize().height / 2)), NULL));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto to = nextPoint();
    auto from = Point(to.x, visibleSize.height - getPositionY());

    auto bread = Sprite::create("img/game_bread.png");
    bread->setPosition(from);
    bread->runAction(MoveTo::create(0.2f, to));
    auto lid = Sprite::create("img/game_lid.png");
    lid->setPosition(from);
    lid->runAction(Sequence::create(DelayTime::create(0.1f), MoveTo::create(0.2f, to), NULL));
    auto comp = Sprite::create("img/game_compseal.png");
    comp->setPosition(from);
    comp->runAction(Sequence::create(DelayTime::create(0.2f), MoveTo::create(0.2f, to), NULL));
    if (!isPotato) addChild(bread);
    addChild(lid);
    addChild(comp);

    float vx = visibleSize.width;
    float vy = visibleSize.height / 2;
    auto delay = DelayTime::create(0.5f);
    auto move = MoveBy::create(0.5f, Point(-vx, vy));
    auto rotate = RotateBy::create(0.5f, -90);
    auto then = CallFunc::create([&]() {
        removeFromParent();
    });
    runAction(Sequence::create(delay, Spawn::create(move, rotate, NULL), then, NULL));
}

Point Burger::nextPoint()
{
    auto len = isPotato ? 2 : manas.size();
    return Point(getContentSize()) / 2 + Point(-5, 7 * len);
}
