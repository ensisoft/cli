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

#include "types.h"

namespace cli
{
    // Supported widget colors. The actual implementation of any specific
    // color is terminal implementation specific.
    enum class color {
        none,           // no specific color. terminal default colors are used.
        highlight,      // Generic highlight color.
        selection,      // Active selection (has focus) color
        inactive,       // Inactive (not focused) selection color.
        text,           // Static object, such as text color
        menuitem,       // menu item color
        attention,      // Attention
        question,       // Question
        information,    // Information
    };

    // Supported video text attributes. Values can be OR'red together.
    // The actual implementation of any attribute is terminal
    // implementation specific.
    enum class attrib {
        none          = 0x0,    // normal text
        reverse_video = 0x1,    // background and text color is reversed
        underline     = 0x2,    // underline text
        highlight     = 0x4,    // highlighted tex
        blink         = 0x8,    // blinking text
        dim           = 0x10,   // dimmed text
        bold          = 0x20    // bold text
    };

    inline
    attrib operator | (attrib lhs, attrib rhs)
    {
        return static_cast<attrib>(bitflag_t(lhs) | bitflag_t(rhs));
    }

    inline
    attrib operator & (attrib lhs, attrib rhs)
    {
        return static_cast<attrib>(bitflag_t(lhs) & bitflag_t(rhs));
    }

    inline
    bool is_set(attrib attr, attrib flag)
    {
        return (attr & flag) == flag;
    }

} // cli