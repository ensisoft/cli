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
#include "formatter.h"
#include "common.h"
#include <string>
#include <functional>

namespace cli
{
    // Button represents a push button. 
    class button : public widget
    {
    public:
        // Evtclick will be invoked when the button is clicked. 
        // This happens when the action key is pressed. 
        std::function<void(void)> evtclick;

        button() : focus_(false) {}

        bool can_focus() const
        {
            return true;
        }
        void set_focus(bool focus) 
        {
            focus_ = focus;
        }
        bool keydown(int raw, int vk)
        {
            if ((vk == VK_ACTION_SPACE || vk == VK_ACTION_ENTER) && evtclick)
            {
                evtclick();
                return true;
            }
            return false;
        }
        rect draw(buffer& fb)
        {
            const color col = focus_ ? COLOR_SELECTION : COLOR_NONE;
            const cell c = {0, ATTRIB_NONE, (short)col};
            formatter f(c, fb);
            f.move(xpos_, ypos_);
            f.print(text_.c_str(), text_.size());
            rect ret = {ypos_, xpos_, xpos_ + (int)text_.size(), ypos_+1};
            return ret;
        }

        int height() const 
        {
            // button is only one row high always.
            return 1; 
        }

        int width() const
        {
            // button width comes directly from the the text length.
            return static_cast<int>(text_.size());
        }

        // Set button text.
        void text(const std::string& txt)
        {
            if (txt != text_)
            {
                text_= txt;
                valid_ = false;
            }
        }
        
        // Get button text.
        std::string text() const
        {
            return text_;
        }
    private:
        bool focus_;
        std::string text_;
    };

} // cli

