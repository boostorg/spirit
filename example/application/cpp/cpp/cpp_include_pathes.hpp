/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_INCLUDE_PATHES_HPP__AF620DA4_B3D2_4221_AD91_8A1ABFFB6944__INCLUDED_)
#define _CPP_INCLUDE_PATHES_HPP__AF620DA4_B3D2_4221_AD91_8A1ABFFB6944__INCLUDED_

#include <string>
#include <list>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace util {

///////////////////////////////////////////////////////////////////////////////
//
//  include_pathes - controlling the include path search order
//
//  General notes:
//
//      Any directories specified with the 'add_include_path()' function before 
//      the function 'set_sys_include_delimiter()' is called are searched only 
//      for the case of '#include "file"' directives, they are not searched for 
//      '#include <file>' directives. If additional directories are specified 
//      with the 'add_include_path()' function after a call to the function 
//      'set_sys_include_delimiter()', these directories are searched for all 
//      '#include' directives. 
//
//      In addition, a call to the function 'set_sys_include_delimiter()' 
//      inhibits the use of the current directory as the first search directory 
//      for '#include "file"' directives. Therefore, the current directory is 
//      searched only if it is requested explicitly with a call to the function
//      'add_include_path(".")'. 
//
//      Callig both functions, the 'set_sys_include_delimiter()' and 
//      'add_include_path(".")' allows you to control precisely which 
//      directories are searched before the current one and which are searched 
//      after.
//
///////////////////////////////////////////////////////////////////////////////
class include_pathes
{
    typedef std::list<boost::filesystem::path> include_list_t;

public:
    include_pathes()
    :   was_sys_include_path(false),
        current_dir(boost::filesystem::initial_path())
    {}
    
    bool add_include_path(char const *path_, bool is_system = false)
    {
        return add_include_path(path_, (is_system || was_sys_include_path) ? 
            system_include_pathes : user_include_pathes);
    }
    void set_sys_include_delimiter() { was_sys_include_path = true; }
    bool find_include_file (std::string &s, bool is_system) const;
    void set_current_directory(char const *path_);

protected:
    bool find_include_file (std::string &s, include_list_t const &pathes) const;
    bool add_include_path(char const *path_, include_list_t &pathes_);

private:
    include_list_t user_include_pathes;
    include_list_t system_include_pathes;
    bool was_sys_include_path;          // saw a set_sys_include_delimiter()
    boost::filesystem::path current_dir;
};

///////////////////////////////////////////////////////////////////////////////
//  Add an include path to one of the search lists (user include path or system 
//  include path).
inline
bool include_pathes::add_include_path (
    char const *path_, include_list_t &pathes_)
{
    namespace fs = boost::filesystem;
    if (path_) {
    fs::path newpath = fs::complete(fs::path(path_, fs::native), current_dir);

        if (!fs::is_directory(newpath)) 
            return false;

        pathes_.push_back (newpath);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//  Find an include file by traversing the list of include directories
inline
bool include_pathes::find_include_file (std::string &s, 
    include_list_t const &pathes) const
{
    namespace fs = boost::filesystem;
    typedef include_list_t::const_iterator const_include_list_iter_t;
    
    const_include_list_iter_t include_pathes_end = pathes.end();
    for (const_include_list_iter_t it = pathes.begin(); 
         it != include_pathes_end; ++it)
    {
        fs::path currpath (*it);
        currpath /= s;      // append filename

        if (fs::exists(currpath)) {
        // found the required file
            s = currpath.string();
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//  Find an include file by searching the user and system includes in the 
//  correct sequence (as it was configured by the user of the C preprocessor
inline
bool include_pathes::find_include_file (std::string &s, bool is_system) const
{
    namespace fs = boost::filesystem;
    
// if not system include (<...>), then search actual directory first
    if (!is_system) {
        if (!was_sys_include_path) {  // set_sys_include_delimiter() not called
        // first look in the current directory
            fs::path currpath = current_dir;
            currpath /= s;
            if (fs::exists(currpath)) {
                s = currpath.string();    // found in local directory
                return true;
            }   

        // iterate all user include file directories to find the file
            return find_include_file(s, user_include_pathes);
        }

    // iterate all user include file directories to find the file
        if (find_include_file(s, user_include_pathes))
            return true;

    // if nothing found, fall through
    // ...
    }

// iterate all system include file directories to find the file
    return find_include_file (s, system_include_pathes);
}

///////////////////////////////////////////////////////////////////////////////
//  Set current directory from a given file name

inline
void include_pathes::set_current_directory(char const *path_)
{
    namespace fs = boost::filesystem;
    
    fs::path filename(path_, fs::native);
    if (fs::is_directory(filename)) 
        current_dir = filename;
    else
        current_dir = filename.branch_path();
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace cpp

#endif // !defined(_CPP_INCLUDE_PATHES_HPP__AF620DA4_B3D2_4221_AD91_8A1ABFFB6944__INCLUDED_)
