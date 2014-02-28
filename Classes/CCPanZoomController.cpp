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
USING_NS_CC;

//Special scale action so view stays centered on a given point
class CCPanZoomControllerScale : CCScaleTo
{
  CCPanZoomController *_controller;
  CCPoint _point;

  static CCPanZoomControllerScale* actionWithDuration(CCTime duration, float scale, CCPanZoomController* controller, CCPoint pt);
  virtual bool initWithDuration(CCTime duration, float scale, CCPanZoomController* controller, CCPoint pt);
};

//Will return value between 0 and 1, think of it as a percentage of rotation
static inline float vectorsDeviation(CGPoint v1, CGPoint v2)
{
  return ccpLength(ccpSub(ccpNormalize(v1), ccpNormalize(v2)))/2.0f;
}

#define GET_PINCH_PTS(touches, pt1, pt2)\
UITouch *touch1 = [touches objectAtIndex:0];\
UITouch *touch2 = [touches objectAtIndex:1];\
CGPoint pt1 = [touch1 locationInView:[touch view]];\
CGPoint pt2 = [touch2 locationInView:[touch view]]

@implementation CCPanZoomControllerScale

+(id) actionWithDuration:(ccTime)duration
scale:(float)s
controller:(CCPanZoomController*)controller
point:(CGPoint)pt
{
  return [[[self alloc] initWithDuration:duration scale:s controller:controller point:pt] autorelease];
}

-(id) initWithDuration:(ccTime)duration
scale:(float)s
controller:(CCPanZoomController*)controller
point:(CGPoint)pt
{
  [super initWithDuration:duration scale:s];
  _controller = [controller retain];
  _point = pt;
  return self;
}

-(void) update: (ccTime) t
{
  [super update:t];
  
  //use damping, but make sure we get there
  if (t < 1.0f)
    [_controller centerOnPoint:_point damping:_controller.zoomCenteringDamping];
  else
    [_controller centerOnPoint:_point];
}

-(void) dealloc
{
  [_controller release];
  [super dealloc];
}

@end

@implementation CCPanZoomController

@synthesize centerOnPinch = _centerOnPinch;
@synthesize zoomOnDoubleTap = _zoomOnDoubleTap;
@synthesize zoomRate = _zoomRate;
@synthesize zoomInLimit = _zoomInLimit;
@synthesize zoomOutLimit = _zoomOutLimit;
@synthesize scrollRate = _scrollRate;
@synthesize scrollDamping = _scrollDamping;
@synthesize zoomCenteringDamping = _zoomCenteringDamping;
@synthesize pinchDamping = _pinchDamping;
@synthesize pinchDistanceThreshold = _pinchDistanceThreshold;
@synthesize doubleTapZoomDuration = _doubleTapZoomDuration;

+ (id) controllerWithNode:(CCNode*)node
{
	return [[[self alloc] initWithNode:node] autorelease];
}

- (id) initWithNode:(CCNode*)node;
{
	[super init];
	
	_touches = [[NSMutableArray alloc] init];
	
  //use the content size to determine the default scrollable area
	_node = node;
  _tr = ccp(0, 0);
  _bl = ccp(node.contentSize.width, node.contentSize.height);
  
  //use the screen size to determine the default window
  CGSize winSize = [[CCDirector sharedDirector] winSize];
	_winTr.x = winSize.width;
	_winTr.y = winSize.height;
  _winBl.x = 0;
  _winBl.y = 0;
  
  //default props
  _centerOnPinch = YES;
  _zoomOnDoubleTap = YES;
  _zoomRate = 1/500.0f;
  _zoomInLimit = 1.0f;
  _zoomOutLimit = 0.5f;
  _scrollRate = 9;
  _scrollDamping = .85;
  _zoomCenteringDamping = .1;
  _pinchDamping = .9;
  _pinchDistanceThreshold = 3;
  _doubleTapZoomDuration = .2;
	
	return self;
}

- (void) dealloc
{
	[_touches release];
	[super dealloc];
}

- (void) setBoundingRect:(CGRect)rect
{
  _bl = rect.origin;
	_tr = ccpAdd(_bl, ccp(rect.size.width, rect.size.height));
}

-(CGRect) boundingRect
{
  CGPoint size = ccpSub(_tr, _bl);
  return CGRectMake(_bl.x, _bl.y, size.x, size.y);
}

- (void) setWindowRect:(CGRect)rect
{
  _winBl = rect.origin;
	_winTr = ccpAdd(_winBl, ccp(rect.size.width, rect.size.height));
}

-(CGRect) windowRect
{
  CGPoint size = ccpSub(_winTr, _winBl);
  return CGRectMake(_winBl.x, _winBl.y, size.x, size.y);
}

-(float) optimalZoomOutLimit
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

- (CGPoint) boundPos:(CGPoint)pos
{
	float scale = _node.scale;
  
  //Correct for anchor
  CGPoint anchor = ccp(_node.contentSize.width*_node.anchorPoint.x,
                       _node.contentSize.height*_node.anchorPoint.y);
  anchor = ccpMult(anchor, (1.0f - scale));
  
  //Calculate corners
  CGPoint topRight = ccpAdd(ccpSub(ccpMult(_tr, scale), _winTr), anchor);
  CGPoint bottomLeft = ccpSub(ccpAdd(ccpMult(_bl, scale), _winBl), anchor);
  
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

- (void) updatePosition:(CGPoint)pos
{
  //user interface to boundPos basically
	pos = [self boundPos:pos];
  
  //debug
  //NSLog(@"Position set to: (%.2f, %.2f)", pos.x, pos.y);
  
	[_node setPosition:pos];
}

- (void) enableWithTouchPriority:(int)priority swallowsTouches:(BOOL)swallowsTouches
{
#if (COCOS2D_VERSION < 0x00020000)
	[[CCTouchDispatcher sharedDispatcher] addTargetedDelegate:self
                                                   priority:priority
                                            swallowsTouches:swallowsTouches];
	[[CCScheduler sharedScheduler] scheduleSelector:@selector(updateTime:) forTarget:self interval:0 paused:NO];
#else
  [[[CCDirector sharedDirector] touchDispatcher] addTargetedDelegate:self
                                                            priority:priority
                                                     swallowsTouches:swallowsTouches];
  [[[CCDirector sharedDirector] scheduler] scheduleSelector:@selector(updateTime:) forTarget:self interval:0 paused:NO];
#endif
}
