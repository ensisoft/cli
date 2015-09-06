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
#include "inputpolicy.h"
#include "formatter.h"
#include <functional>
#include <string>
#include <cassert>

namespace cli
{
    // Basic_input is an input widget template. With different
    // Policies it can be configured to do different things.
    //
    // OutputMask - Output masking policy, which controls how input string gets printed. 
    // InputMask  - Input masking policy, which controls which input keys get accepted.
    // TabList    - Tab completion policy. 
    // VKMask     - Virtual Key masking policy.
    template <typename OutputMask = cleartext,
              typename InputMask  = alnum,
              typename TabList    = tabless,
              typename VKMask     = allvk>
    class basic_input : public widget, public OutputMask,
        public InputMask, public TabList, public VKMask
    {
    public:
        // This will get invoked when VK_ACTION_ENTER is received.
        std::function<void(void)> evtenter;

        // This will get invoked for any input keys.
        std::function<void(void)> evtkey;

        basic_input() : insertpos_(0), caretpos_(0), 
          width_(0), focus_(false) 
        {
        }

        bool can_focus() const
        {
            return width_ != 0;
        }

        void set_focus(bool focus)
        {
            focus_ = focus;
        }
        void set_cursor(cursor& c)
        {
            if (width_ == 0)
                return; 
            c.x = xpos_ + caretpos_;
            c.y = ypos_;
            c.v = true;
        }
        
        rect draw(buffer& fb)
        {
            const cell def = {' ', ATTRIB_NONE, COLOR_NONE};
            formatter f(def, fb);
            
            std::string input = OutputMask::screen(input_);

            const char* ptr = input.c_str();
            ptr += insertpos_;
            ptr -= caretpos_;

            int len = static_cast<int>(input.size());
            len -= insertpos_;
            len += caretpos_;
            f.move(xpos_, ypos_);
            f.print(ptr, len, width_);

            rect ret = {ypos_, xpos_, xpos_ + width_, ypos_+1};
            return ret;
        }

        bool keydown(int raw, int vk)
        {
            if (width_ == 0)
                return false;
            assert(focus_);
            if (vk != -1)
            {
                if (!VKMask::isgood(vk))
                    return false;
                if (vk == VK_ACTION_ENTER)
                {
                    if (evtenter)
                        evtenter();
                    return true;
                }
                bool handled = true;
                switch (vk)
                {
                    case VK_TAB_COMPLETE_NEXT: tab_complete(true);  break;
                    case VK_TAB_COMPLETE_PREV: tab_complete(false); break;
                    case VK_MOVE_UP:           
                        handled = false;
                        if (!InputMask::isgood(raw)) {
                            tab_complete(true);  
                            handled = true;
                        }
                        break;
                    case VK_MOVE_DOWN:         
                        handled = false;
                        if (!InputMask::isgood(raw))
                        {
                            tab_complete(false); 
                            handled = true;
                        }
                        break;

                    case VK_MOVE_HOME:
                        insertpos_ = 0;
                        caretpos_  = 0;
                        break;

                    case VK_MOVE_END:
                        insertpos_ = static_cast<int>(input_.size());
                        caretpos_  = static_cast<int>(input_.size()) % width_;
                        break;

                    case VK_KILL_LINE: kill_line();      break;
                    case VK_KILL_CHAR: kill_char(false); break;
                    case VK_ERASE:     kill_char(true);  break;

                    case VK_MOVE_NEXT:
                    {
                        int size = static_cast<int>(input_.size());
                        if (insertpos_ < size)
                            ++insertpos_;
                        int end = size % width_;
                        if (size > width_)
                            end = width_;
                        if (caretpos_ < end)
                            ++caretpos_;
                    }
                    break;
                    case VK_MOVE_PREV:
                        if (insertpos_ > 0) --insertpos_;
                        if (caretpos_ > 0)  --caretpos_;
                        break;
                    default:
                        handled = false;
                        break;
                }
                // there might be a widget virtual key mapped to some plain
                // ascii input. so if the virtual key is not processed
                // we interpret the input simply as raw input.
                if (handled)
                {
                    if (evtkey)
                        evtkey();
                    // input was handled as virtual key -> no further processing.
                    valid_ = false;
                    return true;
                }
            }
                
            // get rid of control chars
            if (raw < 0x20 || raw > 0xFF)
                return false;

            // do input screening
            if (!InputMask::isgood(raw))
                return false;

            // insert at the caret position
            std::string s1 = input_.substr(0, insertpos_);
            std::string s2 = input_.substr(insertpos_);

            input_  = s1;
            input_ += (char)raw;
            input_ += s2;

            ++insertpos_;
            if (caretpos_ < width_)
                ++caretpos_;
                
            TabList::update(input_, raw);

            if (evtkey)
                evtkey();

            valid_ = false;
            return true;
        }

        int height() const
        {
            // input field is only one row high always.
            return 1;
        }

        int width() const
        {
            return width_;
        }

        // Get input value string.
        std::string value() const
        {
            return input_;
        }
        
        // Set input value string. This will reset caret position to the 
        // start of the input string.
        void value(const std::string& val)
        {
            input_     = val;
            insertpos_ = 0;
            caretpos_  = 0;
            TabList::update(val, 0);
            valid_     = false;
        }

        // Clear input value string. This will reset caret position to the
        // start of the input string.
        void clear()
        {
            input_.clear();
            insertpos_ = 0;
            caretpos_  = 0;
            TabList::update("", 0);
            valid_     = false;
        }

        // Set input widget width.
        void width(int w)
        {
            width_ = w;
            valid_ = false;
        }
    private:
        void tab_complete(bool forward)
        {
            std::string n = forward ? TabList::next() : TabList::prev();
            if (n.empty())
                return;

            input_     = n;
            insertpos_ = static_cast<int>(input_.size());
            caretpos_  = static_cast<int>(input_.size());
            if (caretpos_ > width_)
                caretpos_ = width_;
        }

        void kill_char(bool move_caret)
        {
            std::string s1;
            std::string s2;

            if (move_caret)
            {
                if (insertpos_ == 0)
                    return;
        
                s1 = input_.substr(0, insertpos_-1);
                if (insertpos_ < static_cast<int>(input_.size()))
                    s2 = input_.substr(insertpos_);

                --insertpos_;
                if (caretpos_ > 0)
                    --caretpos_;
            }
            else
            {
                if (insertpos_ > static_cast<int>(input_.size()))
                    return;

                s1 = input_.substr(0, insertpos_);
                if (insertpos_ < static_cast<int>(input_.size()))
                    s2 = input_.substr(insertpos_+1);
            }
            
            input_  = s1;
            input_ += s2;
            
            TabList::update(input_, 0);
        }

        void kill_line()
        {
            // kill the rest of the line
            if (insertpos_ > static_cast<int>(input_.size()))
                return;

            input_ = input_.substr(0, insertpos_);

            TabList::update(input_, 0);
        }
        
        std::string input_;
        int insertpos_;
        int caretpos_;
        int width_;
        bool focus_;
    };

    typedef basic_input <> alphanumeric_input;
    typedef basic_input<cli::password> password_input;
    typedef basic_input<cli::cleartext, cli::numeric> numeric_input;
    

} // cli

