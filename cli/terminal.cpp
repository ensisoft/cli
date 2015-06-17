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


#include "terminal.h"
#ifdef _WIN32
#  define _WIN32_WINNT 0x0500
#  include <windows.h>
#  include <conio.h>
#  include <vector>
#  pragma comment(lib, "user32.lib")
#else
#  include <curses.h>
#endif

#include <cassert>

#include "window.h"
#include "buffer.h"

namespace cli
{

#if defined(_WIN32)

WORD map_color(short col)
{
    switch (col)
    {
        case COLOR_NONE:      return 0;
        case COLOR_SELECTION: return BACKGROUND_GREEN;
        case COLOR_INACTIVE:  return FOREGROUND_GREEN;
        case COLOR_MENUITEM:  return BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE; // white
        case COLOR_HIGHLIGHT: return BACKGROUND_RED | FOREGROUND_RED; 
        default: assert(!"todo"); return 0;
    }
}

WORD map_attrib(short attr)
{
    if (attr & ATTRIB_BOLD)
        return FOREGROUND_INTENSITY;
    return 0;
}


void term_init()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE in  = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(in, &mode);
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    BOOL ret = SetConsoleMode(in, mode);
    assert( ret == TRUE );
    
    // hide the cursor for now.
    CONSOLE_CURSOR_INFO cci = {};
    GetConsoleCursorInfo(out, &cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(out, &cci);
}

void term_init_colors()
{}

void term_uninit()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE in  = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(in, &mode);
    mode |= ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT;
    BOOL ret = SetConsoleMode(in, mode);
    assert( ret == TRUE );
    
    CONSOLE_CURSOR_INFO cci = {};
    GetConsoleCursorInfo(out, &cci);
    cci.bVisible = TRUE;
    SetConsoleCursorInfo(out, &cci);
}

size term_get_size()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFO info = {};
    GetCurrentConsoleFont(out, FALSE, &info);
    COORD coord = GetConsoleFontSize(out, info.nFont);
    HWND hwnd   = GetConsoleWindow();
    assert(hwnd && "full screen not supported");
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    size ret;
    ret.rows = rc.bottom / coord.Y;
    ret.cols = rc.right  / coord.X;
    return ret;
}

int term_get_key()
{
    int ch = _getch();
    if (ch == 0x0 || ch == 0xE0)
    {
        // from msdn:
        // when reading a function key or an arrow key
        // this function has to be called twice, first call returns 0x00 or 0xE0
        // and the second call returns they keycode, which i guess is VK_ something
        _getch();
        if (GetAsyncKeyState(VK_DOWN))  return TERM_MOVE_DOWN;
        if (GetAsyncKeyState(VK_UP))    return TERM_MOVE_UP;
        if (GetAsyncKeyState(VK_HOME))  return TERM_MOVE_HOME;
        if (GetAsyncKeyState(VK_END))   return TERM_MOVE_END;
        if (GetAsyncKeyState(VK_NEXT))  return TERM_MOVE_DOWN_PAGE;
        if (GetAsyncKeyState(VK_PRIOR)) return TERM_MOVE_UP_PAGE;
        if (GetAsyncKeyState(VK_LEFT))  return TERM_MOVE_PREV;
        if (GetAsyncKeyState(VK_RIGHT)) return TERM_MOVE_NEXT;
   } 
   return ch;
}

void term_show_cursor(const cursor& curs)
{
    // todo:
}

void term_draw_buffer(const buffer& buff, const rect& src)
{
    typedef buffer::row_type row;    

    size_t col_count = buff.cols();
    size_t row_count = buff.rows();

    static std::vector<CHAR_INFO> buffer;
    static WORD attrib = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; // force white text on black as a default

    if (buffer.size() != col_count * row_count)
        buffer.resize(col_count * row_count);

    for (size_t i=0; i<row_count; ++i)
    {
        const row& r = buff[i];
        for (size_t x=0; x<col_count; ++x)
        {
            const cell& c = r[x];
            if (c.value == 0)
                continue;
            CHAR_INFO& cc = buffer[i * col_count + x];
            cc.Char.AsciiChar = c.value;
            cc.Attributes     = 0;
            if (c.color == COLOR_NONE)
                cc.Attributes = attrib;
            else
                cc.Attributes |= map_color(c.color);

            if (c.attrib != ATTRIB_NONE) 
                cc.Attributes |= map_attrib(c.attrib);
        }
    }
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD buffersize  = { col_count, row_count };
    COORD buffercoord = { rc.left, rc.top };
    SMALL_RECT rect   = { rc.left, rc.top, rc.right-1, rc.bottom-1};

    BOOL ret = WriteConsoleOutput(out, &buffer[0], buffersize, buffercoord, &rect);
    assert( ret == TRUE );
    ret = 0;    
}

#else

void term_init()
{
    initscr();
    start_color();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    raw();
    curs_set(0);
}

void term_init_colors()
{
    // id, foreground, background
    init_pair(COLOR_SELECTION, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_MENUITEM,  COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_INACTIVE,  COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_HIGHLIGHT, COLOR_RED,   COLOR_RED);
}

void term_uninit()
{
    endwin();
}

size term_get_size()
{
    size ret;
    getmaxyx(stdscr, ret.rows, ret.cols);
    return ret;
}

int term_get_key()
{
    int ch = getch();
    switch (ch)
    {
        case KEY_DOWN:  return TERM_MOVE_DOWN;  
        case KEY_UP:    return TERM_MOVE_UP;
        case KEY_HOME:  return TERM_MOVE_HOME;
        case KEY_END:   return TERM_MOVE_END;
        case KEY_NPAGE: return TERM_MOVE_DOWN_PAGE;
        case KEY_PPAGE: return TERM_MOVE_UP_PAGE;
        case KEY_LEFT:  return TERM_MOVE_PREV;
        case KEY_RIGHT: return TERM_MOVE_NEXT;
    }   
    return ch; 
}

void term_show_cursor(const cursor& curs)
{
    move(curs.y, curs.x);
    if (curs.v)
        curs_set(1);
    else
        curs_set(0);
    refresh();        
}

void term_draw_buffer(const buffer& buff, const rect& src)
{
    typedef buffer::row_type row;

    // transfer the frame buffer contents into the terminal
    // using ncurses as the "rendering" back end.
    int lower_bound = src.top;
    int upper_bound = src.bottom;
    for (; lower_bound < upper_bound; ++lower_bound)
    {
        const row& r = buff[lower_bound];

        for (size_t i=0; i<r.size(); ++i)
        {
            const cell& c = r[i];
            if (c.value == 0)
                continue;
            
            if (c.color != COLOR_NONE)  attron(COLOR_PAIR(c.color));
            if (c.attrib != ATTRIB_NONE)
            {
                if (c.attrib & ATTRIB_UNDERLINE) attron(A_UNDERLINE);
                if (c.attrib & ATTRIB_BOLD)      attron(A_BOLD);
                if (c.attrib & ATTRIB_STANDOUT)  attron(A_STANDOUT);
                if (c.attrib & ATTRIB_DIM)       attron(A_DIM);
            }
            int y = lower_bound;
            int x = i;
            move(y, x);
            printw("%c", c.value);
            if (c.color != COLOR_NONE) attroff(COLOR_PAIR(c.color));
            if (c.attrib != ATTRIB_NONE)
            {
                if (c.attrib & ATTRIB_UNDERLINE) attroff(A_UNDERLINE);
                if (c.attrib & ATTRIB_BOLD)      attroff(A_BOLD);
                if (c.attrib & ATTRIB_STANDOUT)  attroff(A_STANDOUT);
                if (c.attrib & ATTRIB_DIM)       attroff(A_DIM);
            }
        }
    }
    refresh();
}

#endif

} // cli
