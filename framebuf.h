// Copyright (c) 2014 Sami Väisänen, Ensisoft 
//
// http://www.ensisoft.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include "videoattr.h"
#include "types.h"

namespace cli
{
    // the framebuf defines a MxN (rows and columns) based buffer
    // of character cells into which a "frame" can be rendered.
    class framebuf 
    {
    public:
        struct cell {
            char_t value; // the character value in this cell
            cli::color  color;
            cli::attrib attrs;
        };

        struct row {
            row(std::size_t y, framebuf* fb) : y_(y), fb_(fb)
            {}
            cell& operator[](std::size_t x) 
            {
                assert(x < fb_->width_);
                return fb_->buffer_[y_ * fb_->width_ + x];
            }
        private:
            std::size_t y_;
            framebuf* fb_;
        };

        struct const_row {
            const_row(std::size_t y, const framebuf* fb) : y_(y), fb_(fb)
            {}
            const cell& operator[](std::size_t x) const
            {
                assert(x < fb_->width_);
                return fb_->buffer_[y_ * fb_->width_ + x];
            }
            private:
                std::size_t y_;
                const framebuf* fb_;
        };

        framebuf(std::size_t width, std::size_t height)
            : width_(width), height_(height)
        {
            buffer_.resize(height * width);
        }

        void clear()
        {
            const cell clear_value  = {0, cli::color::none, 
                cli::attrib::none};
            fill(clear_value);
        }

        void fill(const cell& value)
        {
            std::fill(buffer_.begin(), buffer_.end(), value);
        }

        row operator[](std::size_t y)
        {
            assert(y < height_);
            return row(y, this);
        }

        const_row operator[](std::size_t y) const
        {
            assert(y < height_);
            return const_row(y, this);
        }

        std::size_t width() const
        {
            return width_;
        }
        std::size_t height() const
        {
            return height_;
        }

    private:
        friend struct row;

        std::size_t width_;
        std::size_t height_;
        std::vector<cell> buffer_;
    };
} // cli