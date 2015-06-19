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
#include <cassert>
#include <algorithm>

namespace cli
{
    // The default pager splits data into fixed size pages. 
    class default_pager
    {
    public:
        
    protected:
       ~default_pager() {}
        default_pager() : old_(0), pos_(0), page_(-1) {}
        
        std::pair<int, int> getpage(int pos, int page_height, int max)
        {
            std::pair<int, int> ret; 
            if (page_height == 0)
                return std::make_pair(-1, -1);                
            int page   = pos / page_height;
            ret.first  = page * page_height;
            ret.second = ret.first + page_height;
            
            old_ = pos_;
            pos_ = pos;
            return ret;
        }
        
        std::pair<int, int> getvisible(int pos, int page_height, int max) const
        {
            if (page_height == 0)
                return std::make_pair(-1, -1);
            int page = pos / page_height;
            std::pair<int, int> ret;
            ret.first  = page * page_height;
            ret.second = ret.first + std::min<int>(max - page * page_height, page_height);
            return ret;
        }

        bool is_dirty(int pos, int page_height)
        {
            if (page_height == 0)
                return false;
            if (pos / page_height != page_)
                return true;

            return pos == old_ || pos == pos_;
        }
        void validate(int pos, int page_height)
        {
            if (page_height)
                page_ = pos / page_height;
        }
        void invalidate()
        {
            page_ = -1;
        }

        // Return the relative position of the selection on a single page.
        int pagepos(int pos, int page_height)
        {
            if (page_height == 0)
                return 0;
            return pos % page_height;
        }

    private:
        int  old_;
        int  pos_;
        int  page_;
    };


} // cli



 
