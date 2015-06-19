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

#include <string>
#include <vector>
#include <cassert>

namespace cli
{
    // Tablist contains a vector of strings that can be 
    // traversed on tab completion events. 
    class tablist
    {
    public:
        typedef std::vector<std::string> list;

        void setlist(const list& tabs)
        {
            tabs_ = tabs;
        }
        list& getlist() 
        {
            return tabs_;
        }
        const list& getlist() const
        {
            return tabs_;
        }
        void settab(int i)
        {
            pos_ = i;
        }
    protected:
        tablist() : pos_(0) {}
       ~tablist() {}
        
        std::string next() const
        {
            if (tabs_.empty())
                return "";
            return tabs_[++pos_ % tabs_.size()];
        }
        std::string prev() const 
        { 
            if (tabs_.empty())
                return "";
            if (--pos_ < 0)
                pos_ = tabs_.size() -1;
            return tabs_[pos_ % tabs_.size()];
        }

        void update(const std::string&, char) {}
    private:
        list         tabs_;
        mutable int  pos_;
    };

} // cli

