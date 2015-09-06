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

#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "common.h"

namespace cli
{
    class cleartext
    {
    public:
       ~cleartext() {}
        cleartext() {}
        inline
        std::string screen(const std::string& input) const
        {
            return input;
        }
    };

    class password 
    {
    public:
        char PASSWORD_CHAR;
    protected:
       ~password() {}
        password() : PASSWORD_CHAR('*') {}
        inline
        std::string screen(const std::string& input) const
        {
            std::stringstream ss;
            ss << std::setw(input.size()) << std::setfill(PASSWORD_CHAR) << "";
            return ss.str();
        }
    };

    class alnum // alphanumeric input
    {
    protected:
       ~alnum() {}
        inline bool isgood(int key) const
        {
            if (key > 0xFF) return false;
            if (key < 0x20) return false;
            return true;
        }
    };

    typedef alnum allchars;
    
    class numeric // numeric input
    {
    protected:
       ~numeric() {}
        inline bool isgood(int key) const
        {
            // fix msvc assert issue
            if (key > 0xFF) return false;
            return ::isdigit(static_cast<unsigned char>(key)) != 0;
        }
    };

    class nochars // no input at all
    {
    protected:
       ~nochars() {}
        inline bool isgood(int) const { return false; }
    };


    class tabless
    {
    protected:
       ~tabless() {}
        inline std::string next() { return ""; }
        inline std::string prev() { return ""; }
        inline void update(const std::string&, char) {}
    };

    class allvk
    {
    protected:
       ~allvk() {}
        inline bool isgood(int) const { return true; }
    };

    class tabvk
    {
    protected:
       ~tabvk() {}
        inline bool isgood(int vk) const 
        {
            switch (vk)
            {
                case VK_TAB_COMPLETE_NEXT:
                case VK_TAB_COMPLETE_PREV:
                case VK_ACTION_ENTER:
                case VK_MOVE_DOWN:
                case VK_MOVE_UP:
                    return true;
            }
            return false;
        }
    };
    
} // cli


