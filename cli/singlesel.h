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

// $Id: singlesel.h,v 1.2 2008/05/05 23:32:50 enska Exp $

#ifndef WIDGET_SINGLESEL_H
#define WIDGET_SINGLESEL_H

#include "common.h"
#include <cassert>

namespace cli
{
    // Implements single selection for listbox and table widgets.
    class default_single_selection
    {
    public:
        // Fired when selection key is pressed.
        event0 evtselect;
        
        // Fired when selected row changes.
        event0 evtrow;
        
        // Set selection position.
        void selpos(int pos)
        {
            row_ = pos;
        }

        // Return the current selection position.
        int selpos() const
        {
            return row_;
        }
    protected:
       ~default_single_selection() {}
        default_single_selection() : row_(0) {}

        bool keydown(int vk, int page_height, int max, bool& invalid)
        {
            if (max == 0 || page_height == 0)
                return false;

            invalid = false;
            int old = row_;
            switch (vk)
            {
                case VK_MOVE_UP:
                    if (row_ > 0)
                        --row_;
                    else
                        row_ = max-1;  // wrap over to the last item
                    break;

                case VK_MOVE_DOWN:
                    row_ = (row_ + 1) % max;
                    break;

                case VK_MOVE_HOME:
                    row_ = 0;
                    break;
                    
                case VK_MOVE_END:
                    row_ = max - 1;
                    break;
                    
                case VK_MOVE_UP_PAGE:
                    row_ -= page_height;
                    if (row_ < 0)
                        row_ = 0;
                    break;
                    
                case VK_MOVE_DOWN_PAGE:
                    row_ += page_height;
                    if (row_ >= max)
                        row_ = max - 1;
                    break;
                    
                case VK_ACTION_SPACE:
                case VK_ACTION_ENTER:
                    if (evtselect)
                        evtselect();
                    return false; 

                default:
                    return false;
            }
            if  (old != row_ && evtrow)
                evtrow();

            return true;
        }

        bool is_selected(int row) const
        {
            return row == row_;
        }
        bool reset() const
        {
            return false;
        }
    private:
        
        int row_;
    };

} // cli

#endif // WIDGET_SINGLESEL_H


