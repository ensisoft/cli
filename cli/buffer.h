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

#include <vector>
#include <cassert>
#include "common.h"

namespace cli
{
    // buffer represents an astract container of character cells
    // for widgets to output their information into.
    // A frame buffer is a collection of M rows each N columns wide.
    class buffer
    {
    public:
        typedef std::vector<cell> row_type;
        typedef std::vector<row_type> row_map;

        buffer() {}

        buffer(size_t rows, size_t cols)
        {
            rows_.resize(rows);
            for (size_t row=0; row<rows; ++row)
                rows_[row].resize(cols);
        }

        void resize(size_t rows, size_t cols)
        {
            rows_.resize(rows);
            for (size_t row=0; row<rows; ++row)
                rows_[row].resize(cols);
        }

        const row_type& operator[](size_t y) const
        {
            assert(y < rows_.size());
            return rows_[y];
        }
        row_type& operator[](size_t x)
        {
            assert(x < rows_.size());
            return rows_[x];
        }
        size_t rows() const 
        {
            return rows_.size();
        }
        size_t cols() const
        {
            return rows_[0].size();
        }
        void clear()
        {
            const cell c = {' ', ATTRIB_NONE, COLOR_NONE};
            for (size_t row=0; row<rows(); ++row)
            {
                buffer::row_type& r = rows_[row];
                for (size_t col=0; col < cols(); ++col)
                    r[col] = c;
            }
        }

        void clear(const rect& rc)
        {
            const cell c = {' ', ATTRIB_NONE, COLOR_NONE};
            for (size_t row=rc.top; row<rc.bottom; ++row)
            {
                buffer::row_type& r = rows_[row];
                for  (size_t col=rc.left; col<rc.right; ++col)
                    r[col] = c;
            }
        }

        void fill(const cell& value)
        {
            for (size_t row=0; row<rows(); ++row)
            {
                buffer::row_type& r = rows_[row];
                for (size_t col=0; col<cols(); ++col)
                    r[col] = value;
            }
        }

    private:
        row_map rows_;
    };

} // cli


