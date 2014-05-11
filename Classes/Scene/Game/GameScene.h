#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "cocosbuilder/CCLayerLoader.h"
#include "cocosbuilder/CCBSelectorResolver.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCBAnimationManager.h"
#include "UIButton.h"
#include "Burger.h"
#include "Mana.h"
#include "../../Common/RandomImpl.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

class Level
{
public:
    float delta;
    int height;
    float speed;
    float freq;
    int potatoNum;
    bool lane;
    Level() {
        delta = 0;
        height = 2;
        speed = 50.0f;
        freq = 3.0f;
        potatoNum = 0;
        lane = false;
    };
};

class GameScene : public Layer, public CCBMemberVariableAssigner, public CCBSelectorResolver
{
    Node* laneA;
    Node* laneB;
    float flickCounter;
    Point touchBegan;
    vector<Node*> manas;
    list<Mana*> flyingManas;
    list<Burger*> burgers;
    float spawnCounter;
    RandomImpl* rnd;
    LabelTTF* scoreLabel;
    int score;
    list<Level> levels;
    float levelCounter;
    Level currentLevel;
    float coolDown;
    bool tutorial;
    CCBAnimationManager *ccbAnimationManager;
    int maxHeight;
    Sprite* pause;
    bool pausing;
    Sprite* pauseSmoke;
    int highScore;
    LabelTTF* highScoreLabel;
    vector<Sprite*> rollersA;
    vector<Sprite*> rollersB;
public:
    static Scene* createScene();
    virtual bool init();
    void initManas();
    GameScene();
    ~GameScene();
    CREATE_FUNC(GameScene);
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);

    virtual bool onAssignCCBMemberVariable(Ref* pTarget, const char* pMemberVariableName, Node* pNode);
    virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName);
    virtual Control::Handler onResolveCCBCCControlSelector(Ref * pTarget, const char* pSelectorName);

    void update(float dt);
    void checkLevel(float dt);
    void updateManas(float dt);
    void updateBurgers(float dt);
    void spawnMana(Mana* mana);
    void drawScore();
    void onRetry(Ref* sender, Control::EventType type);
    void onBack(Ref* sender, Control::EventType type);
    int getPrefBurger(const Point& dir, int manaColor, int expBurgerId = -1) const;
};

class GameSceneLoader : public LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(GameSceneLoader, loader);
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(GameScene);
};

#endif // __GAME_SCENE_H__
