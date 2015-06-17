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

#include "menu.h"
#include "formatter.h"
#include "buffer.h"
#include <cassert>

using namespace std;

namespace 
{
    enum { MENUSPACING = 1 };
}

namespace cli
{

menu::menu() : isopen_(false), erase_(false), menclindex_(0), itemindex_(0), 
               width_(-1), height_(-1)
{
    memset(&eraserc_, 0, sizeof(rect));
}

bool menu::can_focus() const
{
    return false;
}

rect menu::draw(buffer& fb)
{
    assert(valid_ == false);

    cell sel = {' ', ATTRIB_NONE, COLOR_SELECTION};
    cell def = {' ', ATTRIB_NONE, COLOR_MENUITEM};
    cell sp  = {' ', ATTRIB_NONE, COLOR_NONE};
    formatter f(def, fb);
    f.fillblank(true);
    
    rect ret = {};
    
    int xpos = xpos_;

    memset(&eraserc_, 0, sizeof(rect));

    // draw the top level menu items
    for (menulist::size_type i(0); i<menus_.size(); ++i)
    {
        const submenu& sub = menus_[i];
        if (ret.bottom == 0) 
        {
            ret.top    = ypos_;
            ret.bottom = ypos_ + 1;
            ret.left   = xpos_;
        }
        f.setdef(def);
        if (isopen_ && menclindex_ == i)
            f.setdef(sel);

        f.move(xpos, ypos_);
        f.print(sub.text.c_str(), sub.text.size()); 
        // print a space after a menu item
        f.setdef(sp);
        f.move(xpos + sub.text.size(), ypos_);
        f.print("", 1);
        if (isopen_ && menclindex_ == i && !sub.items.empty())
        {
            // this drop down menu will be need to be erased when the
            // menu is closed. therefore save the dropdown menu rectangle for later
            assert(rect_is_empty(eraserc_));

            // find longest menu item string
            int len = 0;
            for (itemlist::size_type x(0); x<sub.items.size(); ++x)
                len = std::max<int>(len, sub.items[x].text.size());
            
            // draw items
            int ypos = ypos_ + 1;
            for (itemlist::size_type x(0); x<sub.items.size(); ++x, ++ypos)
            {
                const string& str = sub.items[x].text;
                f.move(xpos, ypos);
                f.setdef(def);
                if (itemindex_ == x)
                    f.setdef(sel);
                f.print(str.c_str(), len);
            }
            ret.bottom = ypos_ + ypos - 1;
            ret.right  = xpos + len; // xpos already includes left offset
            
            eraserc_.left   = xpos;
            eraserc_.top    = ypos_ + 1;
            eraserc_.right  = xpos + len;
            eraserc_.bottom = ret.bottom;
        }

        xpos += sub.text.size();
        if (i < menus_.size()-1)
            xpos += MENUSPACING;
    }
    // an open menu item may reach further than the menu item row itself
    // so only set if to the menu row width if this is not the case
    if (ret.right < xpos)
        ret.right = xpos; // xpos already includes left offset

    // save the calculated dimensions for later
    width_  = ret.right - ret.left;
    height_ = ret.bottom - ret.top;

    return ret;
}

rect menu::erase()
{
    // we only report a rectangle that needs to be erase
    // when a drop down menu has been closed or the focus has been moved. 
    // Otherwise we report an empty rect.
    if (erase_)
    {
        erase_ = false;
        return eraserc_;
    }
    rect rc = {};
    return rc;
}

bool menu::keydown(int raw, int vk)
{
    if (menus_.empty() || vk == -1)
        return false;

    assert(menclindex_ < menus_.size());
    
    const submenu& sub = menus_[menclindex_];

    switch (vk)
    {
        case VK_MOVE_UP:
            // move up on a submenu to prev menclitem
            if (isopen_ && !sub.items.empty())
            {
                for (itemlist::size_type i(0); i<sub.items.size(); ++i)
                {
                    if (itemindex_ > 0)
                        --itemindex_;
                    else
                        itemindex_ = sub.items.size()-1; // wrap over

                    if (sub.items[itemindex_].separator == false)
                        break;
                }
            }
            break;
        case VK_MOVE_DOWN:
            // move down on a submenu to next menclitem
            if (isopen_ && !sub.items.empty())
            {
                for (itemlist::size_type i(0); i<sub.items.size(); ++i)
                {
                    itemindex_ = (itemindex_ + 1) % sub.items.size();
                    if (sub.items[itemindex_].separator == false)
                        break;
                }
            }
            break;
        case VK_MOVE_NEXT:
            // move to next submenu
            menclindex_ = (menclindex_ + 1) % menus_.size();
            itemindex_ = 0;
            erase_     = true;
            break;
        case VK_MOVE_PREV:
            // move to prev submenu
            if (menclindex_ > 0)
                --menclindex_;
            else
                menclindex_ = menus_.size()-1; // wrap over
            itemindex_ = 0;
            erase_     = true;
            break;
        case VK_ACTION_SPACE:
        case VK_ACTION_ENTER:
            {
                // select a menu item, fire event and close menu
                itemlist::size_type index  = itemindex_;
                isopen_    = !isopen_;
                itemindex_ = 0;
                if (!isopen_)
                    erase_ = true;
                if (!isopen_ && evtmenu)
                {
                    assert(index < sub.items.size());
                    evtmenu(sub.items[index].id);
                }
            }
            break;
    }
    valid_ = false;
    return true;
}

int menu::width() const
{
    if (width_ == -1)
    {
        assert(isopen_ == false);
        // not a single call to to draw. width must be just the 
        // width of the top level menu items + cellspacings.
        int width = 0;
        for (menulist::size_type i(0); i<menus_.size(); ++i)
        {
            const submenu& sub = menus_[i];
            width += sub.text.size();
        }
        if (!menus_.empty())
            width += (menus_.size() - 1) * MENUSPACING;
        
        return width;
    }
    return width_;
}

int menu::height() const
{
    if (height_ == -1)
    {
        assert(isopen_ == false);
        // not yet a single call to draw, height must be just one. 
        return 1;
    }
    return height_;
}


void menu::setmenu(const menulist& list)
{
    valid_ = false;
    menus_ = list;
    isopen_ = false;
    itemindex_ = 0;
    menclindex_ = 0;
}

void menu::open(int submenu)
{
    assert(submenu >= 0);
    assert(submenu < static_cast<int>(menus_.size()));
    menclindex_ = submenu;
    itemindex_ = 0;
    isopen_    = true;
    valid_     = false;
}

void menu::close()
{
    assert(!rect_is_empty(eraserc_));
    isopen_ = false;
    valid_  = false;
    erase_  = true;
    menclindex_ = 0;
    itemindex_ = 0;
}

bool menu::is_open() const
{
    return isopen_;
}

bool menu::is_empty() const
{
    return menus_.empty();
}

} // cli

