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

#ifndef KHAZSLIDER_HPP
#define KHAZSLIDER_HPP

#include "guichan/keylistener.hpp"
#include "guichan/mouselistener.hpp"
#include "guichan/platform.hpp"
#include "guichan/widget.hpp"
#include "guichan/graphics.hpp"
#include <stdint.h>
namespace gcn
{


/**
 * An implementation of a slider where a user can select different values by
 * sliding between a start value and an end value of a scale.
 *
 * If the selected value is changed an action event will be sent to all
 * action listeners of the slider.
 */
class GCN_CORE_DECLSPEC KhazSlider :
            public gcn::Widget,
            public gcn::MouseListener,
            public gcn::KeyListener
{
public:

    enum DragType
    {
        NO_DRAG,
        DRAG_TOP,
        DRAG_CENTER,
        DRAG_BOTTOM
    };
    /**
     * Constructor. The default start value of the slider scale is zero.
     *
     * @param tophandle Filename for the top handle image
     * @param bottomhandle Filename for the bottom handle image
     * @param maxZ The end value of the slider scale.
     */
    KhazSlider (const std::string& tophandle, const std::string& bottomhandle,int16_t maxZ);

    /**
     * Destructor - kills the images
     */
    ~KhazSlider();

    /**
     * Gets the end value of the scale.
     *
     * @return The end value of the scale.
     * @see setScaleEnd, setScale
     */
    int16_t getMaxZ() const;

    /**
     * Sets the end value of the scale.
     *
     * @param maxZ The end value of the scale.
     * @see getMaxZ
     */
    void setMaxZ (int16_t maxZ);

    /**
     * Slides both levels up by 1
     * @return Operation succeeded?
     */
    bool SlideUp( void );

    /**
    * Slides both levels down by 1
    * @return Operation succeeded?
    */
    bool SlideDown( void );

    /**
     * Gets the current selected value.
     *
     * @return The current selected value.
     * @see setTopSlice
     */
    int16_t getTopSlice() const;

    /**
    * Gets the current selected value.
    *
    * @return The current selected value.
    * @see setBottomSlice
    */
    int16_t getBottomSlice() const;

    /**
     * Sets the top slice level
     *
     * @param value The current selected value.
     * @see getTopSlice
     */
    void setTopSlice (int16_t value);

    /**
    * Sets the bottom slice level
    *
    * @param value The current selected value.
    * @see getBottomSlice
    */
    void setBottomSlice (int16_t value);

    /**
     * Sets the length of the marker.
     *
     * @param length The length for the marker.
     * @see getMarkerLength
     */
    void setMarkerLength (int length);

    /**
     * Gets the length of the marker.
     *
     * @return The length of the marker.
     * @see setMarkerLength
     */
    int getMarkerLength() const;

    // Inherited from Widget

    virtual void draw (Graphics* graphics);


    // Inherited from MouseListener.

    virtual void mousePressed (MouseEvent& mouseEvent);

    virtual void mouseReleased(MouseEvent& mouseEvent);

    virtual void mouseDragged (MouseEvent& mouseEvent);

    virtual void mouseWheelMovedUp (MouseEvent& mouseEvent);

    virtual void mouseWheelMovedDown (MouseEvent& mouseEvent);


    // Inherited from KeyListener

    virtual void keyPressed (KeyEvent& keyEvent);

protected:
    /**
     * Draws the top marker.
     *
     * @param graphics A graphics object to draw with.
     */
    virtual void drawTopMarker (gcn::Graphics* graphics);

    /**
    * Draws the bottom marker.
    *
    * @param graphics A graphics object to draw with.
    */
    virtual void drawBottomMarker (gcn::Graphics* graphics);

    /**
     * Converts a marker position to a value in the scale.
     *
     * @param position The position to convert.
     * @return A scale value corresponding to the position.
     * @see valueToMarkerPosition
     */
    virtual int16_t markerPositionToValue (int position) const;

    /**
     * Converts a value to a marker position.
     *
     * @param value The value to convert.
     * @return A marker position corresponding to the value.
     * @see markerPositionToValue
     */
    virtual int valueToMarkerPosition (int16_t value) const;

    /**
    * Gets the marker position of the current slice top
    *
    * @return The marker position of the current slice top
    */
    virtual int getTopMarkerPosition() const;

    /**
    * Gets the marker position of the current slice bottom
    *
    * @return The marker position of the current slice bottom
    */
    virtual int getBottomMarkerPosition() const;

    /**
     * Current drag operation type
     */
    DragType mDragged;

    /**
    * Offset to use for drag y value
    */
    int mDragOffset;

    /**
     * Holds the current top slice value.
     */
    int16_t mTopSlice;

    /**
    * Holds the current bottom slice value.
    */
    int16_t mBottomSlice;

    /**
     * Holds the length of the marker.
     */
    int mMarkerLength;

    /**
     * Holds the end value of the scale.
     */
    int16_t mMaxZ;

    /**
     * Image for the top handle
     */
    const Image* mImageTop;

    /**
    * Image for the bottom handle
    */
    const Image* mImageBottom;
};
}

#endif // end KHAZSLIDER_HPP
