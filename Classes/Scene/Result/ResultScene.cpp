#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"
#include "../../Common/GamePlatform.h"
#include "../../Common/LBSocial.h"
#include "../../Common/LBAnalytics.h"
#include "../Game/GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

static vector<int> manas;

ResultScene::ResultScene()
:popupShown(false)
{
}

ResultScene::~ResultScene()
{
}

Scene* ResultScene::createScene()
{
    auto scene = Scene::create();
    ResultScene *pRet = new ResultScene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
    scene->addChild(pRet);
    return scene;
}

bool ResultScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
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
    runAction(Sequence::create(DelayTime::create(3.5), CallFunc::create([voicePath]() {
        SimpleAudioEngine::getInstance()->playEffect(voicePath.c_str());
    }), NULL));

    auto vs = Director::getInstance()->getVisibleSize();
    auto center = Point(vs) / 2;
    auto bgId = voiceId == 7 ? 2 : (rand() % 7) + 1;
    if (voiceId != 7 && bgId == 2) { bgId = 3; }
    auto bg = Sprite::create(StringUtils::format("img/result_%d.png", bgId));
    bg->setPosition(center);
    auto burger = Sprite::create("img/game_bread_under.png");
    burger->setPosition(Point(100, 100));
    burger->setRotation(-10);
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
    thumbLayer = Node::create();
    thumbLayer->addChild(bg);
    thumbLayer->addChild(burger);
    addChild(thumbLayer);

    auto blkout = Sprite::create("img/game_popup_blkout.png");
    blkout->setPosition(center);
    blkout->setTag(0);
    auto popup = Sprite::create("img/game_popup_bg.png");
    popup->setPosition(center);
    popup->setTag(1);
    okBtn = Sprite::create("img/game_popup_button.png");
    okBtn->setPosition(center - Point(0, (popup->getContentSize().height - okBtn->getContentSize().height) / 2 - 10));
    auto ok = LabelTTF::create("OK", "", 24);
    ok->setPosition(Point(okBtn->getContentSize()) / 2);
    okBtn->addChild(ok);
    auto frame = Scale9Sprite::create("img/game_recipe.png");
    //frame->setContentSize(Size(180, 40));
    //frame->setPosition(btn->getPosition() + Point(-40, 50));
    frame->setPosition(center - Point(0, 60));
    shout = voice.second;
    auto message = LabelTTF::create(shout, "", 18);
    message->setColor(Color3B::BLACK);
    message->setPosition(Point(frame->getContentSize()) / 2);
    frame->addChild(message);
    twBtn = Sprite::create("img/icon_tw.png");
    twBtn->setPosition(frame->getPosition() + Point(frame->getContentSize().width / 2, -frame->getContentSize().height / 2 - twBtn->getContentSize().height / 2));
    fbBtn = Sprite::create("img/icon_fb.png");
    fbBtn->setPosition(twBtn->getPosition() - Point(twBtn->getContentSize().width + fbBtn->getContentSize().width, 0) / 2);

    popupLayer = Node::create();
    popupLayer->addChild(blkout);
    popupLayer->addChild(popup);
    //
    auto rt = RenderTexture::create(vs.width, vs.height);
    rt->begin();
    thumbLayer->visit();
    rt->end();
    rt->setScale(0.35f);
    rt->setPosition(Point(vs) / 2 + Point(0, 40));
    popupLayer->addChild(rt);
    //
    popupLayer->addChild(frame);
    popupLayer->addChild(twBtn);
    popupLayer->addChild(fbBtn);
    popupLayer->addChild(okBtn);
    for (auto n : popupLayer->getChildren()) {
        n->setVisible(false);
    }
    addChild(popupLayer);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = CC_CALLBACK_2(ResultScene::onTouchEnded, this);
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void ResultScene::onTouchEnded(Touch* touch, Event* event)
{
    if (!popupShown) {
        popupShown = true;
        auto blkout = popupLayer->getChildByTag(0);
        auto popup = popupLayer->getChildByTag(1);
        blkout->setVisible(true);
        popup->setScale(0.0f);
        popup->setVisible(true);
        popup->runAction(Sequence::create(ScaleTo::create(0.3f, 1.0f), CallFunc::create([&]() {
            for (auto n : popupLayer->getChildren()) {
                n->setVisible(true);
            }
        }), NULL));
        return;
    }
    auto p = touch->getLocation();
    if (okBtn->getBoundingBox().containsPoint(p)) {
        Director::getInstance()->replaceScene(GameScene::createScene());
    } else if (twBtn->getBoundingBox().containsPoint(p)) {
        AppDelegate::screenShot(thumbLayer, "screenshot.jpg", [&](std::string filePath) {
            LBSocial::tweet((shout + " http://goo.gl/pd1z0L").c_str(), filePath.c_str());
        });
    } else if (fbBtn->getBoundingBox().containsPoint(p)) {
        AppDelegate::screenShot(thumbLayer, "screenshot.jpg", [&](std::string filePath) {
            LBSocial::facebook((shout + " http://goo.gl/pd1z0L").c_str(), filePath.c_str());
        });
    }
}

void ResultScene::setManas(vector<int> m)
{
    manas = m;
}
