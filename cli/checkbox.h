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
#include "formatter.h"
#include <functional>
#include <string>

namespace cli
{
    // todo:
    class checkbox : public widget
    {
    public:
        // Evtclick is invoked when the checkbox is clicked.
        // This happens when the action key is pressed.
        std::function<void (void)> evtclick;

       ~checkbox() {}
        checkbox() : checked_(false), focus_(false) {}
        
        bool can_focus() const { return true; }
        void set_focus(bool f)
        {
            focus_ = f;
        }
        rect draw(buffer& fb)
        {
            const cell c = {0, ATTRIB_NONE, COLOR_NONE};
            const color col = focus_ ? COLOR_SELECTION : COLOR_NONE;

            formatter f(c, fb);
            f.move(xpos_, ypos_);
            f.print(text_.c_str(), text_.size());
            if (checked_)
            {
                const cell c = {0, (short)ATTRIB_NONE, (short)col};
                formatter f(c, fb);
                f.move(xpos_ + text_.size(), ypos_);
                f.print("[*]", 3);
            }
            else
            {
                const cell c = {0, (short)ATTRIB_NONE, (short)col};
                formatter f(c, fb);
                f.move(xpos_ + text_.size(), ypos_);
                f.print("[ ]", 3);
            }
            rect ret = {ypos_, xpos_, xpos_ + (int)text_.size() + 3, ypos_+1};
            return ret;
        }

        bool keydown(int key, int vk)
        {
            if (vk == VK_ACTION_SPACE || vk == VK_ACTION_ENTER)
            {
                checked_ = !checked_;
                valid_   = false;
                if (evtclick)
                    evtclick();
                return true;
            }
            return false;
        }
        
        int height() const
        {
            // checkbox is only one row high always.
            return 1;
        }

        int width() const
        {
            // checbox width is the text length + "[*]".
            return static_cast<int>(text_.size()) + 3;
        }

        void text(const std::string& txt)
        {
            if (txt != text_)
            {
                text_  = txt;
                valid_ = false;
            }
        }
        bool is_checked() const
        {
            return checked_;
        }
        void check(bool val)
        {
            if (val != checked_)
            {
                checked_ = val;
                valid_   = false;
            }
        }
        std::string text() const
        {
            return text_;
        }
    private:
        bool checked_;
        bool focus_;
        std::string text_;
    };
    

} // cli


