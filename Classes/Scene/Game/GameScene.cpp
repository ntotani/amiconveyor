#include "GameScene.h"
#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "cocosbuilder/CCNodeLoaderLibrary.h"
#include "../Result/ResultScene.h"
#include "spine/Json.h"
#include "../Pause/PauseScene.h"
#include "../../Common/GamePlatform.h"

//#define BGM "sound/Super_Gamer_Boy_offvocal.mp3"
#define BGM "sound/Vigour.mp3"
#define HIGH_SCORE "highScore"

using namespace CocosDenshion;

GameScene::GameScene()
:laneA(nullptr)
,laneB(nullptr)
,rollersA(vector<Sprite*>())
,rollersB(vector<Sprite*>())
,scoreLabel(nullptr)
,manas(vector<Node*>())
,maxHeight(0)
,pause(nullptr)
,rnd(new RandomImpl())
,score(0)
,levelCounter(0)
,flickCounter(0)
,touchBegan(Point::ZERO)
,spawnCounter(0)
,coolDown(0.5f)
,tutorial(true)
,ccbAnimationManager(nullptr)
,flyingManas(list<Mana*>())
,burgers(list<Burger*>())
,levels(list<Level>())
,currentLevel(Level())
,pausing(false)
,pauseSmoke(nullptr)
,highScore(0)
,highScoreLabel(nullptr)
,onPotato(false)
,onLane(false)
,crown(nullptr)
{
    for (int i = 0; i < 8; i++) {
        manas.push_back(nullptr);
    }
    for (int i = 0; i < 4; i++) {
        rollersA.push_back(nullptr);
        rollersB.push_back(nullptr);
    }
}

GameScene::~GameScene()
{
    delete rnd;
    SimpleAudioEngine::getInstance()->unloadEffect("sound/se_flick.mp3");
    SimpleAudioEngine::getInstance()->unloadEffect("sound/se_ng.mp3");
    SimpleAudioEngine::getInstance()->unloadEffect("sound/se_ob.mp3");
    SimpleAudioEngine::getInstance()->unloadEffect("sound/se_ok.mp3");
}

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("GameScene", GameSceneLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    ccbReader->autorelease();
    auto layer = ccbReader->readNodeGraphFromFile("ccbi/GameScene.ccbi");
    static_cast<GameScene*>(layer)->initManas();
    static_cast<GameScene*>(layer)->ccbAnimationManager = ccbReader->getAnimationManager();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic(BGM);
    SimpleAudioEngine::getInstance()->preloadEffect("sound/se_flick.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("sound/se_ng.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("sound/se_ob.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("sound/se_ok.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("sound/se_jet.mp3");

    highScore = UserDefault::getInstance()->getIntegerForKey(HIGH_SCORE, 0);
    auto levelFile = highScore > 0 && rnd->next() % 2 == 0 ? "level.json" : "level_short.json";

    auto str = FileUtils::getInstance()->getStringFromFile(levelFile);
    auto json = Json_create(str.c_str());
    coolDown = Json_getFloat(json, "coolDown", 0.5f);
    auto events = Json_getItem(json, "events");
    auto it = events->child;
    while (it) {
        Level l;
        l.delta = Json_getFloat(it, "delta", -1);
        l.height = Json_getInt(it, "height", -1);
        l.speed = Json_getFloat(it, "speed", -1);
        l.freq = Json_getFloat(it, "freq", -1);
        l.potatoNum = Json_getInt(it, "potato", 0);
        l.lane = Json_getInt(it, "lane", 0);
        levels.push_back(l);
        it = it->next;
    }

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    scheduleUpdate();
    return true;
}

bool GameScene::onAssignCCBMemberVariable(Ref* pTarget, const char* pMemberVariableName, Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "laneA", Node*, laneA);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "laneB", Node*, laneB);
    for (int i = 0; i < 8; i++) {
        CCB_MEMBERVARIABLEASSIGNER_GLUE(this, StringUtils::format("mana%d", i).c_str(), Node*, manas[i]);
    }
    for (int i = 0; i < 4; i++) {
        CCB_MEMBERVARIABLEASSIGNER_GLUE(this, StringUtils::format("rollerA%d", i).c_str(), Sprite*, rollersA[i]);
        CCB_MEMBERVARIABLEASSIGNER_GLUE(this, StringUtils::format("rollerB%d", i).c_str(), Sprite*, rollersB[i]);
    }
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "scoreLabel", LabelTTF*, scoreLabel);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "pause", Sprite*, pause);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "crown", Sprite*, crown);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "pauseSmoke", Sprite*, pauseSmoke);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "highScoreLabel", LabelTTF*, highScoreLabel);
    return true;
}

SEL_MenuHandler GameScene::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName)
{
    return NULL;
}

Control::Handler GameScene::onResolveCCBCCControlSelector(Ref * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "onRetry", GameScene::onRetry);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "onBack", GameScene::onBack);
    return NULL;
}

void GameScene::onRetry(cocos2d::Ref *sender, Control::EventType type)
{
    Director::getInstance()->replaceScene(GameScene::createScene());
}

void GameScene::onBack(cocos2d::Ref *sender, Control::EventType type)
{
    pausing = false;
    ccbAnimationManager->runAnimationsForSequenceNamed("game");
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void GameScene::initManas()
{
    for (int i = 0; i < manas.size(); i++) {
        auto mana = Mana::create(manas[i], i);
        mana->setPosition(0, -300);
        mana->runAction(Sequence::create(MoveTo::create(1, Point::ZERO), CallFuncN::create([&](Node* n) {
            auto m = static_cast<Mana*>(n);
            flyingManas.push_back(m);
        }), NULL));
        manas[i]->addChild(mana);
    }
    drawScore();
    pauseSmoke->setLocalZOrder(1000);

    auto b = Burger::create({0, 1, 2}, false);
    b->addMana(Mana::create(manas[0], 0));
    b->addMana(Mana::create(manas[1], 1));
    b->setPosition(laneA->getPosition());
    b->setPositionX(Director::getInstance()->getVisibleSize().width / 2);
    addChild(b);
    burgers.push_back(b);
    highScoreLabel->setString(StringUtils::format("%04d", highScore));
}

bool GameScene::onTouchBegan(Touch *touch, Event *event)
{
    if (pausing) {
        return false;
    }
    flickCounter = 0;
    touchBegan = touch->getLocation();
    if (pause->getBoundingBox().containsPoint(touchBegan)) {
        pausing = true;
        ccbAnimationManager->runAnimationsForSequenceNamed("pause");
        SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
        return false;
    }
    if (crown->getBoundingBox().containsPoint(touchBegan)) {
        GamePlatform::show();
        return false;
    }
    for (auto mana : flyingManas) {
        if (mana->velocity.getLengthSq() <= 0 && mana->getBound().containsPoint(touchBegan - mana->home->getPosition())) {
            mana->setScale(1.2);
            break;
        }
    }
    return true;
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
    if (flickCounter == 0) {
        flickCounter = 0.1f;
    }
    auto touchEnd = touch->getLocation();
    auto dir = (touchEnd - touchBegan).normalize();
    auto vel = dir * 100 / flickCounter;
    for (auto mana : flyingManas) {
        if (mana->getBound().containsPoint(touchBegan - mana->home->getPosition()) && mana->velocity == Point::ZERO) {
            mana->prefBurger = getPrefBurger(dir, mana->color);
            mana->velocity = vel;
            mana->setScale(1);
            mana->setPosition(mana->home->getPosition());
            mana->retain();
            mana->removeFromParent();
            addChild(mana, 100);
            mana->release();
            auto reload = Mana::create(mana->home, mana->color);
            spawnMana(reload);
            mana->home->addChild(reload);
            SimpleAudioEngine::getInstance()->playEffect("sound/se_flick.mp3");
            return;
        }
    }
    for (auto b : burgers) {
        if (b->getBound().containsPoint(touchBegan)) {
            if (b->manas.empty()) {
                continue;
            }
            auto mana = b->popMana(this);
            mana->setPosition(touchBegan);
            mana->velocity = vel;
            mana->prefBurger = getPrefBurger(dir, mana->color, b->burgerId);
            flyingManas.push_back(mana);
            SimpleAudioEngine::getInstance()->playEffect("sound/se_flick.mp3");
            break;
        }
    }
}

int GameScene::getPrefBurger(const Point& dir, int manaColor, int expBurgerId) const
{
    auto ab = dir * 1000;
    auto l = ab.getLength();
    int prefBurger = -1;
    float minScore = 2000000;
    for (auto b : burgers) {
        auto ap = b->getPosition() - touchBegan;
        float dist = max(ab.cross(ap), ap.cross(ab)) / l;
        int correct = b->nextColor() == manaColor ? 0 : 1000000;
        float left = b->getPositionX() * 1000;
        float score = correct + left + dist;
        if (score < minScore && dist < 70 && b->burgerId != expBurgerId) {
            minScore = score;
            prefBurger = b->burgerId;
        }
    }
    return prefBurger;
}

void GameScene::update(float dt)
{
    if (pausing) {
        return;
    }
    flickCounter += dt;
    checkLevel(dt);
    updateManas(dt);
    updateBurgers(dt);
    if (tutorial) {
        return;
    }
    spawnCounter -= dt;
    if (spawnCounter < 0) {
        spawnCounter = currentLevel.freq;
        vector<int> correctColors;
        bool isPotato = false;
        if (currentLevel.potatoNum > 0 && (rnd->next() % 8 == 6 || onPotato)) {
            isPotato = true;
            for (int i = 0; i < currentLevel.potatoNum; i++) {
                correctColors.push_back(6);
            }
        } else {
            for (int i = 0; i < currentLevel.height; i++) {
                correctColors.push_back(rnd->next() % 6);
            }
        }
        auto b = Burger::create(correctColors, isPotato);
        auto lane = currentLevel.lane && (rnd->next() % 2 == 0 || onLane) ? laneB : laneA;
        b->setPosition(lane->getPosition());
        addChild(b);
        burgers.push_back(b);
        int order = burgers.size();
        for (auto bb : burgers) {
            bb->setLocalZOrder(order);
            order--;
        }
        onPotato = onLane = false;
    }
    for (auto r : rollersA) {
        r->setRotation(r->getRotation() - 2 * currentLevel.speed * dt);
    }
    if (currentLevel.lane) {
        for (auto r : rollersB) {
            r->setRotation(r->getRotation() - 2 * currentLevel.speed * dt);
        }
    }
}

void GameScene::checkLevel(float dt)
{
    if (tutorial || levels.empty()) {
        return;
    }
    levelCounter += dt;
    auto nextLevel = levels.front();
    if (levelCounter >= nextLevel.delta) {
        levelCounter = 0;
        currentLevel.height = nextLevel.height != -1 ? nextLevel.height : currentLevel.height;
        currentLevel.speed = nextLevel.speed != -1 ? nextLevel.speed : currentLevel.speed;
        currentLevel.freq = nextLevel.freq != -1 ? nextLevel.freq : currentLevel.freq;
        currentLevel.potatoNum = nextLevel.potatoNum > 0 ? nextLevel.potatoNum : currentLevel.potatoNum;
        currentLevel.lane |= nextLevel.lane;
        levels.pop_front();
        if (nextLevel.potatoNum > 0) {
            onPotato = true;
        }
        if (nextLevel.lane) {
            onLane = true;
        }
    }
}

void GameScene::updateManas(float dt)
{
    for (auto it = flyingManas.begin(); it != flyingManas.end();) {
        auto e = *it;
        e->setPosition(e->getPosition() + e->velocity * dt);
        if (!getBoundingBox().intersectsRect(e->getBoundingBox())) {
            e->removeFromParent();
            SimpleAudioEngine::getInstance()->playEffect("sound/se_ob.mp3");
            it = flyingManas.erase(it);
        } else {
            it++;
        }
    }
}

void GameScene::updateBurgers(float dt)
{
    for (auto it = burgers.begin(); it != burgers.end();) {
        auto burger = *it;
        if (!tutorial) {
            auto vec = Point(-currentLevel.speed * dt, 0);
            burger->setPosition(burger->getPosition() + vec);
        }
        for (auto itt = flyingManas.begin(); itt != flyingManas.end(); itt++) {
            auto fm = *itt;
            if (fm->getBoundingBox().intersectsRect(burger->getBound()) && fm->lastBurger != burger->burgerId && (fm->prefBurger == -1 || fm->prefBurger == burger->burgerId)) {
                bool ok = burger->addMana(fm);
                SimpleAudioEngine::getInstance()->playEffect(StringUtils::format("sound/se_%s.mp3", ok ? "ok" : "ng").c_str());
                flyingManas.erase(itt);
                if (tutorial) {
                    SimpleAudioEngine::getInstance()->playBackgroundMusic(BGM, true);
                    ccbAnimationManager->runAnimationsForSequenceNamed("game");
                }
                tutorial = false;
                break;
            }
        }
        if (burger->validate()) {
            burger->jet();
            SimpleAudioEngine::getInstance()->playEffect("sound/se_jet.mp3");
            score++;
            drawScore();
            if (burger->correctColors.size() > maxHeight) {
                maxHeight = burger->correctColors.size();
                ResultScene::setManas(burger->correctColors);
            }
            it = burgers.erase(it);
        } else if (burger->getBoundingBox().getMaxX() < 0) {
            if (burger->manas.size() > maxHeight) {
                maxHeight = burger->manas.size();
                vector<int> manaColors;
                for (auto m : burger->manas) {
                    manaColors.push_back(m->color);
                }
                ResultScene::setManas(manaColors);
            }
            if (score > highScore) {
                UserDefault::getInstance()->setIntegerForKey(HIGH_SCORE, score);
                UserDefault::getInstance()->flush();
                GamePlatform::reportScore(highScore);
            }
            Director::getInstance()->replaceScene(ResultScene::createScene());
            it = burgers.erase(it);
        } else {
            it++;
        }
    }
}

void GameScene::spawnMana(Mana *e)
{
    /*
    e->setPosition(Point(-1000, -1000));
    e->runAction(Sequence::create(DelayTime::create(coolDown), CallFuncN::create([&](Node* e) {
        auto mana = static_cast<Mana*>(e);
        mana->setPosition(mana->home->getPosition());
        mana->velocity = Point::ZERO;
        flyingManas.push_back(mana);
    }), NULL));
     */
    //e->setPosition(e->home->getPosition());
    e->velocity = Point::ZERO;
    flyingManas.push_back(e);
}

void GameScene::drawScore()
{
    scoreLabel->setString(StringUtils::format("%04d", score));
}
