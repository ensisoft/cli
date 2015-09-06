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

#include "common.h"
#include "formatter.h"
#include "buffer.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

namespace cli
{
    // Implements vertical text scrolling from right to left.
    class right_to_left_ticker
    {
    public:
        void enable_scrolling(bool val)
        {
            enabled_ = val;
        }
        void set_animation_treshold(int millis)
        {
            idle_treshold_ = millis;
        }
    protected:
       ~right_to_left_ticker() {}
        right_to_left_ticker() : enabled_(true),idle_treshold_(2000),
                   pivot_(0), wait_(0), width_(0) {}

        void scroll(buffer& fb, int xpos, int ypos)
        {
            if (!enabled_) return;

            assert(!line_.empty());
            const char* ptr = line_.c_str();
            int charc = 0;
            pivot_ = pivot_ % line_.length();
            charc  = std::min<int>(line_.length() - pivot_, width_);
            
            cell def = {0, ATTRIB_NONE, COLOR_SELECTION};
            formatter f(def, fb);
            f.move(xpos, ypos);
            f.print(ptr + pivot_, charc);
            f.move(xpos + charc, ypos);
            f.print(ptr, std::min<int>(pivot_, width_ - charc));
            ++pivot_;
        }
        void reset()
        {
            pivot_ = 0;
            wait_  = 0;
            line_.clear();
        }
        void set(const std::string& line, int width)
        {
            line_ = line;
            if (static_cast<int>(line.length()) < width)
            {
                // simply expand the string to meet the full space
                std::stringstream ss;
                ss << line;
                ss << std::setw(width - line.length()) << std::setfill(' ') << "";
                line_ = ss.str();
            }
            width_ = width;
        }
        bool is_set() const
        {
            return !line_.empty();
        }
        bool is_idle(int elapsed)
        {
            if (!enabled_) return true;
            if (wait_ < idle_treshold_)
            {
                // do not start to scroll immediately but wait for the idle treshold
                // to expire before scrolling
                wait_ += elapsed;
                return true;
            }
            return false;
        }
    private:
        bool enabled_;
        int  idle_treshold_;
        int  pivot_;
        int  wait_;
        int  width_;
        std::string line_;
    };


    class default_ticker
    {
    protected:
       ~default_ticker() {}

        inline void scroll(buffer&, int, int) {}
        inline void reset() {}
        inline void set(const std::string&, int) {}
        inline bool is_set() const { return true; }
        inline bool is_idle(int ) const { return true; }
    private:
    };


} // cli

