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



// $Id: text.h,v 1.5 2008/05/05 23:32:50 enska Exp $

#ifndef WIDGET_TEXT_H
#define WIDGET_TEXT_H

#include "common.h"
#include "formatter.h"
#include "buffer.h"
#include "widget.h"
#include <string>
#include <cassert>

namespace cli
{
    class text : public widget
    {
    public:
       ~text() {}
        text() : color_(COLOR_NONE), attrib_(ATTRIB_NONE), width_(0) {}

        rect draw(buffer& fb)
        {
            cell c = {' ', attrib_, color_};
            formatter f(c, fb);
            
            f.move(xpos_, ypos_);
            f.print(text_.c_str(), width_);
            
            rect ret = {ypos_, xpos_, xpos_ + width_, ypos_+1};
            return ret;
        }

        int height() const
        {
            return 1;
        }

        int width() const
        {
            return width_;
        }

        // Set new text string to be displayed.
        void settext(const std::string& s)
        {
            if (s == text_) return;
            text_ = s;
            if (width_ == 0)
                width_ = static_cast<int>(s.length());
            valid_ = false;
        }
        
        // Get current text.
        std::string gettext() const
        {
            return text_;
        }
        
        // Set text color.
        void setcol(short col)
        {
            if (col != color_)
            {
                color_ = col;
                valid_ = false;
            }
        }

        // Set text attributes.
        void setattrib(short attrib)
        {
            if (attrib != attrib_)
            {
                attrib_ = attrib;
                valid_  = false;
            }
        }

        // Set text field width.
        void width(int width)
        {
            if (width != width_)
            {
                width_ = width;
                valid_ = false;
            }
        }
    private:
        short  color_;
        short  attrib_;
        int    width_;
        std::string text_;
    };

} // cli

#endif // WIDGET_TEXT_H
