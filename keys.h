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

#include "config.h"

#include <string>
#include "types.h"

namespace cli
{

    // possible modifiers, used as a bitwise OR flag
    enum class keymod
     {
        none    = 0x0,
        shift   = 0x1,
        control = 0x2,
        alt     = 0x4
    };


    inline
    keymod operator | (keymod lhs, keymod rhs)
    {
        return static_cast<keymod>(bitflag_t(lhs) | bitflag_t(rhs));
    }

    inline
    keymod operator & (keymod lhs, keymod rhs)
    {
        return static_cast<keymod>(bitflag_t(lhs) & bitflag_t(rhs));
    }

    inline
    bool is_set(keymod mod, keymod flag)
    {
        return (mod & flag) == flag;
    }

    // primitive key symbols. this list represents a physical keys available
    // on most keyboards. the meaning of each raw key depends on the application.
    // key translation to characters is keyboard layout specific. Some keyboard
    // layouts have different mappings for different characters. For example with US 
    // layout double quote (") is a single key and to generate the same character with 
    // FI layout user needs to press Shift+2.
    // therefore this list does not contain any keys that are layout specific but maps
    // directly to physical keys on the keyboard
    enum class keysym 
    {
        none,                   // no known key
        backspace,
        tab,
        enter,
        space,
        key_0, 
        key_1, 
        key_2, 
        key_3, 
        key_4, 
        key_5, 
        key_6,
        key_7, 
        key_8, 
        key_9,
        key_A, 
        key_B, 
        key_C, 
        key_D, 
        key_E, 
        key_F, 
        key_G, 
        key_H, 
        key_I, 
        key_J, 
        key_K, 
        key_L, 
        key_M, 
        key_N, 
        key_O, 
        key_P, 
        key_Q, 
        key_R, 
        key_S, 
        key_T, 
        key_U, 
        key_V, 
        key_W, 
        key_X, 
        key_Y,
        key_Z,
        f1, 
        f2, 
        f3, 
        f4, 
        f5, 
        f6, 
        f7, 
        f8, 
        f9,
        f10, 
        f11, 
        f12,
        control_R,
        control_L,
        shift_R,
        shift_L,
        alt_L,
        capslock,
        insert,
        del,
        home,
        end,
        pageup,
        pagedown,
        left,
        up,
        down,
        right,
        escape
    };     


    // virtual keys for widget/window actions.
    enum class vkey
    {
        none,
        // cursor control
        move_up,
        move_down,
        move_page_up,
        move_page_down,
        move_home,
        move_end,
        move_next,
        move_prev,

        // larger actions
        action_space,
        action_enter,
        action_erase,
        action_kill_line,
        action_kill_char,
        action_kill_window,

        // window/widget selection
        focus_next,
        focus_prev,
        open_menu,

        // other stuff
        tab_complete_next,
        tab_complete_prev,
        toggle_mark,        

        // marks the end of the cli virtual key space
        // application key space can begin at sentinel + 1
        sentinel
    };

    std::string mod_name(keymod mod);
    std::string sym_name(keysym sym);
    std::string key_name(vkey key);

} // cli