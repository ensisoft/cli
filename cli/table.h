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

#include "widget.h"
#include "common.h"
#include "ticker.h"
#include "pager.h"
#include "singlesel.h"
#include "buffer.h"

#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

namespace cli
{
    // basic_table is a "table" widget template. It is very like to basic_list
    // except that it supports multiple columns. In fact a list could also be 
    // presented with a single column table.
    // 
    // For information about the policies see basic_list template.
    template<typename Database,
             typename Selector = default_single_selection,
             typename Ticker   = default_ticker,
             typename Pager    = default_pager>
    class basic_table : public widget, 
      public Database, public Selector, public Ticker, public Pager
    {
    public:
        basic_table() : focus_(false), color_(COLOR_INACTIVE), width_(0), height_(0), cellspacing_(0) {}

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
            typedef typename vector_type::const_iterator iter;

            cell def = {' ', ATTRIB_NONE, COLOR_NONE};
            cell sel = {' ', ATTRIB_NONE, COLOR_SELECTION};
            cell col = {' ', ATTRIB_NONE, color_};
            formatter f(def, fb);
            f.fillblank(true);

            rect ret  = {};
            ret.left  = xpos_;
            ret.right = xpos_ + width_;
            int ypos  = ypos_;
            int xpos  = xpos_;

            std::pair<int, int> range = Pager::getpage(Selector::selpos(), height_, Database::size());
            for (int i = range.first; i != range.second; ++i, ++ypos)
            {
                xpos  = xpos_;
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
                
                int width = width_;
                int print = 0;
                for (int x=0; x<(int)columns_.size(); ++x)
                {
                    const column& col = columns_[x];
                    if (col.width == 0) continue; // skip 0 length columns
                    converter c(val, x);
                    const char*   str = c.str();
                    size_t        len = c.len();
                    print = std::min<int>(width, col.width);
                    f.move(xpos, ypos);
                    f.print(str, len, print);
                    width -= print;
                    if (width==0)  break;
                    // print cellspacing
                    print = std::min<int>(width, cellspacing_);
                    f.move(xpos + col.width, ypos);
                    f.print("", print);
                    width -= print;
                    xpos  += col.width + cellspacing_;
                    if (width==0) break;
                }
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
                std::stringstream ss;
                value val;
                Database::fetch(val, row);
                for (int x=0; x<(int)columns_.size(); ++x)
                {
                    const column& col = columns_[x];
                    if (col.width == 0) continue; // skip 0 length columns
                    converter conv(val, x);
                    const char*   str = conv.str();
                    size_t        len = conv.len();
                    std::string   tmp(str, len);
                    ss << std::left << std::setw(col.width) << tmp;
                    ss << std::setw(cellspacing_) << "";
                }
                Ticker::set(ss.str(), width_);
            }
            Ticker::scroll(fb, xpos_, ypos_ + pos);
            ret.top    = ypos_ + pos;
            ret.bottom = ypos_ + pos + 1;
            ret.left   = xpos_;
            ret.right  = xpos_ + width_;
            return ret;
        }
        
        // Get the range of currently visible items.
        std::pair<int, int> visible() const
        {
            return Pager::getvisible(Selector::selpos(), height_, Database::size());
        }
        
        bool keydown(int key, int vk)
        {
            bool invalid = true;
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

        void addcol(int width)
        {
            column c;
            c.width = width;
            columns_.push_back(c);
            valid_  = false;
        }
        void remcols() 
        {
            // remove all columns 
            columns_.clear();
        }
        void cellspacing(int spacing)
        {
            assert(spacing > 0);
            cellspacing_ = spacing;
            valid_ = false;
        }
        
        // Set table width
        void width(int width)
        {
            width_ = width;
            valid_ = false;
        }

        // Set table height
        void height(int height)
        {
            height_ = height;
            valid_  = false;
        }

        void inactive_color(color col)
        {
            color_ = col;
        }

        // Get table width.
        int width() const
        {
            return width_;
        }
        
        // Get table height.
        int height() const
        {
            return height_;
        }

    private:
        struct column {
            std::size_t width;
        };
        typedef std::vector<column> vector_type;

        bool focus_;
        short color_;
        int width_;
        int height_;
        int cellspacing_;
        std::vector<column> columns_;
       
    };

} // cli


