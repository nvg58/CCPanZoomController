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
{

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