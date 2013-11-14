/* cxxcam - C++ CAD/CAM driver library.
 * Copyright (C) 2013  Nicholas Gill
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * listwriter.cpp
 *
 *  Created on: 2013-10-19
 *      Author: nicholas
 */

#include "listwriter.h"
#include "cmake/cmake.h"
#include <string>
#include <ostream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace cxxide
{
namespace cmake
{
namespace config
{

namespace
{

void write_package(std::ostream& os, const std::string& package)
{
    os << "FIND_PACKAGE( " << package << " )\n";
}
void write_definitions(std::ostream& os, const std::vector<std::string>& definitions)
{
    os << "ADD_DEFINITIONS( ";
    for(auto& define : definitions)
        os << define << " ";
    os << ")\n";
}
void write_includes(std::ostream& os, const std::vector<std::string>& includes)
{
    os << "INCLUDE_DIRECTORIES( ";
    for(auto& include : includes)
        os << include << " ";
    os << ")\n";
}

bool directory_empty(const directory_t& directory)
{
    return
        directory.definitions.empty() &&
        directory.includes.empty() &&
        directory.compile_flags.cxx.empty() &&
        directory.compile_flags.c.empty() &&
        directory.configure_files.empty() &&
        directory.subdirectories.empty();
}
void write_directory(std::ostream& os, const directory_t& directory)
{
    if(!directory.definitions.empty())
        write_definitions(os, directory.definitions);

    if(!directory.includes.empty())
        write_includes(os, directory.includes);

    if(!directory.compile_flags.cxx.empty())
        os << "SET( CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} " << directory.compile_flags.cxx << "\" )\n";

    if(!directory.compile_flags.c.empty())
        os << "SET( CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} " << directory.compile_flags.c << "\" )\n";

    for(auto& file : directory.configure_files)
        os << "CONFIGURE_FILE( " << file.input << " " << file.output << " )\n";

    for(auto& subdir : directory.subdirectories)
        os << "ADD_SUBDIRECTORY( " << subdir.first << " )\n";
}

void write_file(std::ostream& os, const file_t& file)
{
    if(!file.definitions.empty())
    {
        os << "SET_PROPERTY( SOURCE " << file.name << " APPEND PROPERTY COMPILE_DEFINITIONS ";
        for(auto define : file.definitions)
        {
            if(define.find("-D") == 0)
                define = define.substr(2);
            os << define << " ";
        }
        os << ")\n";
    }
    if(!file.compile_flags.empty())
        os << "SET_PROPERTY( SOURCE " << file.name << " APPEND_STRING PROPERTY COMPILE_FLAGS \" " << file.compile_flags << "\" )\n";
}

void write_target(std::ostream& os, const target_t& target)
{
    std::string target_var = target.name;
    boost::to_upper(target_var);
    
    if(!target.sources.empty())
    {
        os << "SET( " << target_var << "_SOURCES ";
        for(auto& source : target.sources)
            os << "\n    " << source << " ";
        os << ")\n";
    }
    
    switch(target.type)
    {
        case target_t::executable:
            os << "ADD_EXECUTABLE( " << target.name << " ${" << target_var << "_SOURCES} )\n";
            break;
        case target_t::shared_library:
            os << "ADD_LIBRARY( " << target.name << " SHARED ${" << target_var << "_SOURCES} )\n";
            break;
        case target_t::static_library:
            os << "ADD_LIBRARY( " << target.name << " STATIC ${" << target_var << "_SOURCES} )\n";
            break;
    }

    if(!target.label.empty())
        os << "SET_PROPERTY( TARGET " << target.name << " PROPERTY PROJECT_LABEL \"" << target.label << "\" )\n";
    
    if(!target.version.empty())
    {
        os << "SET( " << target_var << "_MAJOR_VERSION " << target.version.major << " )\n";
        os << "SET( " << target_var << "_MINOR_VERSION " << target.version.minor << " )\n";
        os << "SET( " << target_var << "_PATCH_VERSION " << target.version.patch << " )\n";
        os << "SET( " << target_var << "_VERSION ${" << target_var << "_MAJOR_VERSION}.${" << target_var << "_MINOR_VERSION}.${" << target_var << "_PATCH_VERSION} )\n";
        os << "SET_PROPERTY( TARGET " << target.name << " PROPERTY VERSION ${" << target_var << "_VERSION} )\n";
        if(target.type == target_t::shared_library)
            os << "SET_PROPERTY( TARGET " << target.name << " PROPERTY SOVERSION ${" << target_var << "_MAJOR_VERSION} )\n";
    }

    if(!target.packages.empty())
    {
        os << "SET_TARGET_PACKAGES( TARGET " << target.name << " PACKAGES ";
        for(auto& package : target.packages)
            os << package << " ";
        os << ")\n";
    }
    
    if(!target.compile_flags.empty())
        os << "SET_PROPERTY( TARGET " << target.name << " APPEND_STRING PROPERTY COMPILE_FLAGS \" " << target.compile_flags << "\" )\n";
    
    if(!target.definitions.empty())
    {
        os << "SET_PROPERTY( TARGET " << target.name << " APPEND PROPERTY COMPILE_DEFINITIONS ";
        for(auto define : target.definitions)
        {
            if(define.find("-D") == 0)
                define = define.substr(2);
            os << define << " ";
        }
        os << ")\n";
    }
    
    if(!target.includes.empty())
    {
        os << "SET_PROPERTY( TARGET " << target.name << " APPEND PROPERTY INCLUDE_DIRECTORIES ";
        for(auto& include : target.includes)
            os << "\n    " << include << " ";
        os << ")\n";
    }
    
    if(!target.link_flags.empty())
        os << "SET_PROPERTY( TARGET " << target.name << " APPEND_STRING PROPERTY LINK_FLAGS \" " << target.link_flags << "\" )\n";
    
    if(!target.libs.empty())
    {
        os << "SET_TARGET_LIBRARIES( TARGET " << target.name << " LIBS ";
        for(auto& lib : target.libs)
            os << lib << " ";
        os << ")\n";
    }
    if(!target.depends.empty())
    {
        os << "ADD_DEPENDENCIES( " << target.name << " ";
        for(auto& dep : target.depends)
            os << dep << " ";
        os << ")\n";
    }
}

}

list_rewriter_t::list_rewriter_t(std::ostream& os, const configuration_t* config, const directory_t& directory)
 : os(os), config(config), directory(directory), is_managed(false), skip(false),
   wrote_packages(false), wrote_directory(false), wrote_file(false)
{
}

bool list_rewriter_t::managed() const
{
    return is_managed;
}

void list_rewriter_t::whitespace(const char* c, const char* end)
{
    if(skip) return;
    os << std::string(c, end);
}
void list_rewriter_t::comment(const char* c, const char* end)
{
    auto cmt = std::string(c, end);
    
    if(cmt.find("#<< ") == 0)
    {
        if(config && cmt == "#<< Managed Configuration >>##")
        {
            is_managed = true;
        }
        else if(config && cmt == "#<< Referenced Packages >>##")
        {
            /* An alternative exit point for this function.
             * If there are no packages, do not write
             * the header nor any of the content. */
            if(config->packages.empty())
            {
                if(skip) os << "\n";
                skip = !skip;
                return;
            }

            if(skip)
            {
                wrote_packages = true;

                os << "\n";
                for(auto& package : config->packages)
                    write_package(os, package);
            }
            skip = !skip;
        }
        else if(cmt == "#<< Directory Properties >>##")
        {
            /* An alternative exit point for this function.
             * If the directory is empty, do not write
             * the header nor any of the content. */
            if(directory_empty(directory))
            {
                if(skip) os << "\n";
                skip = !skip;
                return;
            }

            if(skip)
            {
                wrote_directory = true;
                
                os << "\n";
                write_directory(os, directory);
            }
            skip = !skip;
        }
        else if(cmt == "#<< File Properties >>##")
        {
            /* An alternative exit point for this function.
             * If there are no files to write, do not write
             * the header nor any of the content. */
            if(directory.files.empty())
            {
                if(skip) os << "\n";
                skip = !skip;
                return;
            }

            if(skip)
            {
                wrote_file = true;

                os << "\n";
                for(auto& file : directory.files)
                    write_file(os, file);
            }
            skip = !skip;
        }
        else if(boost::starts_with(cmt, "#<< Target ") && boost::ends_with(cmt, " Properties >>##"))
        {
            std::string target_name = cmt;
            boost::replace_first(target_name, "#<< Target ", "");
            boost::replace_last(target_name, " Properties >>##", "");
            
            auto target_exists = [this](const std::string& name) -> bool
            {
                for(auto& target : directory.targets)
                    if(target.name == name) return true;
                return false;
            };
            auto get_target = [this](const std::string& name) -> const target_t&
            {
                for(const auto& target : directory.targets)
                    if(target.name == name) return target;
                throw std::logic_error("target not found");
            };
            
            /* An alternative exit point for this function.
             * If the named target does not exist, do not write 
             * the header nor any of the content. */
            if(!target_exists(target_name))
            {
                if(skip) os << "\n";
                skip = !skip;
                return;
            }
            
            if(skip)
            {
                written_targets.insert(target_name);

                os << "\n";
                auto target = get_target(target_name);
                write_target(os, target);
            }
            skip = !skip;
        }
        
        os << '#' << cmt;
    }
    else
    {
        if(skip) return;
        os << '#' << cmt;
    }
}
void list_rewriter_t::begin_command(const char* c, const char* end)
{
    if(skip) return;
    os << std::string(c, end);
}
void list_rewriter_t::open_bracket()
{
    if(skip) return;
    os << '(';
}
void list_rewriter_t::close_bracket()
{
    if(skip) return;
    os << ')';
}
void list_rewriter_t::argument(const char* c, const char* end, bool quoted)
{
    if(skip) return;
    
    if(quoted)
        os << '"' << std::string(c, end) << '"';
    else
        os << std::string(c, end);
}
void list_rewriter_t::end_command()
{
    if(skip) return;
    // ni
}
void list_rewriter_t::eof()
{
    if(config && !wrote_packages && !config->packages.empty())
    {
        os << "\n";
        os << "##<< Referenced Packages >>##\n";
        for(auto& package : config->packages)
            write_package(os, package);
        os << "##<< Referenced Packages >>##\n";
    }
    wrote_packages = false;


    if(!wrote_directory && !directory_empty(directory))
    {
        os << "\n";
        os << "##<< Directory Properties >>##\n";
        write_directory(os, directory);
        os << "##<< Directory Properties >>##\n";
    }
    wrote_directory = false;


    if(!wrote_file && !directory.files.empty())
    {
        os << "\n";
        os << "##<< File Properties >>##\n";
        for(auto& file : directory.files)
            write_file(os, file);
        os << "##<< File Properties >>##\n";
    }
    wrote_file = false;


    auto target_written = [this](const std::string& name) -> bool
    {
        return written_targets.find(name) != end(written_targets);
    };
    for(auto& target : directory.targets)
    {
        if(!target_written(target.name))
        {
            os << "\n";
            os << "##<< Target " << target.name << " Properties >>##\n";
            write_target(os, target);
            os << "##<< Target " << target.name << " Properties >>##\n";
        }
    }
    written_targets.clear();
}

}
}
}

