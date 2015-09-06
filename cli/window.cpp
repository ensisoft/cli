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

#include "config.h"

#include "window.h"
#include "widget.h"
#include "buffer.h"
#include "menu.h"
#include <algorithm>
#include <vector>
#include <cassert>

namespace cli
{

window::window() : 
    focused_(NULL),
    menu_(NULL),
    can_close_(false), 
    is_valid_(false), 
    is_open_(false)
{
    cursor_.x = 0;
    cursor_.y = 0;
    cursor_.v = false;
    memset(&rc_erase_, 0, sizeof(rc_erase_));
}

window::~window()
{
}

void window::add(widget* w)
{
    circus_.push_back(w);
    if (is_open_)
    {
        w->invalidate(true);
        is_valid_ = false;
        if (evtdraw)
            evtdraw(this);
    }
}

void window::add(menu* m)
{
    if (menu_ != NULL)
    {
        rem(menu_);
        menu_ = NULL;
    }
    menu_ = m;
    if (m)
    {
        circus_.push_back(m);
        if (is_open_)
        {
            menu_->invalidate(true);
            is_valid_ = false;
            if (evtdraw)
                evtdraw(this);
        }
    }
}

void window::rem(widget* w)
{
    circus_.erase(remove(circus_.begin(), circus_.end(), w), circus_.end());
    if (is_open_ && evterase)
    {
        if (focused_ == w)
        {
            // todo: find next focused widget
        }
        rect rc   = {};
        rc.top    = w->ypos();
        rc.left   = w->xpos();
        rc.right  = rc.left + w->width();
        rc.bottom = rc.top + w->height();

        // combine this invalid rectangle with already existing rectangle
        rc_erase_ = rect_union(rc_erase_, rc);
        if (evtdraw)
            evtdraw(this);
    }
    if (focused_ == w)
        focused_ = NULL;
    if (menu_ == w)
        menu_ = NULL;
}

bool window::focus(widget* w)
{
    // no update to the focus
    if (w == focused_ || !w->can_focus())
        return false;

    assert(std::find(circus_.begin(), circus_.end(), w) != circus_.end());

    cursor_.v = false;

    if (focused_)
    {
        focused_->set_focus(false);
        focused_->invalidate(false);
    }
    focused_ = w;
    focused_->set_focus(true);
    focused_->set_cursor(cursor_);
    focused_->invalidate(false);

    is_valid_ = false;
    if (evtfocus)  evtfocus(this);
    if (evtdraw)   evtdraw(this);
    if (evtcursor) evtcursor(this, cursor_);
    
    return true;
}

widget* window::focused()
{
    return focused_;
}

const widget* window::focused() const
{
    return focused_;
}

void window::update(widget* w)
{
    assert(std::find(circus_.begin(), circus_.end(), w) != circus_.end());
    w->invalidate(true);
    is_valid_ = false;
    if (evtdraw)
        evtdraw(this);
    if (focused_ == w)
    {
        cursor_.v = false;
        w->set_cursor(cursor_);
        if (evtcursor)
            evtcursor(this,cursor_);
    }
}

void window::move(widget* w, int xpos, int ypos)
{
    assert(std::find(circus_.begin(), circus_.end(), w) != circus_.end());
    
    w->position(xpos, ypos);
    for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
        (*it)->invalidate(true);

    is_valid_ = false;
    if (evtdraw)
        evtdraw(this);
}

const cursor& window::curs() const
{
    return cursor_;
}

rect window::draw(buffer& fb) 
{
    rect erase = {};
    if (evterase)
    {
        for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
        {
            widget* w = *it;
            rect r = w->erase();
            if (!rect_is_empty(r))
                erase = rect_union(erase, r);
        }
        erase = rect_union(rc_erase_, erase);
        evterase(this, erase);
    }
    
    // todo: should the validation be bclilt in the widgets as well?
    // seeing that the invalidation is.
    rect rc = {};
    for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
    {
        widget* w = *it;
        if (!rect_is_empty(erase))
        {
            // if this widgets rectangle falls within the erased rectangle
            // we have a need to believe that it needs to be redrawn
            rect r   = {};
            r.top    = w->ypos();
            r.left   = w->xpos();
            r.right  = r.left + w->width();
            r.bottom = r.top  + w->height();
            if (rect_intersects_rect(r, erase))
                w->invalidate(true);
        }

        if (w->is_valid() || w == focused_ || (w == menu_ && menu_->is_open()))
            continue;

        rect r = w->draw(fb);
        w->validate();
        rc = rect_union(rc, r);
    }
    // draw the focused widget last. This allows to do simple things
    // like have a menu open on top of other widgets. (or a dropdown list, etc)
    // todo: should there be z ordering?
    if (focused_)
    {
        // if the focused widget is not valid or then some widget drew into
        // a rectangle that intersects with the rectangle of the focused
        // widget it needs to be redrawn.
        // so calculate the focused rect here.
        if (focused_->is_valid())
        {
            rect frc   = {};
            frc.top    = focused_->ypos();
            frc.left   = focused_->xpos();
            frc.right  = frc.left + focused_->width();
            frc.bottom = frc.top  + focused_->height();
            if (rect_intersects_rect(frc, rc))
                focused_->invalidate(true);
        }
        cursor_.v = false;
        if (!focused_->is_valid())
        {
            rect r = focused_->draw(fb);
            focused_->validate();
            rc = rect_union(rc, r);
        }
        focused_->set_cursor(cursor_);
    }

    if (menu_ && menu_->is_open())
    {
        if (menu_->is_valid())
        {
            rect mrc   = {};
            mrc.top    = menu_->ypos();
            mrc.left   = menu_->xpos();
            mrc.right  = mrc.left + menu_->width();
            mrc.bottom = mrc.top  + menu_->height();
            if (rect_intersects_rect(mrc, rc))
                menu_->invalidate(true);
        }
        if (!menu_->is_valid())
        {
            rect r = menu_->draw(fb);
            menu_->validate();
            rc = rect_union(rc, r);
            if (cursor_.v)
            {
                // need to hide cursor if it happens to intersect with the  drop down menu
                if (cursor_.x >= rc.left && cursor_.x <= rc.right)
                    if (cursor_.y >= rc.top && cursor_.y <= rc.bottom)
                        cursor_.v = false;
            }
        }
    }

    is_valid_ = true;
    memset(&rc_erase_, 0, sizeof(rect));
    
    return rect_union(rc, erase);

}

rect window::animate(buffer& fb, int elapsed)
{
    rect ret = {};
    for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
    {
        widget* w = *it;
        if (w == focused_ || w == menu_)
            continue;

        rect rc = w->animate(fb, elapsed);
        if (!rect_is_empty(rc))
            ret = rect_union(ret, rc);
    }
    widget* special[2] = {focused_, menu_};
    for (int i=0; i<2; ++i)
    {
        widget* wid = special[i];
        if (!wid)
            continue;
        rect r = wid->animate(fb, elapsed);
        if (rect_is_empty(r))
        {
            // the widget didnt do any animations. in other words it didnt update
            // its rectangle in any way. Thus we must check if some other widget animated
            // into this rectangle. 
            r.top    = wid->ypos();
            r.left   = wid->xpos();
            r.right  = r.left + wid->width();
            r.bottom = r.top  + wid->height();
            if (rect_intersects_rect(r, ret))
            {
                // yes, animation messed up, need to redraw.
                wid->invalidate(true);
                r = wid->draw(fb);
                ret = rect_union(ret, r);
            }
        }
        else
            ret = rect_union(ret, r);
    }
    return ret;
}


void window::invalidate()
{
    for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
        (*it)->invalidate(true);

    is_valid_ = false;
    if (evtdraw)
        evtdraw(this);
}



bool window::keydown(int raw, int vk)
{
    if (vk == VK_KILL_WINDOW && menu_ && menu_->is_open())
    {
        menu_->close();
    }
    else if (vk == VK_KILL_WINDOW && can_close_)
    {
        is_open_ = false;
        return true;
    }
    else if ((vk == VK_FOCUS_NEXT || vk == VK_FOCUS_PREV) && !(menu_ && menu_->is_open()))
    {
        // move focus to another focusable widget.
        // first find the position where the currently focused 
        // widget is, and then move to the next or previous widget 
        // relative to the focused widget.
        std::vector<widget*>::size_type pos = 0;
        if (focused_)
        {
            std::vector<widget*>::iterator it = find(circus_.begin(), circus_.end(), focused_);
            assert(it != circus_.end());
            pos = distance(circus_.begin(), it);
        }
        for (std::vector<widget*>::size_type i(0); i<circus_.size(); ++i)
        {
            widget* next = NULL;
            if (vk == VK_FOCUS_NEXT)
                next = circus_[++pos % circus_.size()];
            else
            {
                if (pos == 0)
                    pos = circus_.size() - 1;
                else
                    --pos;
                next = circus_[pos];
            }
            if (!next->can_focus())
                continue;
            
            if (focused_)
            {
                focused_->set_focus(false);
                focused_->invalidate(true);
            }
            focused_ = next;
            focused_->set_focus(true);
            focused_->set_cursor(cursor_);
            focused_->invalidate(true);
            break;
        }
        if (evtfocus) 
            evtfocus(this);
    }
    else if (vk == VK_OPEN_MENU && menu_ && !menu_->is_open())
    {
        if (!menu_->is_empty())
            menu_->open(0);
    }
    else
    {
        // if we have menu and it is open it overrides the focused widget
        bool ret = false;
        if (menu_ && menu_->is_open())
            ret = menu_->keydown(raw, vk);
        else if (focused_)
            ret = focused_->keydown(raw, vk);
        
        // key was not handled. nothing should be invalid.
        if (!ret)
            return false;
    }

    // see if some widgets become invalid as a result of input handling
    for (std::vector<widget*>::size_type i(0); i<circus_.size(); ++i)
    {
        if (!circus_[i]->is_valid())
        {
            is_valid_ = false;
            break;
        }
    }
    if (!is_valid_)
    {
        if (evtdraw)   evtdraw(this);
        if (evtcursor) evtcursor(this, cursor_);
    }
    return true;
}

void window::close()
{
    is_open_   = false;
}

void window::show()
{
    cursor_.v  = false;
    is_open_   = true;
    
    // find first focusable widget and give focus to it
    // if a focus was already set to a widget
    // do not override that setting
    if (focused_)
    {
        focused_->set_focus(true);
        focused_->set_cursor(cursor_);
        return;
    }

    // find the first focusable widget
    for (std::vector<widget*>::iterator it = circus_.begin(); it != circus_.end(); ++it)
    {
        widget* w = *it;
        if (w->can_focus())
        {
            w->set_focus(true);
            w->set_cursor(cursor_);
            focused_ = w;
            break;
        }
    }   
}

bool window::is_valid() const
{
    return is_valid_;
}

bool window::is_open() const
{
    return is_open_;
}

bool window::has_widget(const widget* w) const
{
    return find(circus_.begin(), circus_.end(), w) != circus_.end();
}

void window::can_close_on_vk(bool val)
{
    can_close_ = val;
}

} // cli

