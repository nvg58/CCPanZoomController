#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
  
  // our bounding rect
  CCRect boundingRect = CCRectMake(0, 0, 964, 700);
  
  // background
  CCSprite *background = CCSprite::create("background.png");
  background->setAnchorPoint(CCPointZero);
  this->addChild(background);
  
  // create and initialize a Label
  CCLabelTTF *label = CCLabelTTF::create("Hello World, try panning and zooming using drag and pinch",
                                         "Marker Felt",
                                         32);
  
	
  // position the label on the center of the bounds
  label->setPosition(ccp(boundingRect.size.width/2 , boundingRect.size.height/2));
  label->setColor(ccWHITE);
  
  // add the label as a child to this Layer
  this->addChild(label);
  
  // the pan/zoom controller
  _controller = CCPanZoomController::controllerWithNode(this);
//  _controller->retain();
  _controller->setBoundingRect(boundingRect);
  _controller->setZoomOutLimit(_controller->getOptimalZoomOutLimit());
  _controller->setZoomInLimit(2.0f);
  
  _controller->enableWithTouchPriority(0, true);
  
  return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}
