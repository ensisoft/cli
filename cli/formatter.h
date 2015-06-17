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

#include "buffer.h"
#include "common.h"
#include <cassert>

namespace cli
{

    // Formatter formats data into the frame buffer provided. 
    // Currently formatting only strings is supported, because
    // it is such a simple operation. This means that any detailed
    // conversions, such as monetary, integer conversions and such 
    // have to be done somewhere else.
    //
    // In order to improve efficiency it would possible to also provide
    // such functions here at the cost of increased complexity. 
    //    
    // todo: should this use integers and allow negative positions too? 
    class formatter
    {
    public:
       ~formatter() {}
        formatter(const cell& def, buffer& fb) :
          default_(def), fb_(fb), posx_(0), posy_(0), fillblank_(true) 
          {
              memset(&blank_, 0, sizeof(cell));
          }
        
        formatter(const cell& def, const cell& blank, buffer& fb) :
          default_(def), blank_(blank), fb_(fb), posx_(0), posy_(0), fillblank_(true)
         {
         }
        formatter(buffer& fb) : 
          fb_(fb), posx_(0), posy_(0), fillblank_(true)
        {
            memset(&default_, 0, sizeof(cell));
            memset(&blank_, 0, sizeof(cell));
        }

        // Set default cell that is to be applied to cells with data.
        void setdef(const cell& def)
        {
            default_ = def;
        }
        
        // Set the blank cell that is to be applied to blank cells.
        void setblank(const cell& blank)
        {
            blank_ = blank;
        }

        // Set this true to enable filling the blank cells
        // with the default cell rather than with the blank cell.
        void fillblank(bool fill)
        {
            fillblank_ = fill;
        }
        
        // Transfer a NUL terminated string into the frame buffer. 
        // The space reserved for this string is given in width.
        // If width is greater than the length of the string, the remaining
        // cells are filled either with blanks or with default cell attributes.
        void print(const char* s, size_t width)
        {
            assert(s);
            size_t x = posx_;
            size_t y = posy_;
            bool got_null = false;
            for (size_t i=0; i<width && x<fb_.cols(); ++i, ++x)
            {
                if (!got_null)
                    got_null = s[i] == 0;

                buffer::row_type& r = fb_[y];
                assert(x >= 0);
                assert(x < r.size());
                if (got_null)
                {
                    if (fillblank_)
                        r[x] = default_;
                    else
                        r[x] = blank_;
                }
                else
                {
                    r[x] = default_;
                    r[x].value = s[i];
                }
            }
        }
        
        // As above, except that the string length is specified explicitly.
        void print(const char* s, size_t len, size_t width)
        {
            assert(s);
            size_t x = posx_;
            size_t y = posy_;
            for (size_t i=0; i<width && x<fb_.cols(); ++i, ++x)
            {
                buffer::row_type& r = fb_[y];
                assert(x >= 0);
                assert(x < r.size());
                if (i<len)
                {
                    r[x] = default_;
                    r[x].value = s[i];
                }
                else
                {
                    if (fillblank_)
                        r[x] = default_;
                    else
                        r[x] = blank_;
                }
            }
        }
        
        // Move the internal pointer to a new
        // location within the frame buffer.
        void move(size_t x, size_t y)
        {
            posx_ = x;
            posy_ = y;
        }
    private:
        cell default_;
        cell blank_;
        buffer& fb_;
        size_t posx_;
        size_t posy_;
        bool fillblank_;
    };

} // cli

