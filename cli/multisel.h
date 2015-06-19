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

#include "common.h"
#include <cassert>
#include <algorithm>

namespace cli
{
    class default_multi_selection
    {
    public:
        enum { MARK_NOT_SET = -1 };
        
        struct range {
            int start;
            int end;
        };

        // Fired when selection key is pressed.
        event0 evtselect;

        // Fired when selected row changes.
        event0 evtrow;

        void selpos(int pos) 
        {
            row_ = pos;
        }
        
        // Return the current selection position.
        int selpos() const
        {
            return row_;
        }

        range selrange() const
        {
            range r = {0, 0};
            
            if (mark_ != MARK_NOT_SET)
            {
                r.start = std::min<int>(row_, mark_);
                r.end   = std::max<int>(row_, mark_) + 1;
            }
            else
            {
                r.start = row_;
                r.end   = row_ + 1;
            }
            return r;
        }
    protected:
       ~default_multi_selection() {}
        default_multi_selection() : row_(0), mark_(-1) {}

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
                    break;
                    
                case VK_MOVE_DOWN:
                    if (row_ < max - 1)
                        ++row_;
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
                    
                case VK_SET_MARK:
                    mark_ = (mark_ != MARK_NOT_SET ? MARK_NOT_SET : row_);
                    if (mark_ == MARK_NOT_SET)
                        invalid = true;
                    break;
                    
                case VK_ACTION_SPACE:
                case VK_ACTION_ENTER:
                    if (evtselect)
                        evtselect();
                    if (mark_ != MARK_NOT_SET)
                    {
                        mark_ = MARK_NOT_SET;
                        invalid = true;
                        break;
                    }
                    return false;
                    
                default:
                    return false;
            }
            if (old != row_ && evtrow)
                evtrow();
            
            return true;
        }
        
        bool is_selected(int row) const
        {
            if (mark_ == MARK_NOT_SET)
                return row == row_;
            
            if (row_ > mark_)
            {
                // rubberband going down
                return row >= mark_ && row <= row_;
            }
            if (row_ < mark_)
            {
                // rubberband going up
                return row >= row_ && row <= mark_;
            }
            return row_ == mark_ && mark_ == row;
        }
        
        bool reset()
        {
            mark_ = MARK_NOT_SET;
            return true;
        }

    private:
        int row_;
        int mark_;
    };

} // cli


