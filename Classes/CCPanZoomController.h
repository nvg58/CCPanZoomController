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

#ifndef __CCPANZOOMCONTROLLER_H__
#define __CCPANZOOMCONTROLLER_H__

#include "cocos2d.h"

class CCPanZoomController
{
private:
  //properties
  cocos2d::CCNode  *_node;
  
  //bounding rect
  cocos2d::CCPoint _tr;
  cocos2d::CCPoint _bl;
  
  //window rect
  cocos2d::CCPoint _winTr;
  cocos2d::CCPoint _winBl;
  
  bool    _centerOnPinch;
  bool    _zoomOnDoubleTap;
  float   _zoomRate;
  float   _zoomInLimit;
  float   _zoomOutLimit;
  float   _swipeVelocityMultiplier;
  float   _scrollDuration;
  float   _scrollRate;
  float   _scrollDamping;
  float   _pinchDamping;
  float   _pinchDistanceThreshold;
  float   _doubleTapZoomDuration;
  
	//touches
  cocos2d::CCPoint _firstTouch;
	float   _firstLength;
	float   _oldScale;
	
  //keep track of touches in order
  cocos2d::CCArray *_touches;
  
  //momentum
  cocos2d::CCPoint _momentum;

  /*!< The max bounds you want to scroll */
  CC_SYNTHESIZE(cocos2d::CCRect, boundingRect, BoundingRect);
  
  /*!< The boundary of your window, by default uses winSize of CCDirector */
  CC_SYNTHESIZE(cocos2d::CCRect, windowRect, WindowRect);
  
  /*!< Should zoom center on pinch pts, default is TRUE */
  CC_SYNTHESIZE(bool, centerOnPinch, CenterOnPinch)

  /*!< Should we zoom in/out on double-tap */
  CC_SYNTHESIZE(bool, zoomOnDoubleTap, ZoomOnDoubleTap);

  /*!< How much to zoom based on movement of pinch */
  CC_SYNTHESIZE(float, zoomRate, ZoomRate);
  
  /*!< The smallest zoom level */
  CC_SYNTHESIZE(float, zoomInLimit, ZoomInLimit);
  
  /*!< The hightest zoom level */
  CC_SYNTHESIZE(float, zoomOutLimit, ZoomOutLimit);
  
  /*!< Rate of the easing part of the scroll action after a swipe */
  CC_SYNTHESIZE(float, scrollRate, ScrollRate);
  
  /*!< When scrolling around, this will dampen the movement */
  CC_SYNTHESIZE(float, scrollDamping, ScrollDamping);

  /*!< Dampen the centering motion of the zoom */
  CC_SYNTHESIZE(float, zoomCenteringDamping, ZoomCenteringDamping);

  /*!< When zooming, this will dampen the zoom */
  CC_SYNTHESIZE(float, pinchDamping, PinchDamping);

  /*!< The distance moved before a pinch is recognized */
  CC_SYNTHESIZE(float, pinchDistanceThreshold, PinchDistanceThreshold);

  /*!< Get the optimal zoomOutLimit for the current state */
  CC_SYNTHESIZE_READONLY(float, optimalZoomOutLimit, OptimalZoomOutLimit);

  /*!< Duration of zoom after double-tap */
  CC_SYNTHESIZE(float, doubleTapZoomDuration, DoubleTapZoomDuration);

public:
/*! Create a new control with the node you want to scroll/zoom */
  static CCPanZoomController* controllerWithNode(cocos2d:: CCNode* node);

/*! Initialize a new control with the node you want to scroll/zoom */
  CCPanZoomController* initWithNode(cocos2d::CCNode* node);

/*! Scroll to position */
  void updatePosition(cocos2d::CCPoint pos);

/*! Center point in window view */
  void centerOnPoint(cocos2d::CCPoint pt);

/*! Center point in window view with damping (call this in your main loop to follow a character smoothly) */
  void centerOnPoint(cocos2d::CCPoint pt, float damping);

/*! Center point in window view with a given duration */
  void centerOnPoint(cocos2d::CCPoint pt, float duration, float rate);

/*! Zoom in on point with duration */
  void zoomInOnPoint(cocos2d::CCPoint pt, float duration);

/*! Zoom out on point with duration */
  void zoomOutOnPoint(cocos2d::CCPoint pt, float duration);

/*! Zoom to a scale on a point with a given duration */
  void zoomOnPoint(cocos2d::CCPoint pt, float duration, float scale);

/*! Enable touches, convenience method really */
  void enableWithTouchPriority(int priority, bool swallowsTouches);

/*! Disable touches */
  void disable();

};

#endif
