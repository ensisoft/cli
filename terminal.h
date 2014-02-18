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

#include <memory>
#include "types.h"
#include "keys.h"

namespace cli
{
    class framebuf;

    // provides access to the underlying terminal/console object
    // that is provided by the system when the process is started.
    class terminal
    {
    public:
        struct keypress {
            cli::keymod mod;
            cli::keysym sym;
        };

        terminal();

       ~terminal();

        // draw the contents of the framebuf in the terminal window.
        void draw(const framebuf& fb);

        // get current terminal width in character columns
        std::size_t width() const;

        // get current termianal height in character rows.
        std::size_t height() const;

        // get the next input keypress 
        keypress wait();

//        cli::handle_t r

    private:
        struct impl;

        std::unique_ptr<impl> pimpl_;
    };

} // cli