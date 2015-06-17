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

// $id: view.h,v 1.1 2007/04/20 00:05:38 enska Exp $

#ifndef WIDGET_VIEW_H
#define WIDGET_VIEW_H

#include "widget.h"
#include "common.h"
#include "pager.h"
#include "formatter.h"
#include "buffer.h"
#include <algorithm>

namespace cli
{
    class default_no_selection 
    {
    public:
        
        // return the current position
        int selpos() const
        {
            return row_;
        }
        
        void selpos(int pos) 
        {
            row_ = pos;
        }
    protected:
       ~default_no_selection() {}
        default_no_selection() : row_(0) {}
        
        bool keydown(int vk, int page_height, int max, bool invalid)
        {
            if (max == 0 || page_height == 0)
                return false;
            invalid = false;
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

                default:
                    return false;
            }
            return true;
        }
        
        bool is_selected(int row) const
        {
            return false;
        }
        bool reset() const 
        {
            return false;
        }
    private:
        int row_;
    };


    // basic_view template displays just some arbitrary rows of data.
    // It doesn't support selection or colors, instead it displays a cursor.
    template<typename Database,
             typename Selector = default_no_selection,
             typename Pager    = default_pager>
    class basic_view : public widget, 
      public Database, public Selector, public Pager
    {
    public:
        basic_view() : focus_(false), showcaret_(true), width_(0), height_(0) {}

        bool can_focus() const
        {
            return Database::size() != 0;
        }
        void set_focus(bool focus) 
        {
            if (Database::size() == 0 && focus == true)
                return;
            focus_ = focus;
        }
        void set_cursor(cursor& c)
        {
            if (!showcaret_)
            {
                c.v = false;
                return;
            }
            c.x = xpos_;
            c.y = ypos_ + Pager::pagepos(Selector::selpos(), height_);
            c.v = true;
        }
        rect draw(buffer& fb)
        {
            typedef typename Database::value     value;
            typedef typename Database::converter converter;

            cell def = {' ', ATTRIB_NONE, COLOR_NONE};
            formatter f(def, fb);
            f.fillblank(true);

            rect ret  = {};
            ret.left  = xpos_;
            ret.right = xpos_ + width_;
            int xpos = xpos_;
            int ypos = ypos_;

            std::pair<int, int> range = Pager::getpage(Selector::selpos(), height_, Database::size());
            for (int i = range.first; i!= range.second; ++i, ++ypos)
            {
                if (!Pager::is_dirty(i, height_))
                    continue;
                if (ret.bottom == 0)
                    ret.top = ret.bottom = ypos;
                ++ret.bottom;
                f.move(xpos, ypos);
                if (i >= Database::size())
                {
                    // blank line
                    f.print("", width_);
                    continue;
                }
                value val;
                Database::fetch(val, i);
                
                converter c(val);
                f.print(c.str(), c.len(), width_);
            }
            return ret;
        }
      
        // Mark this widget as invalid. Drawing is needed.
        void invalidate(bool force)
        {
            if (force)
                Pager::invalidate();
            valid_ = false;
        }
        
        // Mark this widget as valid. No drawing needed right now.
        void validate()
        {
            Pager::validate(Selector::selpos(), height_);
            valid_ = true;
        }

        // Process a keydown event.
        bool keydown(int key, int vk)
        {
            bool invalid = false;
            if (Selector::keydown(vk, height_, Database::size(), invalid))
            {
                if (invalid)
                    Pager::invalidate();
                valid_ = false;
                return true;
            }
            return false;
        }

        // Set view width.
        void width(int width)
        {
            width_ = width;
            valid_ = false;
        }

        // Set view height.
        void height(int height)
        {
            height_ = height;
            valid_  = false;
        }

        // Get view width.
        int width() const
        {
            return width_;
        }

        // Get view height.
        int height() const
        {
            return height_;
        }

        // Control whether to show caret or not when focused.
        // The default is to show the caret.
        void showcaret(bool val)
        {
            showcaret_ = val;
        }
    private:
        bool focus_;
        bool showcaret_;
        int width_;
        int height_;
    };

} // cli

#endif // WIDGET_VIEW_H

