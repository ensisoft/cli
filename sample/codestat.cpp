//
// Copyright (c) 2008 Sami Väisänen
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

#if defined(_WIN32)
#  define WINDOWS
#else
#  define LINUX
#endif

#ifdef WINDOWS
#  define _WIN32_WINNT 0x0500
#  include <windows.h> // for directory api
#elif defined(LINUX)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <dirent.h>
#endif
#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <cli/widgets.h>
#include <cli/terminal.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace cli;
using namespace std;
using namespace boost;
namespace po = boost::program_options;

enum application_keys {
    VK_EXIT_APPLICATION = cli::VK_SENTINEL + 1,
    VK_SORT_BY_NAME,
    VK_SORT_BY_SIZE,
    VK_SORT_BY_CODE,
    VK_SORT_BY_BLANK,
    VK_EXPORT
};

struct file {
    string name;
    int    size;
    int    lines_code;
    int    lines_blank;
};

struct code_stat {
    int lines_code;
    int lines_blank;
    int files;
};

void build_file_stats(file* f, code_stat* s, const std::string& path)
{
    ifstream file;
    file.open(path.c_str());
    if (!file.is_open())
        return;

    while (file.good())
    {
        string line;
        getline(file, line);
        if (line.empty() || line == "\n" || line == "\r\n")
        {
            ++f->lines_blank;
            ++s->lines_blank;
        }
        else
        {
            ++f->lines_code;
            ++s->lines_code;
        }
    }
}

void build_file_list(const string& folder, const regex& include, const regex& exclude, vector<file*>& files, code_stat& codestat)
{
    // todo: proper error checking
#ifdef WINDOWS
    stringstream ss;
    string wildcard = folder + "\\*";
    WIN32_FIND_DATA fd = {};
    HANDLE dir = 0;
    while (true)
    {
        memset(&fd, 0, sizeof(fd));
        if (dir == 0)
        {
            dir = FindFirstFile(wildcard.c_str(), &fd);
            assert(dir != INVALID_HANDLE_VALUE && "failed to open directory");
        }
        else
        {
            BOOL ret = FindNextFile(dir, &fd);
            if (ret == FALSE)
                break;
        }
        string name = fd.cFileName;
        string path = folder + "\\" + name;
        
        file* f        = new file;
        f->size        = static_cast<int>(fd.nFileSizeLow);
        f->lines_code  = 0;
        f->lines_blank = 0;
        f->name        = path;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            f->size  = 0;
            if (name != "." && name != "..")
                build_file_list(path, include, exclude, files, codestat);
        }
        else
        {
            if (regex_search(name, include))
            {
                if (exclude.empty() || !regex_search(path, exclude))
                {
                    build_file_stats(f, &codestat, path);
                    files.push_back(f);
                    ++codestat.files;
                }
            }
        }
    }
    BOOL ret = FindClose(dir);
    assert( ret == TRUE );
    ret = 0;
#endif
#ifdef LINUX
    DIR* dir = opendir(folder.c_str());
    assert(dir && "failed to open directory");
    
    while (true)
    {
        struct dirent* d = readdir(dir);
        if (d == NULL) 
            break;
        
        // the contents of what d points to might get overwritten
        // on subsequent calls to readdir
        string name = d->d_name;
        string path = folder + "/" + name;
        struct stat st = {};
        stat(path.c_str(), &st);
        
        // simply allocate memory on the free store. 
        // techinically this is not a leak since the allocated space
        // does not grow later on. And its used untill exit.
        file* f        = new file; 
        f->size        = static_cast<int>(st.st_size);
        f->lines_code  = 0;
        f->lines_blank = 0;
        f->name        = path;
        if (st.st_mode & S_IFDIR)
        {
            f->size  = 0;
            if (name != "." && name != "..")
                build_file_list(path, include, exclude, files, codestat);
        }
        else
        {
            if (regex_search(name, include))
            {
                if (exclude.empty() || !regex_search(path, exclude))
                {
                    build_file_stats(f, &codestat, path);
                    files.push_back(f);
                    ++codestat.files;
                }
            }
        }
    }
    closedir(dir);
#endif

}

class file_tree_data
{
public:
    vector<file*>* files;
protected:
   ~file_tree_data() {}
    file_tree_data() : files(NULL) {}
    typedef file* value;
    
    class converter {
    public:
        converter(const value& val, int col)
        {
            stringstream ss;
            switch (col)
            {
                case 0: str_ = val->name;   break;
                case 1:
                    ss << val->size / 1024;
                    str_ = ss.str();
                    break;
                case 2:
                    ss << val->lines_code;
                    str_ = ss.str();
                    break;
                case 3:
                    ss << val->lines_blank;
                    str_ = ss.str();
                    break;
                default: assert(0); break;
            }
        }
        inline 
        const char* str() const
        {
            return str_.c_str();
        }
        inline
        size_t len() const 
        {
            return str_.size();
        }
    private:
        string str_;
    };

    // access a "row" of data at the specified index.
    void fetch(value& val, int index) const
    {
        assert(files);
        assert(index >= 0);
        assert(index < static_cast<int>(files->size()));
        val = (*files)[index];
    }
    inline
    int size() const
    {
        assert(files);
        return static_cast<int>(files->size());
    }


private:
};

enum export_status {
    EXPORT_SUCCESS,
    EXPORT_FAIL_FILE_EXISTS,
    EXPORT_FAIL
};

export_status export_html(const vector<file*>& files, const std::string& file, const code_stat& stat, const std::string& include, const std::string& exclude)
{
    FILE* out = fopen(file.c_str(), "w");
    if (out == NULL)
        return EXPORT_FAIL;
    fprintf(out, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
    fprintf(out, "<html>\n");
    fprintf(out, "  <head><title></title>\n");
    fprintf(out, "  </head>\n");
    fprintf(out, "  <body>\n");
    fprintf(out, "  showing: %s excluding: %s<br><br>\n", include.c_str(), exclude.c_str());
    fprintf(out, "  <table border=0 cellspacing=0 cellpadding=0>\n");
    fprintf(out, "    <tr bgcolor=lightgray>\n");
    fprintf(out, "     <th align=left width=600>Name </th>\n");
    fprintf(out, "     <th align=left width=100>KB    </th>\n");
    fprintf(out, "     <th align=left width=100>Code  </th>\n");
    fprintf(out, "     <th align=left width=100>Blank </th>\n");
    fprintf(out, "    </tr>\n");
    
    for (unsigned i(0); i<files.size(); ++i)
    {
        const ::file* f = files[i];
        fprintf(out, "    <tr><td>%s</td> <td>%d</td> <td>%d</td> <td>%d</td></tr>\n", f->name.c_str(), f->size / 1024, f->lines_code, f->lines_blank);
    }
    fprintf(out, "  </table><br><br>\n");
    fprintf(out, "  Total LOC: %d blank: %d files: %d\n", stat.lines_code, stat.lines_blank, stat.files);
    fprintf(out, "  </body>\n");
    fprintf(out, "</html>\n");
    fclose(out);
    return EXPORT_SUCCESS;
}

inline
bool sort_by_name(const file* one, const file* two)
{
    return one->name < two->name;
}

inline
bool sort_by_size(const file* one, const file* two)
{
    return one->size > two->size;
}

inline
bool sort_by_code(const file* one, const file* two)
{
    return one->lines_code > two->lines_code;
}

inline
bool sort_by_blank(const file* one, const file* two)
{
    return one->lines_blank > two->lines_blank;
}

void print_version()
{
    cout << "\ncodestat 0.1";
    cout << "\nCopyright (c) 2008 Sami Vaisanen\n";
    cout << "\nwww.ensisoft.com";
}

int map_input(int ch)
{
    switch (ch)
    {
        case TERM_MOVE_DOWN:      return VK_MOVE_DOWN;
        case TERM_MOVE_UP:        return VK_MOVE_UP;
        case TERM_MOVE_HOME:      return VK_MOVE_HOME;
        case TERM_MOVE_END:       return VK_MOVE_END;
        case TERM_MOVE_DOWN_PAGE: return VK_MOVE_DOWN_PAGE;
        case TERM_MOVE_UP_PAGE:   return VK_MOVE_UP_PAGE;
        case 'q':                 return VK_EXIT_APPLICATION;
        case 'n':                 return VK_SORT_BY_NAME;
        case 's':                 return VK_SORT_BY_SIZE;
        case 'c':                 return VK_SORT_BY_CODE;
        case 'b':                 return VK_SORT_BY_BLANK;
        case 'e':                 return VK_EXPORT;
    }
    return -1;
}

void draw_window(cli::window* win, cli::buffer* fb)
{
    const cli::rect dirty = win->draw(*fb);

    cli::term_draw_buffer(*fb, dirty);
}

int main(int argc, const char* argv[])
{
    try 
    {
        std::string include;
        std::string exclude;
        std::string exportfile("export.html");

        po::options_description desc("Options");
        desc.add_options()
          ("help",    "Print help")
          ("version", "Print version")
          ("include", po::value<std::string>(&include), "Regular expression for file inclusion")
          ("exclude", po::value<std::string>(&exclude), "Regular expression for file exlucions")
          ("file",    po::value<std::string>(&exportfile), "Export results to file")
          ("java",    "Default regex for Java")
          ("c",       "Default regex for C")
          ("cpp",     "Default regex for C")
          ("python",  "Default regex for Python");
        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help"))
        {
            std::cout << desc;
            return 0;
        }
        else if (vm.count("version"))
        {
            print_version();
            return 0;
        }
        if (vm.count("java"))
        {
            if (!include.empty())
                include += "|";
            include += "(\\.java$)";
        }
        if (vm.count("c"))
        {   
            if (!include.empty())
                include += "|";
            include += "(\\.c$)|(\\.h$)";
        }
        if (vm.count("cpp"))
        {
            if (!include.empty())
                include += "|";
            include += "(\\.cpp$)|(\\.h$)|(\\.hpp$)|(\\.cxx$)|(\\.cc$)|(\\.c$)";            
        }
        if (vm.count("python"))
        {
            if (!include.empty())
                include += "|";
            include += "(\\.py$)";
        }

        // search current working directory
        std::string cwd;

#ifdef WINDOWS
        char foo[255] = {};
        DWORD ret = GetCurrentDirectory(sizeof(foo), foo);
        assert( ret > 0 && ret < sizeof(foo));
        cwd.append(foo, ret);
#elif defined(LINUX)
        cwd = ".";
#endif
        if (include.empty())
            include = "\\.*";
    
        regex inc(include);
        regex exc;
        if (!exclude.empty())
        {
            regex temp(exclude);
            exc.swap(temp);
        }
        code_stat stat = {};

        // get some data. 
        vector<file*> files;
        build_file_list(cwd, inc, exc, files, stat);

        if (vm.count("file"))
        {
            export_html(files, exportfile, stat, include, exclude);
            cout << "Wrote: " << exportfile << std::endl;
            return 0;
        }
    

        term_init();
        term_init_colors();

        cli::size size = term_get_size();
        assert(size.cols && size.rows);

        cli::buffer framebuff;
        framebuff.resize(size.rows, size.cols);

        window wnd;
        wnd.evtdraw = boost::bind(draw_window, _1, &framebuff);

        basic_table<file_tree_data> list;
        list.files = &files;
        list.addcol(65); // name
        list.addcol(5); // size
        list.addcol(10);  // lines_code
        list.addcol(10);  // lines_blank
        list.cellspacing(2);
        list.position(0, 4);
        list.width(size.cols);
        list.height(size.rows - 8);

        text text1;
        text text2;
        text text3;
        text text4;

        text1.position(0, 0);
        text1.width(size.cols);
        text1.settext(cwd + " codestat 0.1 showing: " + include + " excluding: " + exclude);
        text1.setattrib(ATTRIB_UNDERLINE | ATTRIB_BOLD | ATTRIB_STANDOUT);

        stringstream ss;
        ss << left;
        ss << setw(67) << setfill(' ') << "[Name";
        ss << setw(7) << setfill(' ') << "KB";
        ss << setw(12)  << setfill(' ') << "Code";
        ss << setw(12)  << setfill(' ') << "Blank";
    
        text2.position(0, 2);
        text2.width(size.cols);
        text2.settext(ss.str());

        ss.str("");
        ss << "Total LOC: " << stat.lines_code << " blank: " << stat.lines_blank << " files: " << stat.files;

        text3.position(1, size.rows - 3);
        text3.width(size.cols-1);
        text3.settext(ss.str());

        ss.str("");
        ss << "Sort by: n) name s) size c) code b) blank - q) to quit - e) to export";
    
        text4.position(1, size.rows - 1);
        text4.width(size.cols-1);
        text4.settext(ss.str());

        wnd.add(&list);
        wnd.add(&text1);
        wnd.add(&text2);
        wnd.add(&text3);
        wnd.add(&text4);
        wnd.show();
        wnd.invalidate();

        bool loop = true;
        bool help = true;
        while (loop)
        {
            int ch = term_get_key();
            int vk = map_input(ch);
            if (!help)
            {
                text4.settext(ss.str());
                wnd.update(&text4);
            }
            switch (vk)
            {
                case VK_EXIT_APPLICATION:
                    loop = false;
                    break;
                case VK_SORT_BY_NAME:
                    sort(files.begin(), files.end(), sort_by_name);
                    list.selpos(0);
                    wnd.update(&list);
                    break;
                case VK_SORT_BY_SIZE:
                    sort(files.begin(), files.end(), sort_by_size);
                    list.selpos(0);
                    wnd.update(&list);
                    break;
                case VK_SORT_BY_CODE:
                    sort(files.begin(), files.end(), sort_by_code);
                    list.selpos(0);
                    wnd.update(&list);
                    break;
                case VK_SORT_BY_BLANK:
                    sort(files.begin(), files.end(), sort_by_blank);
                    list.selpos(0);
                    wnd.update(&list);
                    break;
                case VK_EXPORT:
                    switch (export_html(files, "export.html", stat, include, exclude))
                    {
                        case EXPORT_SUCCESS:
                            text4.settext("Export done. Wrote export.html");
                            wnd.update(&text4);
                            break;
                        case EXPORT_FAIL:
                        case EXPORT_FAIL_FILE_EXISTS:
                            text4.settext("Export failed.");
                            wnd.update(&text4);
                            break;
                    }
                    help = false;
                    break;
                default:
                    wnd.keydown(0, vk);
                    break;
            }
        }

        term_uninit();
    }
    catch (const std::exception& e)
    {
        cerr << e.what();
        return 1;
    }
    return 0;
}


