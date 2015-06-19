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


#include <boost/test/minimal.hpp>
#include <cli/widgets.h>
#include <iostream>
#include <string>
#include <vector>

struct conv
{
   conv(std::string&) {}
   conv(std::string&, int) {}
   const char* str() const { return "keke"; }
   const size_t len() const { return 4; }
};

struct stringdb
{
    typedef std::string value;
    typedef conv   converter;

    void fetch(value& v, int index) {}

    int size() const
    {
        return rowcount;
    }
    
    int rowcount;
};

/*
 * Synopsis: Verify that widgets report their invalid rectagnle correctly.
 *
 * Expected: Each widget reports a rectangle that actually describes
 *           the currently dirty area.
 *
 */
void test0()
{
    cli::buffer fb;
    fb.resize(50, 100);
    
    {
        cli::button b;
        b.position(0, 0);
        b.text("foobar");
    
        cli::rect r = b.draw(fb);
        BOOST_REQUIRE(r.top  == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 1);
        BOOST_REQUIRE(r.right  == 6);
        
        b.position(10, 5);
        r = b.draw(fb);
        BOOST_REQUIRE(r.top == 5);
        BOOST_REQUIRE(r.left == 10);
        BOOST_REQUIRE(r.bottom == 6);
        BOOST_REQUIRE(r.right == 10 + 6);

        BOOST_REQUIRE(b.height() == 1);
        BOOST_REQUIRE(b.width() == 6);
    }
    {
        cli::checkbox c;
        c.position(0, 0);
        c.text("foobar");

        cli::rect r = c.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 1);
        BOOST_REQUIRE(r.right >= 6); 
        
        
        c.position(10, 5);
        r = c.draw(fb);
        BOOST_REQUIRE(r.top = 5);
        BOOST_REQUIRE(r.left == 10);
        BOOST_REQUIRE(r.bottom == 6);        
        BOOST_REQUIRE(r.right >= 10 + 6);
        
        BOOST_REQUIRE(c.height() == 1);
        BOOST_REQUIRE(c.width() == 6 + 3); // foobar[*] or foobar[ ]
    }
    
    {
        cli::basic_input<> i;
        i.position(0, 0);
        i.width(40);
        
        cli::rect r = i.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 1);
        BOOST_REQUIRE(r.right == 40);
        
        BOOST_REQUIRE(i.height() == 1);
        BOOST_REQUIRE(i.width()  == 40);
    }
    {
        cli::basic_list<stringdb> l;
        l.position(0, 0);
        l.rowcount = 100;
        l.height(10);
        l.width(40);
        BOOST_REQUIRE(l.is_valid() == false);
        
        l.validate();
        BOOST_REQUIRE(l.is_valid() == true);
        l.invalidate(true);
        BOOST_REQUIRE(l.is_valid() == false);
        
        cli::rect r = l.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 10);
        BOOST_REQUIRE(r.right == 40);
        
        l.validate();

        bool ret = l.keydown(0, cli::VK_MOVE_DOWN);
        BOOST_REQUIRE(ret == true);
        
        l.position(5, 5);

        r = l.draw(fb);
        BOOST_REQUIRE(r.top == 5);
        BOOST_REQUIRE(r.left == 5);
        BOOST_REQUIRE(r.bottom == 7);
        BOOST_REQUIRE(r.right == 45);
        
        
        l.keydown(0, cli::VK_MOVE_DOWN);
        r = l.draw(fb);
        BOOST_REQUIRE(r.top == 6);
        BOOST_REQUIRE(r.bottom == 8);
    }
    {
        cli::basic_list<stringdb> l;
        l.position(0, 0);
        l.rowcount = 10;
        l.height(10);
        l.width(10);
        
        l.validate();
        l.keydown(0, cli::VK_MOVE_END);
        
        cli::rect r;
        r = l.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.bottom == 10);

    }
    {
        cli::basic_table<stringdb> t;
        t.position(0, 0);
        t.rowcount = 100;
        t.height(10);
        t.width(40);
        BOOST_REQUIRE(t.is_valid() == false);
        
        t.validate();
        BOOST_REQUIRE(t.is_valid() == true);
        t.invalidate(true);
        BOOST_REQUIRE(t.is_valid() == false);
        
        cli::rect r = t.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 10);
        BOOST_REQUIRE(r.right == 40);

    }

    {
        cli::menulist list;
        list.resize(2);
        list[0].text = "Menu 1";
        list[0].items.push_back(cli::make_menu_item("foobar"));
        list[0].items.push_back(cli::make_menu_item("menu item"));
        list[0].items.push_back(cli::make_menu_item("huhu"));
        list[1].text = "help menu";
        list[1].items.push_back(cli::make_menu_item("foobar"));

        cli::menu m;
        m.setmenu(list);
        m.position(1, 1);
        BOOST_REQUIRE(m.is_valid() == false);
        
        m.validate();
        BOOST_REQUIRE(m.is_valid() == true);
        m.invalidate(true);
        BOOST_REQUIRE(m.is_valid() == false);
        
        enum { menuspacing = 1 };

        cli::rect r = m.draw(fb);
        BOOST_REQUIRE(r.top == 1);
        BOOST_REQUIRE(r.left == 1);
        BOOST_REQUIRE(r.right == r.left + strlen("Menu 1") + strlen("help menu") + menuspacing);
        BOOST_REQUIRE(r.bottom == 2);
        
        r = m.erase();
        BOOST_REQUIRE(rect_is_empty(r));
        
        // open first submenu
        m.set_focus(true);
        m.keydown(0, cli::VK_ACTION_SPACE);
        
        r = m.draw(fb);
        // the invalid rectangle should now include the opened sub menu
        BOOST_REQUIRE(r.top == 1);
        BOOST_REQUIRE(r.left == 1);
        BOOST_REQUIRE(r.right == r.left + strlen("Menu 1") + strlen("help menu") + menuspacing);
        BOOST_REQUIRE(r.bottom == 5);
        
        // close the menu
        m.keydown(0, cli::VK_ACTION_SPACE);
        // the erase rectangle should now cover the first opened sub menu
        r = m.erase();
        BOOST_REQUIRE(r.top == 2);
        BOOST_REQUIRE(r.left == 1);
        BOOST_REQUIRE(r.right == r.left + strlen("menu item")); // then longest item dictates the width
        BOOST_REQUIRE(r.bottom == r.top + (int)list[0].items.size());

        // the paint rect should now be just the top level menu items
        r = m.draw(fb);
        BOOST_REQUIRE(r.top == 1);
        BOOST_REQUIRE(r.left == 1);
        BOOST_REQUIRE(r.right == r.left + strlen("menu 1") + strlen("help menu") + menuspacing);
        BOOST_REQUIRE(r.bottom == 2);

        // open second submenu
        m.keydown(0, cli::VK_MOVE_NEXT);
        m.keydown(0, cli::VK_ACTION_SPACE);
        
        m.draw(fb);
        
        // the erase rect show now cover only the second sub menu. check the offsets
        r = m.erase();
        BOOST_REQUIRE(r.top == 2);
        BOOST_REQUIRE(r.left == 1 + strlen("menu 1") + menuspacing);
        BOOST_REQUIRE(r.right == r.left + strlen("foobar"));
        BOOST_REQUIRE(r.bottom == 3);
        
    }
}

/*
 * Synopsis: Verify that the table widget works properly with non-zero
 *           cellspacing and doesnt overrun its allotted space vertically.
 *
 * Expected: Output is clipped to maximum allotted width.
 */
void test1()
{
    cli::buffer fb;
    fb.resize(50, 100);

    cli::cell c = {'X', cli::ATTRIB_NONE, cli::COLOR_NONE};
    fb.fill(c);
    {

        cli::basic_table<stringdb> t;
        t.position(0, 0);
        t.width(40);
        t.rowcount = 1;
        t.height(10);
        t.invalidate(true);
        
        t.addcol(10); // this sums up to 45 cols, which would violate the width
        t.addcol(35);
        
        cli::rect r = t.draw(fb);
        BOOST_REQUIRE(r.top  == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 10);
        BOOST_REQUIRE(r.right == 40);
        
        // make sure the actual "rendered area" is what the rect reports
        int count = 0;
        for (size_t row=0; row<fb.rows(); ++row)
        {
            cli::buffer::row_type& r = fb[row];
            for (size_t col=0; col<fb.cols(); ++col)
            {
                if (r[col].value != 'X')
                    ++count;
            }
        }
        BOOST_REQUIRE(count == 40 * 10);
    }

    fb.fill(c);
    {
        // same as above but with nonzero cell spacing

        cli::basic_table<stringdb> t;
        t.position(0, 0);
        t.width(40);
        t.rowcount = 1;
        t.height(10);
        t.invalidate(true);
        
        t.addcol(10);
        t.addcol(35);
        t.cellspacing(2);
        
        cli::rect r = t.draw(fb);
        BOOST_REQUIRE(r.top == 0);
        BOOST_REQUIRE(r.left == 0);
        BOOST_REQUIRE(r.bottom == 10);
        BOOST_REQUIRE(r.right  == 40);
        
        int count = 0;
        for (size_t row=0; row<fb.rows(); ++row)
        {
            cli::buffer::row_type& r = fb[row];
            for (size_t col=0; col<fb.cols(); ++col)
            {
                if (r[col].value != 'X')
                    ++count;
            }
        }
        BOOST_REQUIRE(count == 40 * 10);
    }
}



/*
 * Synopsis: Verify basic window functionality.
 *
 * Expected: Window provides accurate drawing and state information.
 *
 */
void test2()
{
    cli::buffer fb;
    fb.resize(50, 50);

    cli::window wnd;
        
    cli::button b1;
    cli::button b2;
    cli::button b3;
    
    b1.position(1, 1);
    b1.text("button1");
    b2.position(5, 5);
    b2.text("button2");
    b3.position(15, 46);
    b3.text("button3");

    wnd.add(&b1);
    wnd.add(&b2);
    wnd.add(&b3);
    
    BOOST_REQUIRE(wnd.is_valid() == false);
    BOOST_REQUIRE(wnd.has_widget(&b1) == true);
    BOOST_REQUIRE(wnd.has_widget(&b2) == true);
    BOOST_REQUIRE(wnd.has_widget(&b3) == true);
    
    // window by default picks up the first focusable 
    // widget and gives focus to it
    wnd.show();
    BOOST_REQUIRE(wnd.focused() == &b1);
    
    // the current invalid rectangle should be the union 
    // of the dirty rectangles of all invalid widgets.
    cli::rect rc = wnd.draw(fb);
    BOOST_REQUIRE(rect_is_empty(rc) == false);
    BOOST_REQUIRE(rc.top    == 1);
    BOOST_REQUIRE(rc.bottom == 47);
    BOOST_REQUIRE(rc.left   == 1);
    BOOST_REQUIRE(rc.right  == 22); // b3 x offset + width
    
    // after drawing window should be valid
    BOOST_REQUIRE(wnd.is_valid() == true);
 
    // move focus from b1 to b2.
    // b32 is unchanged so the invalid rect should only
    // be an union of b1 and b2.
    wnd.focus(&b2);
    BOOST_REQUIRE(wnd.focused() == &b2);
    BOOST_REQUIRE(wnd.is_valid() == false);
    
    rc = wnd.draw(fb);
    BOOST_REQUIRE(rect_is_empty(rc) == false);
    BOOST_REQUIRE(rc.top       == 1);
    BOOST_REQUIRE(rc.bottom    == 6);
    BOOST_REQUIRE(rc.left      == 1);
    BOOST_REQUIRE(rc.right     == 12);
    BOOST_REQUIRE(wnd.is_valid() == true);
    
    // if draw is now called nothing should be done.
    rc = wnd.draw(fb);
    BOOST_REQUIRE(rect_is_empty(rc));
    
    // request a widget to be updated.
    wnd.update(&b3);
    BOOST_REQUIRE(wnd.is_valid() == false);
    
    rc = wnd.draw(fb);
    BOOST_REQUIRE(rect_is_empty(rc) == false);
    // b3 area only
    BOOST_REQUIRE(rc.top    == 46);
    BOOST_REQUIRE(rc.bottom == 47);
    BOOST_REQUIRE(rc.left   == 15);
    BOOST_REQUIRE(rc.right  == 22);
    BOOST_REQUIRE(wnd.is_valid() == true);

    // request all widgets to be updated
    wnd.invalidate();
    
    rc = wnd.draw(fb);
    BOOST_REQUIRE(rect_is_empty(rc) == false);
    BOOST_REQUIRE(rc.top    == 1);
    BOOST_REQUIRE(rc.bottom == 47);
    BOOST_REQUIRE(rc.left   == 1);
    BOOST_REQUIRE(rc.right  == 22);
  
}

template<typename Policy>
struct test_selection : public Policy
{
    bool test_keydown(int vk, int page_height, int max)
    {
        bool invalid = false;
        return Policy::keydown(vk, page_height, max, invalid);
    }
    
    bool test_is_selected(int row) const
    {
        return Policy::is_selected(row);
    }
    
    bool test_reset() 
    {
        return Policy::reset();
    }
};


bool row_changed;
bool row_selected;

void row_change()
{
    row_changed = true;
}

void row_select()
{
    row_selected = true;
}

/*
 * Synopsis: Verify that single selection works.
 *
 * Expected: Single selection policy selects and reports selection properly.
 *
 */
void test3()
{
    test_selection<cli::default_single_selection> sel;
    
    BOOST_REQUIRE(sel.selpos() == 0);
    
    BOOST_REQUIRE(sel.test_keydown(200, 10, 20) == false);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 1);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_UP, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 0);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN_PAGE, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 10);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN_PAGE, 10, 15));
    BOOST_REQUIRE(sel.selpos() == 14);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_HOME, 10, 10));
    BOOST_REQUIRE(sel.selpos() == 0);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_END, 10, 25));
    BOOST_REQUIRE(sel.selpos() == 24);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_HOME, 10, 10));

    row_changed  = false;
    row_selected = false;
    
    sel.evtrow    = row_change;
    sel.evtselect = row_select;

    // wrap over from first to last the item
    sel.test_keydown(cli::VK_MOVE_UP, 10, 20);
    BOOST_REQUIRE(sel.selpos() == 19);
    BOOST_REQUIRE(row_changed);
    
    sel.test_keydown(cli::VK_ACTION_SPACE, 10, 20);
    BOOST_REQUIRE(sel.selpos() == 19);
    BOOST_REQUIRE(row_selected);


    BOOST_REQUIRE(sel.test_is_selected(-1) == false);
    BOOST_REQUIRE(sel.test_is_selected(100) == false);
    BOOST_REQUIRE(sel.test_is_selected(sel.selpos()));
}


/*
 * Synopsis: Verify that multi selection works.
 *
 * Expected: Multi selection policy selects and reports selection properly.
 *
 */
void test4()
{
    test_selection<cli::default_multi_selection> sel;
    
    cli::default_multi_selection::range r = sel.selrange();
    BOOST_REQUIRE(r.start == 0);
    BOOST_REQUIRE(r.end   == 1);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 1);
    r = sel.selrange();
    BOOST_REQUIRE(r.start == 1);
    BOOST_REQUIRE(r.end   == 2);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_UP, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 0);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_END, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 19);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_HOME, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 0);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN_PAGE, 10, 20));
    BOOST_REQUIRE(sel.selpos() == 10);
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_UP_PAGE, 15, 20));
    BOOST_REQUIRE(sel.selpos() == 0);
    
    BOOST_REQUIRE(sel.test_keydown(cli::VK_SET_MARK, 10, 20));
    BOOST_REQUIRE(sel.test_keydown(cli::VK_MOVE_DOWN, 10, 20));
    r = sel.selrange();
    BOOST_REQUIRE(r.start == 0);
    BOOST_REQUIRE(r.end   == 2);
    sel.test_reset();
    BOOST_REQUIRE(sel.selpos() == 1);
    r = sel.selrange();
    BOOST_REQUIRE(r.start == 1);
    BOOST_REQUIRE(r.end   == 2);

    row_changed   = false;
    row_selected  = false;
    sel.evtrow    = row_change;
    sel.evtselect = row_select;
    
    sel.test_keydown(cli::VK_MOVE_DOWN, 10, 20);
    BOOST_REQUIRE(row_changed);
    
    sel.test_keydown(cli::VK_ACTION_SPACE, 10, 20);
    BOOST_REQUIRE(row_selected);
}

template<typename Policy>
struct test_pager : public Policy
{
    std::pair<int, int> test_getpage(int pos, int page_height, int max)
    {
        return Policy::getpage(pos, page_height, max);
    }

    bool test_is_dirty(int pos, int page_height)
    {
        return Policy::is_dirty(pos, page_height);
    }
    
    void test_validate(int pos, int page_height)
    {
        Policy::validate(pos, page_height);
    }

};

/*
 * Synopsis: Verify that default pager works as expected.
 *
 * Expected: Data is split correctly into fixed size pages. 
 *           Dirty lines are indicated so.
 * 
 */
void test5()
{
    test_pager<cli::default_pager> page;
    
    // assume that we have a list of data of 55 items.
    // this is split over to "pages" each page begin 7 rows of data.
    // With this we get 8 pages where the last page is not full.
    std::pair<int, int> p;

    p = page.test_getpage(0, 7, 55);
    
    BOOST_REQUIRE(p.first  == 0);
    BOOST_REQUIRE(p.second == 7);
    
    p = page.test_getpage(8, 7, 55);
    BOOST_REQUIRE(p.first == 7);
    BOOST_REQUIRE(p.second == 14);
    
    BOOST_REQUIRE(page.test_is_dirty(7, 7));
    BOOST_REQUIRE(page.test_is_dirty(8, 7));
    BOOST_REQUIRE(page.test_is_dirty(13, 7));
    
    page.test_validate(8, 7);
    //BOOST_REQUIRE(page.test_is_dirty(7, 7) == false);
    //BOOST_REQUIRE(page.test_is_dirty(8, 7) == false);
    //BOOST_REQUIRE(page.test_is_dirty(13, 7) == false);

}

/*
 * Synopsis: Verify that formatter object works as expected.
 * 
 * Expected: Data is formatted appropriately. No out of bounds formatting.
 */
void test6()
{
    cli::buffer fb;
    fb.resize(1, 50);
    
    cli::cell blank = {0,   cli::ATTRIB_NONE, cli::COLOR_NONE};
    cli::cell fill  = {'x', 100, 150};

    fb.fill(blank);

    cli::formatter f(fill, fb);
    
    {
        // make sure that when blank filling is not set on
        // the space after the string is left untouched

        f.fillblank(false);
        f.move(0, 0);
        f.print("foo", 20);
        
        BOOST_REQUIRE(fb[0][0] == cli::make_cell('f', 100, 150));
        BOOST_REQUIRE(fb[0][1] == cli::make_cell('o', 100, 150));
        BOOST_REQUIRE(fb[0][2] == cli::make_cell('o', 100, 150));
        
        // following cells on this row are left untouched
        BOOST_REQUIRE(fb[0][3] == blank);
        BOOST_REQUIRE(fb[0][4] == blank);
        BOOST_REQUIRE(fb[0][49] == blank);
        
    }
    
    fb.fill(blank);

    {
        // make sure that when filling is set on
        // the space after the string is filled with default cell values
        f.fillblank(true);
        f.move(0, 0);
        f.print("foo", 20);

        BOOST_REQUIRE(fb[0][0] == cli::make_cell('f', 100, 150));
        BOOST_REQUIRE(fb[0][1] == cli::make_cell('o', 100, 150));
        BOOST_REQUIRE(fb[0][2] == cli::make_cell('o', 100, 150));

        // these are all filled
        BOOST_REQUIRE(fb[0][3]  == fill);
        BOOST_REQUIRE(fb[0][4]  == fill);
        BOOST_REQUIRE(fb[0][19] == fill); // this is the last cell to be filled 
        
        BOOST_REQUIRE(fb[0][20] == blank);
        BOOST_REQUIRE(fb[0][49] == blank);
        
    }
    
    fb.fill(blank);
    
    {
        // make sure that only part of the part of the string specified
        // by length is transferred to the frame buffer.
        // also make sure that the space after the transferred string is left untouched.
        f.fillblank(false);
        f.move(0, 0);
        f.print("a longer string than foo", 8, 20);
        
        BOOST_REQUIRE(fb[0][0] == cli::make_cell('a', 100, 150));
        BOOST_REQUIRE(fb[0][1] == cli::make_cell(' ', 100, 150));
        BOOST_REQUIRE(fb[0][2] == cli::make_cell('l', 100, 150));
        BOOST_REQUIRE(fb[0][3] == cli::make_cell('o', 100, 150));
        BOOST_REQUIRE(fb[0][4] == cli::make_cell('n', 100, 150));
        BOOST_REQUIRE(fb[0][5] == cli::make_cell('g', 100, 150));
        BOOST_REQUIRE(fb[0][6] == cli::make_cell('e', 100, 150));        
        BOOST_REQUIRE(fb[0][7] == cli::make_cell('r', 100, 150));
        BOOST_REQUIRE(fb[0][8]  == blank);
        BOOST_REQUIRE(fb[0][49] == blank);
    }

    fb.fill(blank);

    {
        // make sure that only part of the the string indicated by
        // length is transferred to the frame buffer.
        // also make sure that the space after the string is filled.
        f.fillblank(true);
        f.move(0, 0);
        f.print("a longer string than foo", 8, 20);
        
        BOOST_REQUIRE(fb[0][0] == cli::make_cell('a', 100, 150));
        BOOST_REQUIRE(fb[0][1] == cli::make_cell(' ', 100, 150));
        BOOST_REQUIRE(fb[0][2] == cli::make_cell('l', 100, 150));
        BOOST_REQUIRE(fb[0][3] == cli::make_cell('o', 100, 150));
        BOOST_REQUIRE(fb[0][4] == cli::make_cell('n', 100, 150));
        BOOST_REQUIRE(fb[0][5] == cli::make_cell('g', 100, 150));
        BOOST_REQUIRE(fb[0][6] == cli::make_cell('e', 100, 150));        
        BOOST_REQUIRE(fb[0][7] == cli::make_cell('r', 100, 150));
        
        BOOST_REQUIRE(fb[0][8]  == fill);
        BOOST_REQUIRE(fb[0][19] == fill);
        
        BOOST_REQUIRE(fb[0][20] == blank);
        BOOST_REQUIRE(fb[0][49] == blank);

    }

    fb.fill(blank);

    {
        // make sure that buffer is not overrun (valgrind)
        f.fillblank(false);
        f.move(0, 0);
        f.print("", 100);

        f.move(49, 0);
        f.print("", 100);
    }
}


int test_main(int, char* [])
{
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();

    return 0;
}
