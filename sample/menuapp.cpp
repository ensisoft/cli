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


#include <boost/bind.hpp>
#include <cli/widgets.h>
#include <cli/terminal.h>
#include <cassert>

enum { 
    MENUID_FILE_NEW = 1,
    MENUID_FILE_OPEN,
    MENUID_FILE_SAVE,
    MENUID_FILE_SAVE_AS,
    MENUID_FILE_EXIT,
    MENUID_FILE_EDIT_CUT,
    MENUID_FILE_EDIT_COPY,
    MENUID_FILE_EDIT_PASTE,
    MENUID_HELP_HELP,
    MENUID_HELP_ENSISOFT
};

#define CASE(x) case x: return #x

const char* menustr(int menuid)
{
    switch (menuid)
    {
        CASE(MENUID_FILE_NEW);
        CASE(MENUID_FILE_OPEN);
        CASE(MENUID_FILE_SAVE);
        CASE(MENUID_FILE_SAVE_AS);
        CASE(MENUID_FILE_EXIT);
        CASE(MENUID_FILE_EDIT_CUT);
        CASE(MENUID_FILE_EDIT_COPY);
        CASE(MENUID_FILE_EDIT_PASTE);
        CASE(MENUID_HELP_HELP);
        CASE(MENUID_HELP_ENSISOFT);
        default: return "unknown menu id";
    }
    return NULL;
}

int map_input(int ch)
{
    switch (ch)
    {
        case cli::TERM_MOVE_DOWN:      return cli::VK_MOVE_DOWN;
        case cli::TERM_MOVE_UP:        return cli::VK_MOVE_UP;
        case cli::TERM_MOVE_HOME:      return cli::VK_MOVE_HOME;
        case cli::TERM_MOVE_END:       return cli::VK_MOVE_END;
        case cli::TERM_MOVE_DOWN_PAGE: return cli::VK_MOVE_DOWN_PAGE;
        case cli::TERM_MOVE_UP_PAGE:   return cli::VK_MOVE_UP_PAGE;
        case cli::TERM_MOVE_PREV:      return cli::VK_MOVE_PREV;
        case cli::TERM_MOVE_NEXT:      return cli::VK_MOVE_NEXT;
        case 'm':                 return cli::VK_OPEN_MENU;
        case 'n':                 return cli::VK_MOVE_DOWN;
        case 'p':                 return cli::VK_MOVE_UP;
        case 'a':                 return cli::VK_MOVE_PREV;
        case 'd':                 return cli::VK_MOVE_NEXT;
        case ' ':                 return cli::VK_ACTION_SPACE;
        case '\n':                return cli::VK_ACTION_ENTER;
        case '\t':                return cli::VK_FOCUS_NEXT;
        case 'q':                 return cli::VK_KILL_WINDOW;
    }
    return -1;
}

const char* strings[] = 
{
    "To navigate the menu:",
    "  press 'a' for moving left",
    "  press 'd' for moving right",
    "  press 'n' for moving down",
    "  press 'p' for moving up",
    "  press 'm' for menu",
    "  press space to select",
    "",
    "To change focus press 'tab'"
};

class const_text_data
{
public:
protected:
   ~const_text_data() {}
    
    typedef const char* value;
    
    class converter {
    public:
        converter(const value& val) : str_(val) {}

        inline
        const char* str() const
        {
            return str_;
        }
        inline
        size_t len() const
        {
            return strlen(str_);
        }
    private:
        const char* str_;
    };

    void fetch(value& val, int index) const
    {
        val = strings[index];
    }
    
    int size() const
    {
        return sizeof(strings)/sizeof(const char*);
    }
};

void menu_event(int id, cli::text* txt)
{
    txt->settext(menustr(id));
}

void draw_buffer(cli::window* win, cli::buffer* fb)
{
    const cli::rect dirty = win->draw(*fb);

    cli::term_draw_buffer(*fb, dirty);
}

void clear_buffer(cli::window* win, cli::rect rc, cli::buffer* fb)
{
    fb->clear(rc);
}

void set_cursor(cli::window* win, cli::cursor c)
{
    cli::term_show_cursor(c);
}


int main(int argc, const char* argv[])
{
    cli::term_init();
    cli::term_init_colors();

    cli::size size = cli::term_get_size();
    assert(size.cols && size.rows);

    cli::buffer framebuff;
    framebuff.resize(size.rows, size.cols);
    
    cli::window wnd;
    wnd.evtdraw   = boost::bind(draw_buffer, _1, &framebuff);
    wnd.evterase  = boost::bind(clear_buffer, _1, _2, &framebuff);    
    wnd.evtcursor = boost::bind(set_cursor, _1, _2);

    // show some header data
    cli::text text1;
    text1.position(0, 0);
    text1.width(size.cols);
    text1.settext("press 'q' to exit sample");
    text1.setattrib(cli::ATTRIB_UNDERLINE | cli::ATTRIB_BOLD);
    
    // create menu objects and items
    cli::menulist list;
    list.resize(3);
    list[0].text = "File";
    list[0].items.push_back(cli::make_menu_item("New", MENUID_FILE_NEW));
    list[0].items.push_back(cli::make_menu_item("Open", MENUID_FILE_OPEN));
    list[0].items.push_back(cli::make_menu_item("Save", MENUID_FILE_SAVE));
    list[0].items.push_back(cli::make_menu_item("Save As...", MENUID_FILE_SAVE_AS));
    list[0].items.push_back(cli::make_menu_item("Exit", MENUID_FILE_EXIT));
    list[1].text = "Edit";
    list[1].items.push_back(cli::make_menu_item("Cut", MENUID_FILE_EDIT_CUT));
    list[1].items.push_back(cli::make_menu_item("Copy", MENUID_FILE_EDIT_COPY));
    list[1].items.push_back(cli::make_menu_item("Paste", MENUID_FILE_EDIT_PASTE));
    list[2].text = "Help";
    list[2].items.push_back(cli::make_menu_item("Help", MENUID_HELP_HELP));
    list[2].items.push_back(cli::make_menu_item("www.ensisoft.com", MENUID_HELP_ENSISOFT));

    cli::text text2;
    text2.position(0, 13);
    text2.width(size.cols);
    text2.settext("text2");

    cli::menu menu1;
    menu1.setmenu(list);
    menu1.position(0, 1);
    menu1.evtmenu = boost::bind(menu_event, _1, &text2);

    cli::basic_view<const_text_data> view1;
    view1.position(0, 3);
    view1.height(9);
    view1.width(size.cols);
    
    wnd.add(&text1);
    wnd.add(&menu1);
    wnd.add(&view1);
    wnd.add(&text2);
    wnd.show();
    wnd.invalidate();
    wnd.can_close_on_vk(true);
    
    while (wnd.is_open())
    {
        int ch = cli::term_get_key();
        int vk = map_input(ch);
        wnd.keydown(ch, vk);
    }
    cli::term_uninit();
    return 0;
}
