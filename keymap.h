// Copyright (c) 2014 Sami Väisänen, Ensisoft 
//
// http://www.ensisoft.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#pragma once

#include <initializer_list>
#include <chrono>
#include <string>
#include <map>
#include "keys.h"
#include "types.h"

namespace cli
{
    // map sequences of raw input keys to virtual keys
    // based on a key map configuration (i.e. "binding")
    class keymap
    {
    public:
        enum class modifier {
            ctrl, alt, shift
        };

        // modifiers + input key = vk
        struct binding {
            std::string name;
            cli::bitflag_t mods;
            cli::rawkey_t key;
            cli::vkey vk;
        };

        keymap(std::initializer_list<binding> bindings);
        keymap();        

       ~keymap();

        // try translate the raw input key to a virtual key.
        // returns the vkey that matched the input or vkey::none
        // if no virtual key was decoded.
        vkey translate(rawkey_t raw);

    private:
        typedef std::uint32_t keyvalue_t;
        typedef std::map<keyvalue_t, vkey> database;

        database keys_;
    };

} //  cli


