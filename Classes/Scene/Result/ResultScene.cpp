#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"
#include "../../Common/GamePlatform.h"
#include "../../Common/LBSocial.h"
#include "../../Common/LBAnalytics.h"
#include "../Game/GameScene.h"

USING_NS_CC;

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
    auto vs = Director::getInstance()->getVisibleSize();
    auto center = Point(vs) / 2;
    auto bg = Sprite::create("img/result_4.png");
    bg->setPosition(center);
    // burger tower
    thumbLayer = Node::create();
    thumbLayer->addChild(bg);
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
    frame->setPosition(center - Point(0, 50));
    auto message = LabelTTF::create("さばー！！", "", 18);
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
    rt->setScale(0.4f);
    rt->setPosition(Point(vs) / 2 + Point(0, 30));
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
    }
    auto p = touch->getLocation();
    if (okBtn->getBoundingBox().containsPoint(p)) {
        Director::getInstance()->replaceScene(GameScene::createScene());
    }
}
