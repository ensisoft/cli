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

#include "config.h"

#include "widget.h"
#include "common.h"
#include "pager.h"
#include "formatter.h"
#include "ticker.h"
#include "singlesel.h"
#include <vector>
#include <memory>
#include <cassert>
#include <string>

namespace cli
{
    // basic_list is a "list box" widget template. It simply displays a list of data.
    // It can be configured with the following policies.
    // 
    // Database - This is the data access policy. Database policy needs to provide
    // a random access to the actual data to be displayed and it also needs to provide
    // a converter type that can convert the data into strings ready for printing.
    // This implies applying conversion and formatting in case of a non-string data.
    //
    // Ticker - Ticker policy provides vertical text scrolling. The default is an "empty do-nothing"
    // implementation.
    // 
    // Pager - Paging policy provides an algorithm for paging. Paging is the process of splitting
    // multiple rows of data into smaller chunks (pages) that get displayed one at a time.
    template <typename Database,
              typename Selector = default_single_selection,
              typename Ticker   = default_ticker,
              typename Pager    = default_pager> 
    class basic_list : public widget, 
      public Database, public Selector, public Ticker, public Pager
    {
    public:
        basic_list() : fill_(false), focus_(false), color_(COLOR_INACTIVE), width_(0), height_(0) {} 

        bool can_focus() const 
        {
            return Database::size() != 0;
        }
        void set_focus(bool focus) 
        { 
            if (Database::size() == 0 && focus == true)
                return;
            focus_ = focus; 
            if (!focus_)
                Ticker::reset();
        }
        rect draw(buffer& fb)
        {
            typedef typename Database::value     value;
            typedef typename Database::converter converter;

            const cell def = {' ', ATTRIB_NONE, COLOR_NONE};       // default color
            const cell sel = {' ', ATTRIB_NONE, COLOR_SELECTION};  // selection color when focused
            const cell col = {' ', ATTRIB_NONE, color_};           // selection color when not focused
            formatter f(fb);
            f.setdef(def);
            f.setblank(def);
            f.fillblank(fill_);

            rect ret  = {};
            ret.left  = xpos_;
            ret.right = xpos_ + width_;
            int ypos = ypos_;
            int xpos = xpos_;
            
            std::pair<int, int> range = Pager::getpage(Selector::selpos(), height_, Database::size());
            for (int i = range.first; i != range.second; ++i, ++ypos)
            {
                if (!Pager::is_dirty(i, height_))
                    continue;
                if (ret.bottom == 0)
                    ret.top = ret.bottom = ypos;
                ret.bottom = ypos + 1;
                f.move(xpos, ypos);
                f.setdef(def);
                if (i >= Database::size())
                {
                    // blank line
                    f.print("", width_);
                    continue;
                }
                if (Selector::is_selected(i))
                {
                    if (focus_)
                        f.setdef(sel);
                    else
                        f.setdef(col);
                }
                value val;
                Database::fetch(val, i);

                converter c(val);
                f.print(c.str(), c.len(), width_);
            }
            return ret;
        }

        rect animate(buffer& fb, int elapsed)
        {
            rect ret = {};
            typedef typename Database::value     value;
            typedef typename Database::converter converter;
            if (!focus_ || Database::size() == 0)
                return ret;
            if (Ticker::is_idle(elapsed))
                return ret;
            int row = Selector::selpos();
            int pos = Pager::pagepos(row, height_);
            if (!Ticker::is_set())
            {
                value val;
                Database::fetch(val, row);
                converter conv(val);
                std::string str(conv.str(), conv.len());
                Ticker::set(str, width_);
            }
            Ticker::scroll(fb, xpos_, ypos_ + pos);
            ret.top    = ypos_ + pos;
            ret.bottom = ypos_ + pos + 1;
            ret.left   = xpos_;
            ret.right  = xpos_ + width_;
            return ret;
        }

        bool keydown(int, int vk)
        {
            bool invalid = false;
            if (Selector::keydown(vk, height_, Database::size(), invalid))
            {
                if (invalid)
                    Pager::invalidate();
                Ticker::reset();
                valid_ = false;
                return true;
            }
            return false;
        }

        void invalidate(bool force)
        {
            if (force)
                Pager::invalidate();
            Ticker::reset();
            valid_ = false;
        }

        void validate()
        {
            Pager::validate(Selector::selpos(), height_);
            valid_ = true;
        }

        void resetmark()
        {
            if (Selector::reset())
                Pager::invalidate();
        }

        // Set listbox width.
        void width(int width)
        {
            width_ = width;
            valid_ = false;
        }
        
        // Set listbox height.
        void height(int height)
        {
            height_ = height;
            valid_  = false;
        }

        // Get list width.
        int width() const
        {
            return width_;
        }

        // Get list height.
        int height() const
        {
            return height_;
        }

        void fill_highlight(bool val)
        {
            fill_=val;
        }

        void inactive_color(color col)
        {
            color_ = col;
        }
    private:
        bool  fill_;
        bool  focus_;
        short color_;
        int   width_;
        int   height_;
    };

} // cli



