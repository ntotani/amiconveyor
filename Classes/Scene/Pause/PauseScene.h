#ifndef __leadblow__PauseScene__
#define __leadblow__PauseScene__

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class PauseScene : public Layer
{
public:
    PauseScene();
    ~PauseScene();
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(PauseScene);
};

#endif /* defined(__leadblow__PauseScene__) */
