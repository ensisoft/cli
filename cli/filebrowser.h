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

#include "config.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <string>
#include <vector>
#include <algorithm>

namespace cli
{
    // Filebrowser tab completion policy.
    // This tab policy will traverse the file system and update its 
    // internal list of possible completions every time a path separator
    // is processed. For windows this means "\\" and for linux "/".
    class filebrowser
    {
    public:
        enum filemask { files = 0x1, folders = 0x2, hidden = 0x4 };
        
        void set_file_mask(int mask)
        {
            fmask_ = mask;
        }
    protected:
        filebrowser() : index_(0), fmask_(0) {}
       ~filebrowser() {}

        std::string next()
        {
            if (match_.empty())
                return "";
            return match_[++index_ % match_.size()];
        }
        std::string prev()
        {
            if (match_.empty())
                return "";
            if (index_ > 0) 
                --index_;
            else
                index_ = match_.size()-1;
            return match_[index_ % match_.size()];
        }
        
        void update(const std::string& item, int in)
        {
#if defined(_WIN32)
            const int SEPARATOR = '\\';
#else
            const int SEPARATOR = '/';
#endif
            if (in == SEPARATOR)
                update_tab_list(item);
            else
                search_tab_list(item);
        }
    private:
        typedef std::vector<std::string> path_container;
        
        void update_tab_list(const std::string& item)
        {
            using namespace std;
            using namespace boost;
            using namespace boost::filesystem;

            match_.clear();
            paths_.clear();
            index_ = 0;

            path ph(item);
            if (!exists(ph))
                return;
            
            try
            {
                directory_iterator end;
                for (directory_iterator it(ph); it != end; ++it)
                {
                    const path& p = *it;
#ifdef LINUX
                    string s = p.leaf().string();
                    if (!s.empty() && s[0] == '.' && !(fmask_ & hidden))
                        continue;
#endif
                    string native = p.string(); //(*it).native_file_string();
#ifdef WINDOWS
                    replace_first(native, "\\\\", "\\");
#endif
                    try {
                        if (is_directory(*it))
                        {
                            if (fmask_ & folders) 
                                paths_.push_back(native);
                        }
                        else
                        {
                            if(fmask_ & files) 
                                paths_.push_back(native);
                        }
                    }
                    catch (const std::exception&) 
                    {
                        // e.g. permission denied. 
                        // simply ignore this for now, todo: fix this
                    }
                }
            }
            catch (const std::exception&)
            {
                paths_.clear();
                match_.clear();
            }
            sort(paths_.begin(), paths_.end());
            copy(paths_.begin(), paths_.end(), back_inserter(match_));
            index_ = 0;
        }
        void search_tab_list(const std::string& filter)
        {
            match_.clear();
            if (filter.empty())
            {
                std::copy(paths_.begin(), paths_.end(), std::back_inserter(match_));
                return;
            }

            // copy all matches from paths_ into match_
            for (path_container::iterator it = paths_.begin(); it != paths_.end(); ++it)
            {
                const std::string& s = *it;
                if (!strncmp(s.c_str(), filter.c_str(), filter.size()))
                    match_.push_back(s);
            }
            index_ = 0;
        }

        path_container paths_;
        path_container match_;
        path_container::size_type index_;
        int fmask_;
    };

} // cli


 
