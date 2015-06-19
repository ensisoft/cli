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
#include "formatter.h"
#include "widget.h"
#include "buffer.h"
#include <string>
#include <cassert>

namespace cli
{
    class progressbar : public widget
    {
    public:
       ~progressbar() {}
        progressbar() : width_(0), low_(0), high_(100), pos_(0) {}
        
        rect draw(buffer& fb)
        {
            cell text = {' ', ATTRIB_NONE, COLOR_NONE};
            cell fill = {'=', ATTRIB_NONE, COLOR_NONE};
            formatter f(text, fb);
            f.move(xpos_, ypos_);
            f.print("[", width_);
            
            float x = float((pos_ - low_)) / float((high_ - low_));
            int fc  = (int)(x * (width_ - 2));
            f.move(xpos_ + 1, ypos_);
            f.setdef(fill);
            if (fc > width_ - 2)
                fc = width_ - 2;
            if (x > 0 && fc > 0)
                f.print("", fc); 

            f.setdef(text);
            f.move(xpos_ + width_ - 1, ypos_);
            f.print("]", 1);

            for (int i(0); i<(int)text_.size(); ++i)
            {
                int x = xpos_ + (width_ - 2 - (int)text_.size()) / 2;
                if (x + i < fc)
                    f.setdef(fill);
                else
                    f.setdef(text);
                f.move(x + i, ypos_);
                f.print(&text_[i], 1);
            }

            rect rc = {ypos_, xpos_, xpos_ + width_, ypos_ + 1};
            return rc;
        }
        
        int height() const
        {
            // one row only
            return 1;
        }

        int width() const
        {
            return width_;
        }

        void width(int width)
        {
            valid_ = false;
            width_ = width;
        }

        void setrange(int low, int high)
        {
            assert(high >= low);
            low_   = low;
            high_  = high;
            valid_ = false;
        }
        void setpos(int pos)
        {
            pos_ = pos;
            valid_ = false;
        }
        int getpos() const
        {
            return pos_;
        }
        std::pair<int, int> getrange() const
        {
            return std::make_pair(low_, high_);
        }

        void settext(const std::string& s)
        {
            if (s == text_) return;
            text_  = s;
            valid_ = false;
        }
        std::string gettext() const
        {
            return text_;
        }
    private:
        int width_;
        int low_;
        int high_;
        int pos_;
        std::string text_;
    };

} // cli

