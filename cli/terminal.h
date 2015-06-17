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

namespace cli
{

class window;
class buffer;

// the widget library is completely abstracted away from
// any concrete terminal implementations and platforms. 
// Therefor some of the lifting has to be done by the 
// host application. Naturally the app can use some *other* 
// lib for this stuff.
// terminal.cpp is a simple "layer" that provides a simplified
// API on top of different native terminal APIs. 
// It currently supports just Windows console API and *nix ncurses.

enum term_funtion_keys 
{
    TERM_MOVE_DOWN = 0x100,
    TERM_MOVE_UP,
    TERM_MOVE_HOME,
    TERM_MOVE_END,
    TERM_MOVE_DOWN_PAGE,
    TERM_MOVE_UP_PAGE,
    TERM_MOVE_NEXT,
    TERM_MOVE_PREV
};

// Initialize the terminal for use. 
void term_init();

// Initialize UI color fields for use at later stage.
void term_init_colors();

// Clean up and unitialize terminal.
void term_uninit();

// Get current terminal window size in terms of rows and columns
size term_get_size();

// Transfer the contents of the given frame buffer to
// the "physical" terminal window.
void term_draw_buffer(const buffer& buff, const rect& src);

// Read next input key from the input queue. Will block untill
// a key is available.
int  term_get_key();

// Show or hide cursor depending the cursor state.
void term_show_cursor(const cursor& curs);

} // cli