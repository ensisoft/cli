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

#pragma once

#include <algorithm>
#include <boost/function.hpp>
#ifdef WINDOWS
#  undef COLOR_HIGHLIGHT
#endif

namespace cli
{

struct size
{
    union {
        int x;
        int cols;
    };
    union {
        int y;
        int rows;
    };
};


typedef boost::function0<void> event0;
typedef boost::function1<void, int> event1;
typedef boost::function2<void, int, int> event2;

// Supported widget colors. Only few of these colors are used
// by the widgets directly (mostly just COLOR_NONE and COLOR_SELECTION).
// The rest of the colors are some that the client application can set and specify if needed.
// Whether any of the colors are actually supported depends on the backend
// that is eventually used to transfer the frame_buffer into the terminal.
enum color 
{
    COLOR_NONE,               // No specific color. Terminal default colors should be used.
    COLOR_HIGHLIGHT,          // Generic highlight color.
    COLOR_SELECTION,          // Active selection (has focus) color. 
    COLOR_INACTIVE,           // Inactive (not focused) selection color.
    COLOR_STATIC,             // Static object (text) color.
    COLOR_MENUITEM,           // Menclitem color
    COLOR_ATTENTION,          // Attention color. 
    COLOR_QUESTION,           // Question color.
    COLOR_INFORMATION,        // Information color.
    COLOR_SENTINEL            // Always the last color. Not used by widgets, but can be used as a starting value for application defined colors.
};


// Video text attributes. Values can be OR'ed together.
// Whether any of these is supported or has any effect depends
// on the backend that is eventually used to transfer the frame_buffer into the terminal.
// Most widgets will default to using ATTRIB_NONE unless explicitly told to use something else.
enum attrib 
{
    ATTRIB_NONE           = 0x0,  // Default specific attributes. Terminal default attributes should be used.
    ATTRIB_REVERSE_VIDEO  = 0x1,  // Reverse video color.
    ATTRIB_UNDERLINE      = 0x2,  // Underline text.
    ATTRIB_STANDOUT       = 0x4,  // Highlight mode.
    ATTRIB_BLINK          = 0x8,  // Blink text annoyingly.
    ATTRIB_DIM            = 0x10, // Dim text.
    ATTRIB_BOLD           = 0x20  // Bold text.
};


// Cell object contains information for a single character cell in the frame buffer.
// In other words for a screen size of M, N there are is a cell object for each 
// character in a M*N array. See frame_buffer for more.
struct cell 
{
    int   value; 
    short attrib;
    short color;
};

// Make cell object from value v, attrib a and color c.
inline
cell make_cell(int v, short a, short c)
{
    cell cl = {v, a, c};
    return cl;
}

inline
bool operator==(const cell& lhs, const cell& rhs)
{
    return lhs.value == rhs.value &&
      lhs.attrib == rhs.attrib &&
      lhs.color == rhs.color;
}

// Cursor represents an "abstract cursor". Widgets update the cursor 
// values to reflect their idea about where the cursor should be. 
// Application will be responsible for translating this information
// for a particular terminal.
struct cursor 
{
    int  x;     // X location
    int  y;     // Y location
    bool v;     // Visibility. True for visible, false for not visible.
};

// Rect simply describes a rectangle.
struct rect 
{
    int top;     // top left
    int left;
    int right;
    int bottom;  // bottom right
};

inline
rect rect_union(const rect& lhs, const rect& rhs)
{
    if (lhs.right == 0 || lhs.bottom == 0)
        return rhs;
    if (rhs.right == 0 || rhs.bottom == 0)
        return lhs;
    rect ret = {};
    ret.top    = std::min<size_t>(lhs.top, rhs.top);
    ret.left   = std::min<size_t>(lhs.left, rhs.left);
    ret.right  = std::max<size_t>(lhs.right, rhs.right);
    ret.bottom = std::max<size_t>(lhs.bottom, rhs.bottom);
    return ret;
}

inline
bool rect_is_empty(const rect& rc)
{
    if (rc.bottom == 0 || rc.right == 0)
        return true;
    return false;
}

inline
bool rect_intersects_rect(const rect& rect0, const rect& rect1)
{
    rect res;
    res.right  = std::min<int>(rect0.right, rect1.right);
    res.bottom = std::min<int>(rect0.bottom, rect1.bottom);
    res.left   = std::max<int>(rect0.left, rect1.left);
    res.top    = std::max<int>(rect0.top, rect1.top);
    if(((res.right - res.left) > 0) && ((res.bottom - res.top) > 0))
    {
        return true;
    }
    else
    {
        res.left = res.top = res.right = res.bottom = 0;
        return false;
    }
}


// Virtual keys for widgets.
enum vk_keys
{
    VK_MOVE_UP,             // move up a line
    VK_MOVE_DOWN,           // move down a line
    VK_MOVE_UP_PAGE,        // move up a page
    VK_MOVE_DOWN_PAGE,      // move down a page
    VK_SET_MARK,            // toggle mark    
    VK_ACTION_SPACE,        // action on space key (could be rebound)
    VK_ACTION_ENTER,        // action on enter key (could be rebound)
    VK_ERASE,               // erase, backspace (could be rebound)
    VK_FOCUS_NEXT,          // move focus to next focusable widget
    VK_FOCUS_PREV,          // move focus to previous focusable widget
    VK_OPEN_MENU,           // open window menu (if any)
    VK_TAB_COMPLETE_NEXT,   // tab complete (select next completion from the list)
    VK_TAB_COMPLETE_PREV,   // tab complete (select prev completion from the list)
    VK_MOVE_HOME,           // move to the beginning of data (first line)
    VK_MOVE_END,            // move to the end of data (last line)
    VK_MOVE_NEXT,           // move to a next character on a single line
    VK_MOVE_PREV,           // move to a previous character on a single line
    VK_KILL_LINE,           // kill the current line of text starting at the cursor
    VK_KILL_CHAR,           // kill the current character (the one the cursor is on)
    VK_KILL_WINDOW,         // kill (close) window. window can override this virtual key.
    VK_SENTINEL             // Always the last key. Not used by widgets for anything, but can be used for a starting range for application keys.
};

} // cli


