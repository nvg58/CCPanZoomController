/* Copyright (c) 2011 Robert Blackwood
 * Ported to CPP by Giap Nguyen on 28 Feb 2014
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "CCPanZoomController.h"
#include "cocos2d.h"

void handleDoubleTapAt(cocos2d::CCPoint pt)
{
	float mid = (_zoomInLimit + _zoomOutLimit) / 2;

	//closer to being zoomed out? then zoom in, else zoom out
	if (_node->scale < mid)
		this->zoomInOnPoint(pt, _doubleTapZoomDuration);
	else
		this->zoomOutOnPoint(pt, _doubleTapZoomDuration);
}

void zoomInOnPoint(cocos2d::CCPoint pt, float duration)
{
	this->zoomOnPoint(pt, duration, _zoomInLimit);
}

void zoomOutOnPoint(cocos2d::CCPoint pt, float duration)
{
	this->zoomOnPoint(pt, duration, _zoomOutLimit);
}

void zoomOnPoint(cocos2d::CCPoint pt, float duration, float scale)
{
	_node->runAcion(CCPanZoomControllerScale::create(duration, scale, this, pt));
}

void beginScroll(cocos2d::CCPoint* pos)
{
	//reset
	_momentum = cocos2d::CCPointZero;
	_firstTouch = pos;
}

void moveScroll(cocos2d::CCPoint* pos)
{
	//diff
	pos = cocos2d::ccpSub(pos, _firsTouch);

	//apply momentum
	_momentum->x += pos->x;
	_momentum->y += pos->y;

	//dampen value
	pos = cocos2d::ccpMult(pos, _scrollDamping * _node->scale);

	//debug
	//NSLog(@"Moving to: (%.2f, %.2f)", pos.x, pos.y);

	this->updatePosition::cocos2d::ccpAdd(_node->position, pos);
}

void endScroll(cocos2d::CCPoint* pos)
{
}

void beginZoom(cocos2d::CCPoint* pt, cocos2d::CCPoint* pt2)
{
	//initialize our zoom vars
	_firsLength = cocos2d::ccpDistance(pt, pt2);
	_oldScale = _node->scale;

	//get the mid point of pinch
	_firstTouch = [_node->convertToNodeSpace(CCDirector::shareDirector->convertToGL(ccpMidPoint(pt, pt2));
}

void moveZoom(cocos2d::CCPoint* pt, cocos2d::CCPoint* pt2)
{
	//What's the difference in length since we began
	float length = cocos2d::ccpDistance(pt, pt2);
	float diff = (length - _firstLength);

	//ignore small movements
	if (fabs(diff) < _pinchDistanceThreshold)
		return;

	//calculate new scale
	float factor = diff * _zoomRate;
	float scaleTo = (_oldScale + factor);
	float absScaleTO = fabs(scaleTo);
	float mult = absScaleTo / scaleTo;

	//paranoia
	if (!_oldScale)
		_oldScale = 0.001;

	//dampen
	float newScale = _oldScale*mult*pow(absScaleTo / _oldScale, _pinchDamping);

	if (isnormal(newScale))
	{
		//bound scale
		if (newScale > _zoomInLimit)
			newScale = _zoomInLimit;
		else if (newScale < _zoomOutLimit)
			newScale = _zoomOutLimit;

		//set the new scale
		_node->scale = newScale;

		//NSLog(@"Scale:%.2f", newScale);

		//center on midpoint of pinch
		if (_centerOnPinch)
			this->setCenterPoint(_firstTouch, _zoomCenteringDamping);
		else
			this->updatePosition(_node->position);
	}
	else
		cocos2d::CCLog("CCPanZoomController - Bad scale!");
}

void centerOnPoint(cocos2d::CCPoint pt)
{
	this->centerOnPoint(pt, 1.0f);
}

void centerOnPoint(cocos2d::CCPoint pt, float damping)
{
	//calc the difference between the window middle and the pt, apply the damping
	CCPoint* mid = _node->convertToNodeSpace(ccpMidPoint(_winTr, _winBl));
	CCPoint* diff = ccpMult(ccpSub(mid, pt), damping);
	CCPoint* oldPos = _node->position;
	CCPoint* newPos = ccpAdd(oldPos, diff);

	//NSLog(@"Centering on: (%.2f, %.2f)", newPos.x, newPos.y);
	this->updatePosition(newPos);
}

void centerOnPoint(cocos2d::CCPoint pt, float duration, float rate)
{
	//calc the difference between the window middle and the pt
	CCPoint* mid = _node->convertToNodeSpace(ccpMidpoint(_winTr, _winBl));
	CCPoint* diff = ccpSub(mid, pt);

	//get the final destination
	CCPoint* final = this->boundPos(ccpAdd(_node->position, diff));

	//move to n*ew position, with an ease
	cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(duration, final);
	cocos2d::CCEaseOut* ease = cocos2d::CCEaseOut::create(moveTo, rate);

	_node->runaction(ease);
}

void endZoom(cocos2d::CCPoint pt, cocos2d::CCPoint pt2)
{
	//[self moveZoom:pt otherPt:pt2];
}
}
=======
USING_NS_CC;

//Special scale action so view stays centered on a given point
class CCPanZoomControllerScale : public CCScaleTo
{
  CCPanZoomController *_controller;
  CCPoint _point;

  static CCPanZoomControllerScale* actionWithDuration(CCTime duration,
                                                      float scale,
                                                      CCPanZoomController* controller,
                                                      CCPoint pt)
  {
    CCPanZoomControllerScale *pRet = new CCPanZoomControllerScale();
    if (pRet && pRet->initWithDuration(duration, scale, controller, pt))
    {
      pRet->autorelease();
      return pRet;
    }
      CC_SAFE_DELETE(pRet);
    
    return NULL;
  }
  
  virtual bool initWithDuration(CCTime duration,
                                float scale,
                                CCPanZoomController* controller,
                                CCPoint pt)
  {
    if (CCScaleTo::initWithDuration(duration, scale))
    {
      controller->retain();
      _point = pt;
      return true;
    }
    return false;
  }
  
  void update(float t)
  {
//    [super update:t];
    
    //use damping, but make sure we get there
    if (t < 1.0f)
      _controller->centerOnPoint(_point, _controller->getZoomCenteringDamping());
    else
      _controller->centerOnPoint(_point);
  }
  
  void dealloc()
  {
    _controller->release();
//    [super dealloc];
  }
};

//Will return value between 0 and 1, think of it as a percentage of rotation
static inline float vectorsDeviation(CCPoint v1, CCPoint v2)
{
  return ccpLength(ccpSub(ccpNormalize(v1), ccpNormalize(v2)))/2.0f;
}

#define GET_PINCH_PTS(touches, pt1, pt2)\
CCTouch *touch1 = (CCTouch*)_touches->objectAtIndex(0);\
CCTouch *touch2 = (CCTouch*)_touches->objectAtIndex(1);\
CCPoint pt1 = touch1->getLocationInView();\
CCPoint pt2 = touch2->getLocationInView()


CCPanZoomController* CCPanZoomController::controllerWithNode(CCNode* node)
{
  CCPanZoomController *pRet = new CCPanZoomController();
  if (pRet && pRet->initWithNode(node))
  {
    pRet->autorelease();
    return pRet;
  }
  CC_SAFE_DELETE(pRet);
  return NULL;
}

bool CCPanZoomController::initWithNode(cocos2d::CCNode *node)
{
//	[super init];
	
	_touches = CCArray::create();
	
  //use the content size to determine the default scrollable area
	_node = node;
  _tr = ccp(0, 0);
  _bl = ccp(node->getContentSize().width, node->getContentSize().height);
  
  //use the screen size to determine the default window
  CCSize winSize = CCDirector::sharedDirector()->getWinSize();
  _winTr = CCPointMake(winSize.height, winSize.width);
	_winBl = CCPointZero;
  
  //default props
  _centerOnPinch = true;
  _zoomOnDoubleTap = true;
  _zoomRate = 1/500.0f;
  _zoomInLimit = 1.0f;
  _zoomOutLimit = 0.5f;
  _scrollRate = 9;
  _scrollDamping = .85;
  zoomCenteringDamping = .1;
  _pinchDamping = .9;
  _pinchDistanceThreshold = 3;
  _doubleTapZoomDuration = .2;
	
	return true;
}

//void dealloc()
//{
//	_touches->release();
//	[super dealloc];
//}

void CCPanZoomController::setBoundingRect(CCRect rect)
{
  _bl = rect.origin;
	_tr = ccpAdd(_bl, ccp(rect.size.width, rect.size.height));
}

CCRect CCPanZoomController::getBoundingRect()
{
  CCPoint size = ccpSub(_tr, _bl);
  return CCRectMake(_bl.x, _bl.y, size.x, size.y);
}

  void CCPanZoomController::setWindowRect(CCRect rect)
{
  _winBl = rect.origin;
	_winTr = ccpAdd(_winBl, ccp(rect.size.width, rect.size.height));
}

CCRect CCPanZoomController::getWindowRect()
{
  CCPoint size = ccpSub(_winTr, _winBl);
  return CCRectMake(_winBl.x, _winBl.y, size.x, size.y);
}

float CCPanZoomController::getOptimalZoomOutLimit()
{
  //default to 100%
  float xMaxZoom = 1;
  float yMaxZoom = 1;
  
  float width = (_tr.x - _bl.x);
  float height = (_tr.y - _bl.y);
  
  //don't divide by zero
  if (width)
    xMaxZoom = (_winTr.x - _winBl.x) / width;
    if (height)
      yMaxZoom = (_winTr.y - _winBl.y) / height;
      
      //give the best out of the 2 zooms
      return (xMaxZoom > yMaxZoom) ? xMaxZoom : yMaxZoom;
}

CCPoint CCPanZoomController::boundPos(cocos2d::CCPoint pos)
{
	float scale = _node->getScale();
  
  //Correct for anchor
  CCPoint anchor = ccp(_node->getContentSize().width*_node->getAnchorPoint().x,
                       _node->getContentSize().height*_node->getAnchorPoint().y);
  anchor = ccpMult(anchor, (1.0f - scale));
  
  //Calculate corners
  CCPoint topRight = ccpAdd(ccpSub(ccpMult(_tr, scale), _winTr), anchor);
  CCPoint bottomLeft = ccpSub(ccpAdd(ccpMult(_bl, scale), _winBl), anchor);
  
  //bound x
	if (pos.x > bottomLeft.x)
		pos.x = bottomLeft.x;
    else if (pos.x < -topRight.x)
      pos.x = -topRight.x;
      
      //bound y
      if (pos.y > bottomLeft.y)
        pos.y = bottomLeft.y;
        else if (pos.y < -topRight.y)
          pos.y = -topRight.y;
          
          return pos;
}

void CCPanZoomController::updatePosition(CCPoint pos)
{
  //user interface to boundPos basically
	pos = boundPos(pos);
  
  //debug
  //NSLog(@"Position set to: (%.2f, %.2f)", pos.x, pos.y);
  
  _node->setPosition(pos);
}

void CCPanZoomController::enableWithTouchPriority(int priority, bool swallowsTouches)
{
  CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, priority, swallowsTouches);

  CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(CCPanZoomController::updateTime), this, 0, false);
}


void CCPanZoomController::disable()
{
  CCDirector::sharedDirector()->getTouchDispatcher()->removeAllDelegates();
  CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(CCPanZoomController::updateTime), this);
  
  //Clean up any stray touches
  for (int i = 0; i < _touches->count(); ++i)
  {
    this->ccTouchCancelled((CCTouch*)_touches->objectAtIndex(i), NULL);
  }
}

void CCPanZoomController::updateTime(float dt)
{
  float degrade = dt*(_momentum.x*_scrollRate);
  _momentum.x -= degrade;
  
  degrade = dt*(_momentum.y*_scrollRate);
  _momentum.y -= degrade;
  
  if (!(_touches->count()))
  {
    // Apply momentum
    this->updatePosition(ccpAdd(_node->getPosition(), ccpMult(_momentum, _scrollDamping)));
  }
}

bool CCPanZoomController::ccTouchBegan(CCTouch* touch, CCEvent *event)
{
	_touches->addObject(touch);
	
	bool multitouch = _touches->count() > 1;
	
	if (multitouch)
	{
    //reset history so auto scroll doesn't happen
    _momentum = CCPointZero;
    
    //end the first touche's panning
    this->endScroll(_firstTouch);
    
    //get the 2 points
    GET_PINCH_PTS(_touches, pt1, pt2);
    
    //setup to zoom
    this->beginZoom(pt1, pt2);
	}
	else
  {
    //Start scrolling
    this->beginScroll(_node->convertToNodeSpace(touch->getLocationInView()));
  }
	
	return true;
}

void CCPanZoomController::ccTouchMoved(cocos2d::CCTouch *touch, CCEvent *event)
{
  //pinching case (zooming)
	bool multitouch = _touches->count() > 1;
	if (multitouch)
	{
    //get the 2 points
    GET_PINCH_PTS(_touches, pt1, pt2);
		
    //zoom it!
    this->moveZoom(pt1, pt2);
	}
	else
	{
    //pan around
    this->moveScroll(_node->convertToNodeSpace(touch->getLocationInView()));
	}
}

void CCPanZoomController::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
  //pinching case (zooming)
	bool multitouch = _touches->count() > 1;
	if (multitouch)
	{
    //get the 2 points, UITouch* touch1 and touch2 are declared here too
    GET_PINCH_PTS(_touches, pt1, pt2);
		
    //doesn't really do anything right now
		this->endZoom(pt1, pt2);
		
		//which touch remains?
		if (touch == touch2)
      this->beginScroll(_node->convertToNodeSpace(touch1->getLocationInView()));
		else
			this->beginScroll(_node->convertToNodeSpace(touch2->getLocationInView()));
	}
  
  //one finger case (panning)
	else
	{
    //end scroll
    CCPoint pt = _node->convertTouchToNodeSpace(touch);
    this->endScroll(pt);
    
    //handle double-tap zooming
//    if (_zoomOnDoubleTap && [touch tapCount] == 2)
//      [self handleDoubleTapAt:pt];
	}
	
  _touches->removeObject(touch);
}

void CCPanZoomController::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
  this->ccTouchCancelled(pTouch, pEvent);
}

- (void) handleDoubleTapAt:(CGPoint)pt
{
  float mid = (_zoomInLimit + _zoomOutLimit)/2;
  
  //closer to being zoomed out? then zoom in, else zoom out
  if (_node.scale < mid)
    [self zoomInOnPoint:pt duration:_doubleTapZoomDuration];
  else
    [self zoomOutOnPoint:pt duration:_doubleTapZoomDuration];
}

- (void) zoomInOnPoint:(CGPoint)pt duration:(float)duration
{
  [self zoomOnPoint:pt duration:duration scale:_zoomInLimit];
}

- (void) zoomOutOnPoint:(CGPoint)pt duration:(float)duration
{
  [self zoomOnPoint:pt duration:duration scale:_zoomOutLimit];
}

- (void) zoomOnPoint:(CGPoint)pt duration:(float)duration scale:(float)scale
{
  [_node runAction:[CCPanZoomControllerScale actionWithDuration:duration scale:scale controller:self point:pt]];
}

