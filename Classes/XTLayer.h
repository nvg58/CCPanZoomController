/****************************************************************************
 Copyright (c) 2013      Edwin van Poeijer
 
 http://www.vanpoeijer.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __XTLayer__
#define __XTLayer__

#include "cocos2d.h"

using namespace cocos2d;

class XTLayer : public cocos2d::CCLayer
{
public:
    enum XTTouchDirection { LEFT,RIGHT,UP,DOWN };
    enum XTGesture { TAP,LONGTAP,SWIPE };

    XTLayer();
    ~XTLayer();
    
private:
    CCArray* xtAllTouchextromSet(CCSet *_touches);
    XTTouchDirection _xtTouchDirection;

    // Default settings
    float _xtSwipeThreshold;
    float _xtSwipeTime;
    float _xtTapThreshold;
    float _xtTapTime;
    float _xtLongTapTreshold;
    float _xtLongTapTime;
    float _xtDoubleTapTime;
    
    // Touch positions
    CCPoint xtGestureStartPoint;
    CCPoint xtActualPoint;
    CCPoint xtPreviousPoint;
    CCPoint xtGestureEndPoint;
    bool _touchHasMoved;
    long _xtTouchStart;
    int  _xtNumberOfTaps;
    long millisecondNow();
    void tapHandler(CCObject* caller);
    
public:
   
    // Gesture settings
    float xtSwipeThreshold() { return _xtSwipeThreshold;};
    void xtSwipeThreshold(float value) { _xtSwipeThreshold -= value;};
    float xtSwipeTime() { return _xtSwipeTime;};
    void xtSwipeTime(float value) { _xtSwipeTime = value;};
    float xtTapTreshold() { return _xtTapThreshold;};
    void xtTapTreshold(float value) { _xtTapThreshold = value;};    
    float xtTapTime() { return _xtTapTime;};
    void xtTapTime(float value) { _xtTapTime = value;};    
    float xtLongTapTreshold() { return _xtLongTapTreshold;};
    void  xtLongTapTreshold(float value) { _xtLongTapTreshold = value;};
    float xtLongTapTime() { return _xtLongTapTime;};
    void xtLongTapTime(float value) { _xtLongTapTime = value;};
    float xtDoubleTapTime() { return _xtDoubleTapTime;};
    void xtDoubleTapTime(float value) { _xtDoubleTapTime = value;};
    
    
    // Override of touches
    virtual void ccTouchesBegan(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* _touches, cocos2d::CCEvent* event);
    
    // Callbacks
    virtual void xtTouchesBegan(cocos2d::CCSet* _touches, cocos2d::CCEvent* event){}
    virtual void xtTouchesMoved(cocos2d::CCSet* _touches, cocos2d::CCEvent* event){}
    virtual void xtTouchesEnded(cocos2d::CCSet* _touches, cocos2d::CCEvent* event){}

    // Easy touch callbacks
    virtual void xtTouchesBegan(CCPoint position){}
    virtual void xtTouchesMoved(CCPoint position){}
    virtual void xtTouchesEnded(CCPoint position){}
    
    // Gesture Callbacks
    virtual void xtTapGesture(CCPoint position) {}
    virtual void xtDoubleTapGesture(CCPoint position) {}
    virtual void xtLongTapGesture(CCPoint position) {}
    virtual void xtSwipeGesture(XTTouchDirection direction, float distance, float speed) {}
};

#endif /* defined(__XTLayer__) */
