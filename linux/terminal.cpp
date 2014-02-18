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

#include "../config.h"

#ifndef LINUX_OS
#  error this implementation is only for linux-ncurses
#endif

#include <algorithm>
#include <curses.h>
#include "../terminal.h"
#include "../framebuf.h"


namespace cli
{

struct terminal::impl {
};

terminal::terminal()
{
    struct ncurses {
        ncurses()
        {
            initscr();

            // initialize ncurses
            initscr();

            // initialize colors
            start_color();

            // enable the keypad for function keys such as KEY_LEFT
            keypad(stdscr, TRUE);

            noecho();

            // disable line buffering and erase/kill processing
            //cbreak();

            // raw is similar to cbreak except that interrupt, suspend, quit and flow control
            // characters do not generate an interrupt
            raw();

            // hide the cursor
            curs_set(0);

            // initialize some colors
            init_pair((short)color::none, COLOR_BLACK, COLOR_BLACK);
            init_pair((short)color::text, COLOR_WHITE, COLOR_BLACK);
            init_pair((short)color::inactive, COLOR_GREEN, COLOR_BLACK);
        }

       ~ncurses()
        {
            endwin();            
        }
    };
    static ncurses init;
}

terminal::~terminal()
{}

void terminal::draw(const framebuf& fb)
{
    // this is a simple clipping draw that starts at 0,0 in the terminal
    // and extends to min(fb-width, terminal-width), min(fb-height, terminal-height)

    const auto WIDTH  = std::min(fb.width(), width());
    const auto HEIGHT = std::min(fb.height(), height());

    for (auto y=decltype(HEIGHT){0}; y != HEIGHT; ++y)
    {
        const auto& row = fb[y];
        for (auto x=decltype(WIDTH){0}; x != WIDTH; ++x)
        {
            const auto& c = row[x];

            switch (c.color)
            {
                case color::none:
                case color::text:
                case color::inactive:
                    attron(COLOR_PAIR((short)c.color));
                    break;
                default:
                break;
            }

            if (c.attrs != attrib::none)
            {
                if (is_set(c.attrs, attrib::underline)) attron(A_UNDERLINE);
                if (is_set(c.attrs, attrib::bold))      attron(A_BOLD);
                if (is_set(c.attrs, attrib::highlight)) attron(A_STANDOUT);
                if (is_set(c.attrs, attrib::dim))       attron(A_DIM);
            }

            move(y, x);
            printw("%c", c.value);

            if (c.attrs != attrib::none)
            {
                if (is_set(c.attrs, attrib::underline)) attroff(A_UNDERLINE);
                if (is_set(c.attrs, attrib::bold))      attroff(A_BOLD);
                if (is_set(c.attrs, attrib::highlight)) attroff(A_STANDOUT);
                if (is_set(c.attrs, attrib::dim))       attroff(A_DIM);
            }

            switch (c.color)
            {
                case color::none:
                case color::text:
                case color::inactive:
                    attroff(COLOR_PAIR((short)c.color));
                    break;
                default:
                break;
            }
        }
    }

    refresh();
}

std::size_t terminal::width() const
{
    int ret, dummy;
    getmaxyx(stdscr, dummy, ret);
    return std::size_t(ret);
}

std::size_t terminal::height() const
{
    int ret, dummy;
    getmaxyx(stdscr, ret, dummy);
    return std::size_t(ret);
}

} // cli
