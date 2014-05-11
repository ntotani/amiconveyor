#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"
#include "../../Common/GamePlatform.h"
#include "../../Common/LBSocial.h"
#include "../../Common/LBAnalytics.h"
#include "../Game/GameScene.h"
#include "SimpleAudioEngine.h"
#include "cocosbuilder/CCNodeLoaderLibrary.h"

USING_NS_CC;
using namespace CocosDenshion;

static vector<int> manas;

ResultScene::ResultScene()
:thumbLayer(nullptr)
,bg(nullptr)
,burger(nullptr)
,message(nullptr)
,ccbAnimationManager(nullptr)
,thumb(nullptr)
{
}

ResultScene::~ResultScene()
{
}

Scene* ResultScene::createScene()
{
    auto scene = Scene::create();
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("ResultScene", ResultSceneLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    ccbReader->autorelease();
    auto layer = ccbReader->readNodeGraphFromFile("ccbi/ResultScene.ccbi");
    static_cast<ResultScene*>(layer)->initAfter(ccbReader->getAnimationManager());
    scene->addChild(layer);
    return scene;
}

bool ResultScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = CC_CALLBACK_2(ResultScene::onTouchEnded, this);
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

bool ResultScene::onAssignCCBMemberVariable(Ref* pTarget, const char* pMemberVariableName, Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "thumbLayer", Node*, thumbLayer);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "bg", Sprite*, bg);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "burger", Node*, burger);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "message", LabelTTF*, message);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "thumb", Sprite*, thumb);
    return true;
}

SEL_MenuHandler ResultScene::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onFacebook", ResultScene::onFacebook);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onTwitter", ResultScene::onTwitter);
    return NULL;
}

Control::Handler ResultScene::onResolveCCBCCControlSelector(Ref * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "onOk", ResultScene::onOk);
    return NULL;
}

void ResultScene::initAfter(CCBAnimationManager* a)
{
    ccbAnimationManager = a;
    vector<pair<string, string>> voiceLib = {
        pair<string, string>("fueen", "ふえーん！"),
        pair<string, string>("hiddoi", "ひっどーい！"),
        pair<string, string>("hiddooi", "ひっどーい！"),
        pair<string, string>("kyaa", "きゃー！"),
        pair<string, string>("shinjirannai", "しんじらんな~い！"),
        pair<string, string>("usonaki", "あーんあーんあーん！"),
        pair<string, string>("uwaaan", "うわーん！"),
        pair<string, string>("puhaa", "ぷはー")
    };
    auto voiceId = rand() % voiceLib.size();
    auto voice = voiceLib[voiceId];
    auto voicePath = "sound/voice_" + voice.first + ".wav";
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic(voicePath.c_str());
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/all_right!.mp3");
    bg->runAction(Sequence::create(DelayTime::create(3.5), CallFunc::create([voicePath]() {
        SimpleAudioEngine::getInstance()->playEffect(voicePath.c_str());
    }), NULL));
    
    auto vs = Director::getInstance()->getVisibleSize();
    auto bgId = voiceId == 7 ? 2 : (rand() % 7) + 1;
    if (voiceId != 7 && bgId == 2) { bgId = 3; }
    bg->setTexture(StringUtils::format("img/result_%d.png", bgId));
    burger->addChild(Sprite::create("img/game_bread_under.png"));
    for (int i = 0; i < manas.size(); i++) {
        auto img = StringUtils::format("img/game_mana_%d.png", manas[i]);
        auto mana = Sprite::create(img);
        mana->setPosition(Point(burger->getContentSize()) / 2 + Point(-5, 7 * (i + 1)));
        burger->addChild(mana);
        if (i == manas.size() - 1) {
            auto bread = Sprite::create("img/game_bread.png");
            bread->setPosition(Point(burger->getContentSize()) / 2 + Point(-5, 7 * (i + 2)));
            burger->addChild(bread);
        }
    }
    shout = voice.second;
    message->setString(shout);
    auto rt = RenderTexture::create(vs.width, vs.height);
    rt->begin();
    thumbLayer->visit();
    rt->end();
    rt->setPosition(Point(thumb->getContentSize()) / 2);
    thumb->addChild(rt);
}

void ResultScene::onTouchEnded(Touch* touch, Event* event)
{
    auto now = ccbAnimationManager->getRunningSequenceName();
    if (now && strcmp(now, "popup") != 0) {
        ccbAnimationManager->runAnimationsForSequenceNamed("popup");
    }
}

void ResultScene::setManas(vector<int> m)
{
    manas = m;
}

void ResultScene::onOk(Ref* sender, Control::EventType type)
{
    Director::getInstance()->replaceScene(GameScene::createScene());
}

void ResultScene::onFacebook(Ref* target)
{
    AppDelegate::screenShot(thumbLayer, "screenshot.jpg", [&](std::string filePath) {
        LBSocial::facebook((shout + " http://goo.gl/pd1z0L").c_str(), filePath.c_str());
    });
}

void ResultScene::onTwitter(Ref* target)
{
    AppDelegate::screenShot(thumbLayer, "screenshot.jpg", [&](std::string filePath) {
        LBSocial::tweet((shout + " http://goo.gl/pd1z0L").c_str(), filePath.c_str());
    });
}
