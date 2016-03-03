#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"



USING_NS_CC;

class HelloWorld;


// Before HelloWorld class declaration
class HelloWorldHud : public cocos2d::Layer
{
public:
    void numCollectedChanged(int numCollected);
    void projectileButtonTapped(Object *pSender);
    virtual bool init();
    CREATE_FUNC(HelloWorldHud);
    
    cocos2d::LabelTTF *label;
//private:
//    HelloWorld *_gameLayer;
    CC_SYNTHESIZE(HelloWorld *, _gameLayer, GameLayer);
};


class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    void setViewPointCenter(cocos2d::Point position);
    void setPlayerPosition(cocos2d::Point position);
    void onTouchEnded(Touch *touch, Event *unused_event);
    bool onTouchBegan(Touch *touch, Event *unused_event);
    void movePos(float);
    Point tileCoordForPosition(cocos2d::Point position);
    void addEnemyAtPos(Point pos);
    void animateEnemy(Sprite *enemy);
    void enemyMoveFinished(Object *pSender);
    
    // Inside HelloWorld class declaration
    int _numCollected;
    static HelloWorldHud *_hud;
    
    
    CC_SYNTHESIZE(int, _mode, Mode);
    
private:
    cocos2d::TMXTiledMap *_tileMap;//地图
    cocos2d::TMXLayer *_background;//背景
    cocos2d::Sprite *_player;//玩家
    cocos2d::TMXLayer *_meta;//不可过
    cocos2d::TMXLayer *_foreground;//可以拾取

    
};




#endif // __HELLOWORLD_SCENE_H__
