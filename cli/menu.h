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

#include "formatter.h"
#include "widget.h"
#include "common.h"
#include <vector>
#include <string>

namespace cli
{
    struct menu_item {
        std::string text;
        int  id;
        bool separator;
    };

    inline
    menu_item make_menu_item(const std::string& text, int id=0, bool sep=false)
    {
        menu_item m = {text, id, sep};
        return m;
    }
    
    
    struct submenu {
        std::string text;
        std::vector<menu_item> items;
    };

    typedef std::vector<submenu> menulist;
    typedef std::vector<std::string> itemlist;

    class menu : public widget
    {
    public:
        menu();

        // This event is fired when a menu item is selected from
        // the menu. The event parameter is the id of the menu 
        // item in question.
        event1 evtmenu;

        
        // Focus stuff.
        bool can_focus() const;
        
        // Draw this menu to the frame buffer
        rect draw(buffer& fb);

        // Return a rectangle describing the area that should
        // be erase. This is an empty rectangle, except when a menu
        // item has been closed.
        rect erase();

        // Process keydown event.
        bool keydown(int raw, int vk);

        int height() const;   
        
        // Get menu width. Menu width is the width 
        // of all of its submenu items (text length) + menu spacing.
        int width() const;

        // Create a new menu from the menulist.
        // This will create a copy of the passed list internally.
        void setmenu(const menulist& list);

        // Open the menu at the specified submenu.
        void open(int submenu);
        
        // Close the menu
        void close();

        bool is_open() const;
        bool is_empty() const;

    private:
        bool isopen_;                    // flag telling if there is a menu open or not
        bool erase_;                     // flag telling if we have something to erase
        menulist menus_;                 // the list of menus      
        menulist::size_type menclindex_;  // selected top level menu item
        itemlist::size_type itemindex_;  // selected sub menu item
        rect eraserc_;                   // the rectangle that needs erasing
        int  width_;                     // width of the current menu rectangle (open or closed)
        int  height_;                    // height of the current menu rectangle (open or closed)
    };
    

} // cli

