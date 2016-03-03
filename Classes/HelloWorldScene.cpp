#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

using namespace ui;


using namespace cocostudio::timeline;

USING_NS_CC;



// At top of file
HelloWorldHud *HelloWorld::_hud = NULL;

Touch *_touch = NULL;

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;





// implement some methods of HelloWorldHud
bool HelloWorldHud::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    label = LabelTTF::create("0", "fonts/Marker Felt.ttf", 18.0f, Size(50, 20), TextHAlignment::RIGHT);
    label->setColor(Color3B(0, 0, 0));
    int margin = 10;
    label->setPosition(visibleSize.width - (label->getDimensions().width / 2) - margin,
                       label->getDimensions().height / 2 + margin);
    this->addChild(label);
    
    
    // in HelloWorldHud's init method
    // define the button
    MenuItem *on = MenuItemImage::create("projectile-button-on.png", "projectile-button-on.png");
    MenuItem *off = MenuItemImage::create("projectile-button-off.png", "projectile-button-off.png");
    
    auto toggleItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(HelloWorldHud::projectileButtonTapped, this), off, on, NULL);
    auto toggleMenu = Menu::create(toggleItem, NULL);
    toggleMenu->setPosition(on->getContentSize().width * 2, on->getContentSize().height / 2);
    this->addChild(toggleMenu);
    
    return true;
}

void HelloWorldHud::numCollectedChanged(int numCollected)
{
    char showStr[20];
    sprintf(showStr, "%d", numCollected);
    label->setString(showStr);
}

// in HelloWorldHud
//callback for the button
//mode 0 = moving mode
//mode 1 = ninja star throwing mode
void HelloWorldHud::projectileButtonTapped(Object *pSender)
{
    if (_gameLayer->getMode() == 1) {
        _gameLayer->setMode(0);
    }
    else {
        _gameLayer->setMode(1);
    }
}


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);
    
    // Add to the HelloWorld::createScene() method, right before the return
    auto hud = HelloWorldHud::create();
    _hud = hud;
    _hud->setGameLayer(layer);
    
    scene->addChild(hud);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    
    
    // At top of init for HelloWorld layer
    SimpleAudioEngine::getInstance()->preloadEffect("pickup.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("hit.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("move.mp3");
    //SimpleAudioEngine::getInstance()->playBackgroundMusic("TileMap.mp3", true);
    
    _mode =0;
    
    /**  you can create scene with following comment code instead of using csb file.
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    **/
    
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    //auto rootNode = CSLoader::createNode("MainScene.csb");

    //addChild(rootNode);
    
    //_titleMap = TMXTiledMap::create("TileMap.tmx");
    //_background = _titleMap->layerNamed("Background");
    
    //addChild(_titleMap, -1);

    
    
    std::string file = "TileMap.tmx";
    auto str = String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(file.c_str()).c_str());
    _tileMap = TMXTiledMap::createWithXML(str->getCString(),"");
    _background = _tileMap->layerNamed("Background");
    addChild(_tileMap,-1);
    
    
    TMXObjectGroup *objects = _tileMap->getObjectGroup("Objects");
    CCASSERT(NULL != objects, "'Objects' object group not found");
    auto spawnPoint = objects->getObject("SpawnPoint");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
    int x = spawnPoint["x"].asInt();
    int y = spawnPoint["y"].asInt();
    _player = Sprite::create("Player.png");
    _player->setPosition(x, y);
    addChild(_player);
    setViewPointCenter(_player->getPosition());

    
    // Inside init method
    auto listener = EventListenerTouchOneByOne::create();
    //lambda expression: advanced feature in C++ 11
    //listener->onTouchBegan = [&](Touch *touch, Event *unused_event)->bool { return true; };
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //不可越初始化
    _meta = _tileMap->getLayer("Meta");
    _meta->setVisible(false);
    
    //可拾取初始化
    _foreground = _tileMap->getLayer("Foreground");

    //初始化敌人
    // in the init method - after creating the player
    // iterate through objects, finding all enemy spawn points
    // create an enemy for each one
    for (auto& eSpawnPoint : objects->getObjects()) {
        ValueMap& dict = eSpawnPoint.asValueMap();
        if (dict["Enemy"].asInt() == 1) {
            x = dict["x"].asInt();
            y = dict["y"].asInt();
            this->addEnemyAtPos(Point(x,y));
        }
    }
    
    
    return true;
}

//设置这个视图为玩家所在的位置
//http://www.cocos.com/doc/tutorial/show?id=1205
void HelloWorld::setViewPointCenter(Point position) {
    auto winSize = Director::getInstance()->getWinSize();
    
    int x = MAX(position.x, winSize.width / 2);
    int y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
    auto actualPosition = Point(x, y);
    
    auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
    auto viewPoint = centerOfView - actualPosition;
    this->setPosition(viewPoint);
    
    
}


//设置玩家位置
void HelloWorld::setPlayerPosition(Point position)
{
    //第二节新加，如果是不可过，直接返回
    Point tileCoord = this->tileCoordForPosition(position);
    int tileGid = _meta->getTileGIDAt(tileCoord);
    if (tileGid) {
        auto properties = _tileMap->getPropertiesForGID(tileGid).asValueMap();
        if (!properties.empty()) {
            
            auto collision = properties["Collidable"].asString();
            if ("True" == collision) {
                // In case for collidable tile
                SimpleAudioEngine::getInstance()->playEffect("hit.mp3");
                return;
            }
            
            //第三新加
            auto collectable = properties["Collectable"].asString();
            if ("True" == collectable) {
                _meta->removeTileAt(tileCoord);
                _foreground->removeTileAt(tileCoord);
                
                // Add inside setPlayerPosition, in the case where a tile is collectable
                this->_numCollected++;
                this->_hud->numCollectedChanged(_numCollected);
                
                // In case of collectable tile
                SimpleAudioEngine::getInstance()->playEffect("pickup.mp3");
            }
            //新加end3
            
        }
    }
    //新加end2
    
    // Right before setting player position
    SimpleAudioEngine::getInstance()->playEffect("move.mp3");
    
    _player->setPosition(position);
}



bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event)
{
    _touch = touch;
    //长按快速走路
    schedule(CC_SCHEDULE_SELECTOR(HelloWorld::movePos),0.3f);
    CCLOG("touch begin");
    return true;
}


void HelloWorld::movePos(float dt)
{
    
    auto touchLocation = _touch->getLocation();
    touchLocation = this->convertToNodeSpace(touchLocation);
    
    auto playerPos = _player->getPosition();
    auto diff = touchLocation - playerPos;
    if (abs(diff.x) > abs(diff.y)) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width;
        }
        else {
            playerPos.x -= _tileMap->getTileSize().width;
        }
    }
    else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height;
        }
        else {
            playerPos.y -= _tileMap->getTileSize().height;
        }
    }
    
    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getMapSize().width) &&
        playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getMapSize().height) &&
        playerPos.y >= 0 &&
        playerPos.x >= 0)
    {
        this->setPlayerPosition(playerPos);
    }
    
    this->setViewPointCenter(_player->getPosition());
    
}


//移动触屏
void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event)
{
    this->movePos(100.0f);
    //长按结束，停止
    unschedule(CC_SCHEDULE_SELECTOR(HelloWorld::movePos));
    CCLOG("unschedule");
}

//
//http://www.cocos.com/doc/tutorial/show?id=992
Point HelloWorld::tileCoordForPosition(Point position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return Point(x, y);
}


//增加敌人
//http://www.cocos.com/doc/tutorial/show?id=1206
void HelloWorld::addEnemyAtPos(Point pos)
{
    auto enemy = Sprite::create("enemy1.png");
    enemy->setPosition(pos);
    this->addChild(enemy);
    
    this->animateEnemy(enemy);
}

//starts another iteration of enemy movement.
//敌人的移动结束，继续下一次移动
void HelloWorld::enemyMoveFinished(Object *pSender)
{
    Sprite *enemy = (Sprite *)pSender;
    
    this->animateEnemy(enemy);
}

//敌人每次移动10像素
void HelloWorld::animateEnemy(Sprite *enemy)
{
    // speed of the enemy
    float actualDuration = 0.3f;
    
    // Create the actions
    //原方法
    //auto position = (_player->getPosition() - enemy->getPosition()).normalize()*10;
    
    //通用方法
    //Vec2 direction = ( _player->getPosition() - enemy->getPosition() );
    //direction.normalize();
    //auto position =  direction * 10;
    
    //新版本方法
    auto position =  ccpNormalize(_player->getPosition() - enemy->getPosition()) * 10;
    
    auto actionMove = MoveBy::create(actualDuration, position);
    auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyMoveFinished, this));
    enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
    
    //敌人的面向hero
    auto diff = ccpSub(_player->getPosition(), enemy->getPosition());
    float angleRadians = atanf((float)diff.y / (float)diff.x);
    float angleDegrees = CC_RADIANS_TO_DEGREES(angleRadians);
    float cocosAngle = -1 * angleDegrees;
    if (diff.x < 0) {
        cocosAngle += 180;
    }
    enemy->setRotation(cocosAngle);
}

