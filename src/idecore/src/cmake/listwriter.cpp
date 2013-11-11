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

namespace cxxide
{
namespace cmake
{

list_rewriter_t::list_rewriter_t(std::ostream& os, const configuration_t* config, const directory_t& directory)
 : os(os), config(config), directory(directory), is_managed(false), skip(false)
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
            if(skip)
            {
                os << "\n";
                for(auto& package : config->packages)
                    os << "FIND_PACKAGE( " << package << " )\n";
            }
            skip = !skip;
        }
        else if(cmt == "#<< Directory Properties >>##")
        {
            if(skip)
            {
                os << "\n";
                
                if(!directory.definitions.empty())
                {
                    os << "ADD_DEFINITIONS( ";
                    for(auto& define : directory.definitions)
                        os << define << " ";
                    os << ")\n";
                }

                if(!directory.includes.empty())
                {
                    os << "INCLUDE_DIRECTORIES( ";
                    for(auto& include : directory.includes)
                        os << include << " ";
                    os << ")\n";
                }
                
                if(!directory.compile_flags.cxx.empty())
                    os << "SET( CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} " << directory.compile_flags.cxx << "\" )\n";

                if(!directory.compile_flags.c.empty())
                    os << "SET( CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} " << directory.compile_flags.c << "\" )\n";

                for(auto& file : directory.configure_files)
                    os << "CONFIGURE_FILE( " << file.input << " " << file.output << " )\n";

                for(auto& subdir : directory.subdirectories)
                    os << "ADD_SUBDIRECTORY( " << subdir.first << " )\n";
                
            }
            skip = !skip;
        }
        else if(cmt == "#<< File Properties >>##")
        {
            if(skip)
            {
                os << "\n";
                for(auto& file : directory.files)
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
            }
            skip = !skip;
        }
        else if(cmt == "#<< Target Properties >>##")
        {
            if(skip)
            {
                os << "\n";
                /*
                TODO write targets in individual sections
                so they can be made conditional etc.
                */
                for(auto& target : directory.targets)
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
}

}
}

