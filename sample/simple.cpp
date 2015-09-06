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

#if defined(_WIN32)
#  define WINDOWS
#else
#  define LINUX
#endif

#include <cli/widgets.h>
#include <cli/terminal.h>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>

#ifdef WINDOWS
#  include <windows.h> // for directory API
#elif defined(LINUX)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <dirent.h>
#endif

enum { VK_EXIT_APPLICATION = cli::VK_SENTINEL + 1 };

// simple recursive file/folder object
struct file {
    std::string name;
    std::string attrib;
    int    size;
    bool   is_folder;
    std::vector<file*> files;
    file*  parent;
};

#ifdef LINUX
inline
std::string mask_str(int mode, int r, int w, int x)
{
    std::string str;
    str += mode & r ? "r" : "-";
    str += mode & w ? "w" : "-";
    str += mode & x ? "x" : "-";
    return str;
}
#endif

void build_dir_tree(const std::string& folder, file& root)
{
    // a simple function to build a directory tree starting at the specified folder
    // there's no error checking. just asserting. 
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
        
        file* f      = new file;
        f->name      = name;
        f->size      = static_cast<int>(fd.nFileSizeLow);
        f->is_folder = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
        f->parent    = &root;
        if (f->is_folder)
        {
            f->name += "/";
            f->size  = 0;
            if (f->name != "./" && f->name != "../")
                build_dir_tree(path, *f);
        }
        SYSTEMTIME syst = {};
        FileTimeToSystemTime(&fd.ftCreationTime, &syst);
        ss.str("");
        ss << syst.wDay << "/" << syst.wMonth << "/" << syst.wYear;
        f->attrib = ss.str();
        root.files.push_back(f);
    }

    BOOL ret = FindClose(dir);
    assert( ret == TRUE );
    ret = 0;
#elif defined(LINUX)
    DIR* dir = opendir(folder.c_str());
    assert(dir && "failed to open directory");
    
    while (true)
    {
        struct dirent* d = readdir(dir);
        if (d == NULL) 
            break;
        
        // the contents of what d points to might get overwritten
        // on subsequent calls to readdir
        std::string name = d->d_name;
        std::string path = folder + "/" + name;
        struct stat st = {};
        stat(path.c_str(), &st);
        
        // simply allocate memory on the free store. 
        // techinically this is not a leak since the allocated space
        // does not grow later on. And its used untill exit.
        file* f     = new file; 
        f->name      = d->d_name;
        f->size      = static_cast<int>(st.st_size);
        f->is_folder = st.st_mode  & S_IFDIR;
        f->parent    = &root;
        if (f->is_folder)
        {
            f->name += "/";
            f->size  = 0;
            if (f->name != "./" && f->name != "../")
                build_dir_tree(path, *f);
        }
        f->attrib += (st.st_mode & S_IFDIR) ? "d" : "-";
        f->attrib += mask_str(st.st_mode, S_IRUSR, S_IWUSR, S_IXUSR);
        f->attrib += mask_str(st.st_mode, S_IRGRP, S_IWGRP, S_IXGRP);
        f->attrib += mask_str(st.st_mode, S_IROTH, S_IWOTH, S_IXOTH);
        root.files.push_back(f);
    }
    
    closedir(dir);
#endif

}

// a data access policy for table widget. 
// the widgets do not own any data, instead they access it 
// on demand basis (random access) through a data access policy class.
class file_tree_data
{
public:
    file* root;
protected:
   ~file_tree_data() {}
    file_tree_data() : root(NULL) {}
    typedef file* value;
    
    class converter {
    public:
        converter(const value& val, int col)
        {
            switch (col)
            {
                case 0: str_ = val->attrib; break;
                case 1: str_ = val->name;   break;
                case 2:
                {
                    std::stringstream ss;
                    ss << val->size;
                    str_ = ss.str();
                }
                break;
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
        std::string str_;
    };

    // access a "row" of data at the specified index.
    void fetch(value& val, int index) const
    {
        assert(root);
        assert(index >= 0);
        assert(index < static_cast<int>(root->files.size()));
        val = root->files[index];
    }
    
    int size() const
    {
        return static_cast<int>(root->files.size());
    }

private:
    
    
};

int map_input(int ch)
{
    // do very simple input -> virtual key mapping
    // more realistic (and complex) input key -> virtual key mapping
    // is left as an "excercise for the reader". (;
    switch (ch)
    {
        case cli::TERM_MOVE_UP:   return cli::VK_MOVE_UP;
        case cli::TERM_MOVE_DOWN: return cli::VK_MOVE_DOWN;
        case 'n':  return cli::VK_MOVE_DOWN;
        case 'p':  return cli::VK_MOVE_UP;
        case '\t': return cli::VK_FOCUS_NEXT;
        case ' ':  return cli::VK_ACTION_SPACE;
        case 'q':  return VK_EXIT_APPLICATION;
    }
    return -1;

}

void select_tree_node(cli::basic_table<file_tree_data>* list, cli::window* wnd)
{
    int selpos = list->selpos();
    if (list->root->files.empty())
        return;

    assert(list->root);

    file* next = list->root->files.at(selpos);
    if (!next->is_folder)
        return;
    if (next->name == "./")
        return;
    
    if (next->name == "../")
    {
        if (list->root->parent == NULL)
            return;
        // go one step back
        list->root = list->root->parent;
    }
    else
    {
        // go one step forward
        list->root = next;

    }
    list->selpos(0);
    wnd->update(list);
}

void draw_window(cli::window* win, cli::buffer* fb)
{
    const cli::rect dirty = win->draw(*fb);
    // if (dirty.bottom == 0 || dirty.left == 0)
    //     return;

    cli::term_draw_buffer(*fb, dirty);
}

int main(int argc, const char* argv[])
{
    // current working directory
    std::string cwd;

#ifdef WINDOWS
    char foo[255] = {};
    DWORD ret = GetCurrentDirectory(sizeof(foo), foo);
    assert( ret > 0 && ret < sizeof(foo));
    cwd.append(foo, ret);
#elif defined(LINUX)
    cwd = ".";
#endif

    // get some data. 
    file root = {};
    build_dir_tree(cwd, root);


    // for any non-trivial Console application the
    // C/C++ standard stream stuff isnt going to be enough. 
    // so the first thing to do here, is to drop into raw mode, disable 
    // echo and line buffering. 
    cli::term_init();
    cli::term_init_colors();

    // figure out current terminal window size and adjust the frame
    // buffer accordingly
    cli::size size = cli::term_get_size();
    assert(size.cols && size.rows);

    // this is the primary frame buffer. We'll use this 
    // for intermediate widget output storage.
    cli::buffer framebuff;
    
    // make the screenbuffer cover the whole terminal area.
    framebuff.resize(size.rows, size.cols);

    // create a window object for managing some widgets.
    // there's no limit to how many windows there can be or
    // how many widgets they may contain.
    // in any "real" application one would probably combine
    // a window object along with some widgets (and some UI functionality)
    // into larger application specific component.
    cli::window wnd;
    // bind some interesting window events.
    // if draw event is not used then one can manually check
    // after some window operations if the window needs drawing with window::is_valid()
    wnd.evtdraw = std::bind(draw_window, std::placeholders::_1, &framebuff);
                                    

    
    // create a table widget for displaying the contents of the file hierarchy.
    cli::basic_table<file_tree_data> list;
    list.root = &root;
    list.addcol(10); // attrib
    list.addcol(25); // name
    list.addcol(10); // size
    list.cellspacing(2);
    list.position(0, 2);
    list.width(size.cols);
    list.height(size.rows - 5);
    list.evtselect = std::bind(select_tree_node, &list, &wnd);

    cli::text text1;
    cli::text text2;

    text1.position(0, 0);
    text1.width(size.cols);
    text1.settext(cwd + " press 'q' to exit sample 'tab' to change focus");
    text1.setattrib(cli::ATTRIB_UNDERLINE | cli::ATTRIB_BOLD);
    text2.position(0, size.rows-1);
    text2.width(size.cols);
    text2.settext("Text1");

    cli::button button1;
    cli::button button2;
    cli::button button3;
    
    button1.position(0, size.rows-2);
    button1.text("Delete");
    button2.position(10, size.rows-2);
    button2.text("Create");
    button3.position(20, size.rows-2);
    button3.text("Rename");
    button1.evtclick = std::bind(&cli::text::settext, &text2, "Delete clicked!");
    button2.evtclick = std::bind(&cli::text::settext, &text2, "Create clicked!");
    button3.evtclick = std::bind(&cli::text::settext, &text2, "Rename clicked!");

    // add widgets into the window
    wnd.add(&list);
    wnd.add(&button1);
    wnd.add(&button2);
    wnd.add(&button3);
    wnd.add(&text1);
    wnd.add(&text2);

    wnd.show();
    wnd.invalidate();
    
    // enter the application specific loop.
    // in this simple loop we only read some input (blocking while doing it)
    // and then process it immediately
    while (true)
    {
        int ch = cli::term_get_key();
        int vk = map_input(ch);
        if (vk == VK_EXIT_APPLICATION)
            break;
        
        // in this application we only have a single window
        // so, pass the input to that. In a more complex scenario
        // there could be a list or a stack of windows and the 
        // input woudld get passed to "topmost" window.
        wnd.keydown(ch, vk);
        
    }

    cli::term_uninit();
    return 0;
}
