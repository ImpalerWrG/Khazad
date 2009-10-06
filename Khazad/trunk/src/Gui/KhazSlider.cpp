/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naessén and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naessén a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// derived from guichan slider by peterix

/*
 * For comments regarding functions please see the header file.
 */

//TODO: error check on image load

#include "KhazSlider.hpp"

#include "guichan/graphics.hpp"
#include "guichan/image.hpp"
#include "guichan/key.hpp"
#include "guichan/mouseinput.hpp"
#include <stdint.h>
#include <algorithm>
using namespace std;

namespace gcn
{

    
KhazSlider::KhazSlider
(
const std::string& tophandle,
const std::string& bottomhandle,
const std::string& topbutton,
const std::string& bottombutton,
const std::string& topbutton_hover,
const std::string& bottombutton_hover,
const std::string& topbutton_click,
const std::string& bottombutton_click,
const std::string& background,
int16_t maxZ = 1
)
{
    mImageTop = Image::load(tophandle);
    mImageBottom = Image::load(bottomhandle);
    mImageTopB = Image::load(topbutton);
    mImageBottomB = Image::load(bottombutton);
    mImageTopBH = Image::load(topbutton_hover);
    mImageBottomBH = Image::load(bottombutton_hover);
    mImageTopBC = Image::load(topbutton_click);
    mImageBottomBC = Image::load(bottombutton_click);
    mImageBackground = Image::load(background);
    mMaxZ = maxZ;
    
    mHighlighted = NO_HIGHLIGHT;
    mDragged = NO_DRAG;
    
    setFocusable (false);
    setFrameSize (0);
    setBottomSlice (0);
    setTopSlice (mMaxZ);
    setMarkerLength (32);

    addMouseListener (this);
    addKeyListener (this);
}

KhazSlider::~KhazSlider()
{
    delete(mImageTop);
    delete(mImageBottom);
    delete(mImageTopB);
    delete(mImageBottomB);
    delete(mImageTopBC);
    delete(mImageBottomBC);
    delete(mImageTopBH);
    delete(mImageBottomBH);
    delete(mImageBackground);
}

void KhazSlider::setMaxZ (int16_t maxZ)
{
    mMaxZ = maxZ;
}

int16_t KhazSlider::getMaxZ() const
{
    return mMaxZ;
}

// utility function for color gradients
gcn::Color MixColors(gcn::Color first,gcn::Color second, int steps, int index)
{
    // cut off extremes
    steps +=2;
    double gradient = ((double)(index + 1)) / ((double)steps);
    double rs, rt, gs, gt, bs, bt, r, g ,b;
    rs = first.r;
    gs = first.g;
    bs = first.b;
    
    rt = second.r;
    gt = second.g;
    bt = second.b;
    
    r = rs + (rt - rs) * gradient;
    g = gs + (gt - gs) * gradient;
    b = bs + (bt - bs) * gradient;
    
    return gcn::Color(r,g,b);
}


void KhazSlider::draw (gcn::Graphics* graphics)
{
    // draw background
    /*
    Color shadowColor = getBaseColor() - 0x101010;
    int alpha = getBaseColor().a;
    shadowColor.a = alpha;

    graphics->setColor (shadowColor);
    graphics->fillRectangle (gcn::Rectangle (0, 0, getWidth(), getHeight()));
    graphics->setColor (shadowColor - 0x202020);
    graphics->fillRectangle (gcn::Rectangle (12, 12, 8, getHeight() - 24));
    graphics->setColor (shadowColor - 0x303030);
    graphics->fillRectangle (gcn::Rectangle (13, 13, 6, getHeight() - 26));
    graphics->setColor (shadowColor - 0x606060);
    graphics->fillRectangle (gcn::Rectangle (14, 14, 4, getHeight() - 28));
    graphics->setColor (shadowColor - 0x808080);
    graphics->fillRectangle (gcn::Rectangle (15, 15, 2, getHeight() - 30));
*/
    int step = mImageBackground->getHeight();
    for(int i = 0; i <= getHeight(); i+=step)
    {
        graphics->drawImage(mImageBackground, 0, i);
    }
    // draw 'chain'
    gcn::Color topColor = 0xE3E3E3;
    gcn::Color bottomColor = 0xEB8D00;
    int top = getTopMarkerPosition();
    int bottom = getBottomMarkerPosition();
    int height = bottom - top;
    int numslices = getTopSlice() - getBottomSlice();
    
    // don't touch this
    float softstep = (float)(getHeight() - 2 * getMarkerLength() - 2 * 16) /(float) getMaxZ();
    for(int i = 0;i < numslices;i++)
    {
        gcn::Color faceColor = MixColors(topColor,bottomColor, numslices, i);
        Color highlightColor, shadowColor;
        int alpha = getBaseColor().a;
        highlightColor = faceColor + 0x303030;
        highlightColor.a = alpha;
        shadowColor = faceColor - 0x303030;
        shadowColor.a = alpha;
        
        int current = getMarkerLength() + 16 + softstep * (i + (getMaxZ() - getTopSlice()));
        int next = getMarkerLength() + 16 + softstep * (1+ i + (getMaxZ() - getTopSlice()));
        
        graphics->setColor (faceColor);
        graphics->fillRectangle (gcn::Rectangle (12,current , 8, next-current));
        graphics->setColor (highlightColor);
        graphics->drawLine (12, current, 12, next - 1);
        graphics->drawLine (12, current, 20, current);
        graphics->setColor (shadowColor);
        graphics->drawLine (13, next - 1 , 20, next -1);
        graphics->drawLine (20, current, 20, next - 1);
    }
    if(mHighlighted == CLICK_TOP)
        graphics->drawImage(mImageTopBC, 0, 0);
    else if(mHighlighted == HOVER_TOP)
        graphics->drawImage(mImageTopBH, 0, 0);
    else
        graphics->drawImage(mImageTopB, 0, 0);
    graphics->drawImage(mImageTop, 0, getTopMarkerPosition() - getMarkerLength());
    
    graphics->drawImage(mImageBottom, 0, getBottomMarkerPosition());
    if(mHighlighted == CLICK_BOTTOM)
        graphics->drawImage(mImageBottomBC, 0, getHeight() - mImageBottomB->getHeight());
    else if(mHighlighted == HOVER_BOTTOM)
        graphics->drawImage(mImageBottomBH, 0, getHeight() - mImageBottomB->getHeight());
    else
        graphics->drawImage(mImageBottomB, 0, getHeight() - mImageBottomB->getHeight());
}


void KhazSlider::mousePressed (MouseEvent& mouseEvent)
{
    // TODO: determine which action to take
    
    if (mouseEvent.getButton() == gcn::MouseEvent::LEFT
            && mouseEvent.getX() >= 0
            && mouseEvent.getX() <= getWidth()
            && mouseEvent.getY() >= 0
            && mouseEvent.getY() <= getHeight())
    {
        int y = mouseEvent.getY();
        int topmarker = valueToMarkerPosition(getTopSlice());
        int bottommarker = valueToMarkerPosition(getBottomSlice());
        
        // TODO: clicking inside handles is different form outside handles
        
        // on top button
        if(y < 16)
        {
            SlideUp();
            mHighlighted = CLICK_TOP;
        }
        // on top of top handle
        else if(y < topmarker - getMarkerLength())
        {
            int zy = markerPositionToValue (y + getMarkerLength()/2);
            setTopSlice (zy);
            mDragged = DRAG_TOP;
            mDragOffset = getMarkerLength()/2;
        }
        // on top handle
        else if(y < topmarker)
        {
            mDragged = DRAG_TOP;
            mDragOffset = topmarker - y;
        }
        // center
        else if ( y < bottommarker)
        {
            mDragged = DRAG_CENTER;
            mDragOffset = topmarker - y;
        }
        // on bottom handle
        else if ( y < bottommarker + getMarkerLength())
        {
            mDragged = DRAG_BOTTOM;
            mDragOffset = bottommarker - y;
        }
        // under bottom handle
        else if (y < getHeight() - 16)
        {
            int zy = markerPositionToValue (y - getMarkerLength()/2);
            setBottomSlice (zy);
            mDragged = DRAG_BOTTOM;
            mDragOffset = - getMarkerLength()/2;
        }
        // bottom button
        else
        {
            SlideDown();
            mHighlighted = CLICK_BOTTOM;
        }
        distributeActionEvent();
    }
}

void KhazSlider::mouseReleased(MouseEvent& mouseEvent)
{
    if (mouseEvent.getButton() == gcn::MouseEvent::LEFT)
    {
        mDragged = NO_DRAG;
        //TODO: determine based on mouse position if there should be highlight
        updateHighlight(mouseEvent);
    }
}
void KhazSlider::updateHighlight(MouseEvent& mouseEvent)
{
    if(mouseEvent.getX() >= 0
        && mouseEvent.getX() <= getWidth()
        && mouseEvent.getY() >= 0
        && mouseEvent.getY() <= getHeight())
    {
        int y = mouseEvent.getY();
        if(y < 16)
        {
            mHighlighted = HOVER_TOP;
        }
        else if(y >= getHeight() - 16)
        {
            mHighlighted = HOVER_BOTTOM;
        }
        else
        {
            mHighlighted = NO_HIGHLIGHT;
        }
    }
}
void KhazSlider::mouseMoved(MouseEvent& mouseEvent)
{
    updateHighlight(mouseEvent);
}

void KhazSlider::mouseExited(MouseEvent& mouseEvent)
{
    if(mHighlighted == HOVER_BOTTOM || mHighlighted == HOVER_TOP)
        mHighlighted = NO_HIGHLIGHT;
}

void KhazSlider::mouseEntered(MouseEvent& mouseEvent)
{
    mouseMoved(mouseEvent);
}

void KhazSlider::mouseDragged (MouseEvent& mouseEvent)
{
    // TODO: use drag offset so things don't jump
    switch (mDragged)
    {
        case DRAG_TOP:
            setTopSlice (markerPositionToValue ( mouseEvent.getY() + mDragOffset));
            break;
        case DRAG_BOTTOM:
            setBottomSlice (markerPositionToValue ( mouseEvent.getY() + mDragOffset));
            break;
        case DRAG_CENTER:
            // we compute with slices to avoid runaway values
            int numslices = getTopSlice() - getBottomSlice();
            int topmarker = mouseEvent.getY() +mDragOffset;
            int newtop = markerPositionToValue (topmarker);
            int newbottom = newtop -numslices;
            if(newbottom < 0)
            {
                newbottom = 0;
                newtop = numslices;
            }
            setTopSlice (newtop);
            setBottomSlice (newbottom);
            break;
    }
    distributeActionEvent();

    mouseEvent.consume();
    
}

void KhazSlider::setTopSlice (int16_t value)
{
    mTopSlice = value;
    if (value > getMaxZ())
    {
        mTopSlice = getMaxZ();
        return;
    }

    if (value < 1)
    {
        mTopSlice = 1;
        return;
    }
    if(mBottomSlice >= mTopSlice)
        mBottomSlice = mTopSlice - 1;
}

int16_t KhazSlider::getTopSlice() const
{
    return mTopSlice;
}

void KhazSlider::setBottomSlice (int16_t value)
{
    mBottomSlice = value;
    if (value < 0)
    {
        mBottomSlice = 0;
        return;
    }
    
    if (value > mMaxZ - 1)
    {
        mBottomSlice = mMaxZ - 1;
        return;
    }
    if(mTopSlice <= mBottomSlice)
        mTopSlice = mBottomSlice + 1;
}

int16_t KhazSlider::getBottomSlice() const
{
    return mBottomSlice;
}

bool KhazSlider::SlideUp( void )
{
    if(mTopSlice < mMaxZ)
    {
        mTopSlice ++;
        mBottomSlice ++;
        return true;
    }
    return false;
}

bool KhazSlider::SlideDown( void )
{
    if(mBottomSlice > 0)
    {
        mTopSlice --;
        mBottomSlice --;
        return true;
    }
    return false;
}

int KhazSlider::getMarkerLength() const
{
    return mMarkerLength;
}

void KhazSlider::setMarkerLength (int length)
{
    mMarkerLength = length;
}

void KhazSlider::keyPressed (KeyEvent& keyEvent)
{
    Key key = keyEvent.getKey();

    if (key.getValue() == Key::UP)
    {
        SlideUp();
        distributeActionEvent();
        keyEvent.consume();
    }
    else if (key.getValue() == Key::DOWN)
    {
        SlideDown();
        distributeActionEvent();
        keyEvent.consume();
    }
}

int16_t KhazSlider::markerPositionToValue (int v) const
{
    // v = 0 -- v is on bottom
    // move things a bit to center the focus point
    v -= getMarkerLength() / 4;
    
    // total height of the widget
    int w = getHeight(); 
    // height of the active scale
    int w2 = w - 2 * getMarkerLength() - 2 * 16; 
    v = w - v;
    v= max(v, getMarkerLength() + 16) - (getMarkerLength() + 16);
    v= min(v, w2);
    // v is now in range of valid values
    double pos = v / ( (double) w2 );
    return pos * getMaxZ();
}

int KhazSlider::valueToMarkerPosition (int16_t value) const
{
    // real scale is between the handles, limited by bottom of top one in extreme position and top of bottom one in ex.p.
    double v = getHeight() - 2 * getMarkerLength() - 2* 16;
    double scale = ((double) value) / ((double) getMaxZ());
    return getHeight() - (getMarkerLength() + 16 + v * scale);
}

int KhazSlider::getTopMarkerPosition() const
{
    return valueToMarkerPosition (getTopSlice());
}

int KhazSlider::getBottomMarkerPosition() const
{
    return valueToMarkerPosition (getBottomSlice());
}

void KhazSlider::mouseWheelMovedUp (MouseEvent& mouseEvent)
{
    SlideUp();
    distributeActionEvent();

    mouseEvent.consume();
}

void KhazSlider::mouseWheelMovedDown (MouseEvent& mouseEvent)
{
    SlideDown();
    distributeActionEvent();

    mouseEvent.consume();
}
}
