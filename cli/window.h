//
// Copyright (c) 2007 Sami Väisänen
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//


// $Id: window.h,v 1.4 2008/05/05 23:32:50 enska Exp $

#ifndef WIDGET_WINDOW_H
#define WIDGET_WINDOW_H

#include <boost/utility.hpp>
#include <vector>
#include "common.h"

namespace cli
{
    class menu;
    class widget ;
    class buffer;

    // A window object manages a bunch of widgets and gives them a common context and frame.
    class window : boost::noncopyable
    {
    public:
        // Evtdraw event will be invoked when the window is dirty and needs painting.
        // As a response to this event application should call the draw function.
        // Inside the draw function one should be careful not to make any calls to 
        // to the window object, since this maybe result in the event being fired recursively.
        boost::function1<void, window*> evtdraw;
        
        // Evterase event will be invoked when a framebuffer area needs to be
        // erased/cleared to remove any remaining output.
        // If the whole framebuffer should be erased right and bottom will be set to 
        // std::numeric_limits<size_t>::max() value.
        // If this event is invoked it is invoked during the draw operation.
        boost::function2<void, window*, rect> evterase;

        // Evtfocus event will be invoked when the window changes the focused widget.
        boost::function1<void, window*> evtfocus;
        
        // Evtcursor event will be invoked when the window changes the current cursor state.
        boost::function2<void, window*, cursor> evtcursor;

       ~window();
        window();
        
        // Add a widget into this window.
        // If window is open this will invalidate the added widget and request a redraw.
        void add(widget* w);

        // Add a menu widget to this window.
        // If window is open this will invalidate the added widget and request a redraw.
        // If window already has a menu this will replace that menu.
        void add(menu* m);

        // Remove a widget from this window.
        // If window is open this will request the application to clear the
        // frame buffer through evtdraw_bg event. It will then invalidate all 
        // widgets and request a redraw.
        void rem(widget* w);

        // Try to move the focus to this widget. If the widget is not focusable or already 
        // has focus nothing will be done.
        // Returns true if focus was set, otherwise false.
        bool focus(widget* w);

        // Request a widget to redrawn.
        void update(widget* w);
            
        // Move a widget to a new location. Moving a widget will invalidate
        // the whole window and will reqclire redrawing.
        void move(widget* w, int xpos, int ypos);
        
        // Get the abstract cursor state.
        const 
        cursor& curs() const;

        // Get currently focused widget. Can be NULL if none of the widgets are focusable.
        const
        widget* focused() const;        
        widget* focused();

        // Request the window to draw  the currently dirty widgets
        // into the specified frame_buffer. The invalid rectangles from all
        // dirty widgets are combined into a one final rectangle that describes
        // the union of all dirty rectangles. This is the area that has changed 
        // in the frame buffer and should be transferred to the terminal.
        rect draw(buffer& fb);
        
        // Request the window to call animate on every widget. 
        // The invalid rectangles from Widgets that support some kind of animation are 
        // combined into one final rectangle that describes the union of all dirty
        // rectangles. This is the area that has changed in the the frame buffer
        // and should be transferred to the terminal.
        rect animate(buffer& fb, int elapsed);
            
        // Invalidate all widgets. Will force complete redraw.            
        void invalidate();

        // Process a keypress. Raw should be the raw ASCII character
        // that was read as an input. VK may describe a virtual key event that is
        // to be executed. If no virtual key press was generated by the input vk should be -1.
        // In a case where both raw and vk are applicable the behaviour is widget defined.
        // The function returns true if the key press was handled. Otherwise it returns false.
        // If the key event processing causes updates to any widgets (and they become invalid)
        // the draw event will be invoked.
        bool keydown(int raw, int vk);

        // Set the close flag on this window.
        void close(); 

        // Showing a window will prepare the window for display. Window
        // will find the first focusable widget (if any) and also setup the cursor. 
        void show();

        bool is_valid() const;
        bool is_open() const;
        bool has_widget(const widget* w) const;
            
        // Disable/enable VK_KILL_WINDOW.
        void can_close_on_vk(bool val);
    private:     
        
        std::vector<widget*> circus_;

        widget* focused_; 
        menu*   menu_;
        bool can_close_;
        bool is_valid_;
        bool is_open_;
        cursor cursor_;
        rect rc_erase_;
    };

} // cli

#endif // WIDGET_WINDOW_H

