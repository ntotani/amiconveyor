#ifndef __gsweeper__ResultScene__
#define __gsweeper__ResultScene__

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class ResultScene : public Layer
{
    Node* thumbLayer;
    Node* popupLayer;
    Sprite* okBtn;
    Sprite* twBtn;
    Sprite* fbBtn;
    bool popupShown;
    string shout;
public:
    ResultScene();
    ~ResultScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(ResultScene);
    void onTouchEnded(Touch* touch, Event* event);
    static void setManas(vector<int> manas);
};

#endif /* defined(__gsweeper__ResultScene__) */
