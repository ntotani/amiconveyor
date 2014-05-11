#ifndef __gsweeper__ResultScene__
#define __gsweeper__ResultScene__

#include "cocos2d.h"
#include "cocosbuilder/CCLayerLoader.h"
#include "cocosbuilder/CCBSelectorResolver.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCBAnimationManager.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

class ResultScene : public Layer, public CCBMemberVariableAssigner, public CCBSelectorResolver
{
    Node* thumbLayer;
    Sprite* bg;
    Node* burger;
    LabelTTF* message;
    CCBAnimationManager *ccbAnimationManager;
    Sprite* thumb;
    string shout;
public:
    ResultScene();
    ~ResultScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(ResultScene);
    void onTouchEnded(Touch* touch, Event* event);
    static void setManas(vector<int> manas);

    virtual bool onAssignCCBMemberVariable(Ref* pTarget, const char* pMemberVariableName, Node* pNode);
    virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(Ref * pTarget, const char* pSelectorName);
    virtual Control::Handler onResolveCCBCCControlSelector(Ref * pTarget, const char* pSelectorName);
    void onOk(Ref* sender, Control::EventType type);
    void onFacebook(Ref* target);
    void onTwitter(Ref* target);
    void initAfter(CCBAnimationManager* a);
};

class ResultSceneLoader : public LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ResultSceneLoader, loader);
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(ResultScene);
};

#endif /* defined(__gsweeper__ResultScene__) */
