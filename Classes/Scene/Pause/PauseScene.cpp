#include "PauseScene.h"

PauseScene::PauseScene()
{
}

PauseScene::~PauseScene()
{
}

Scene* PauseScene::createScene()
{
    auto scene = Scene::create();
    auto layer = PauseScene::create();
    scene->addChild(layer);
    return scene;
}

bool PauseScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    auto center = Point(Director::getInstance()->getVisibleSize()) / 2;
    auto blkout = Sprite::create("img/game_popup_blkout.png");
    blkout->setPosition(center);
    auto popup = Sprite::create("img/game_popup_bg.png");
    popup->setPosition(center);
    addChild(blkout);
    addChild(popup);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = [](Touch* touch, Event* event) {
        Director::getInstance()->popScene();
    };
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}
