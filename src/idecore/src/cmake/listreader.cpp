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
 * listreader.cpp
 *
 *  Created on: 2013-10-19
 *      Author: nicholas
 */

#include "listreader.h"
#include "cmake/cmake.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace cxxide
{
namespace cmake
{
namespace config
{

list_reader_t::command_t::command_t()
 : type(UNKNOWN)
{
}

list_reader_t::command_t::command_t(std::string cmd_name)
 : type(UNKNOWN), name(cmd_name)
{
    boost::to_upper(cmd_name);
    
    if(cmd_name == "PROJECT")
        type = PROJECT;
    else if(cmd_name == "FIND_PACKAGE")
        type = FIND_PACKAGE;
    else if(cmd_name == "ADD_DEFINITIONS")
        type = ADD_DEFINITIONS;
    else if(cmd_name == "INCLUDE_DIRECTORIES")
        type = INCLUDE_DIRECTORIES;
    else if(cmd_name == "SET")
        type = SET;
    else if(cmd_name == "CONFIGURE_FILE")
        type = CONFIGURE_FILE;
    else if(cmd_name == "ADD_SUBDIRECTORY")
        type = ADD_SUBDIRECTORY;
    else if(cmd_name == "SET_PROPERTY")
        type = SET_PROPERTY;
    else if(cmd_name == "ADD_EXECUTABLE")
        type = ADD_EXECUTABLE;
    else if(cmd_name == "ADD_LIBRARY")
        type = ADD_LIBRARY;
    else if(cmd_name == "SET_TARGET_PACKAGES")
        type = SET_TARGET_PACKAGES;
    else if(cmd_name == "SET_TARGET_LIBRARIES")
        type = SET_TARGET_LIBRARIES;
    else if(cmd_name == "ADD_DEPENDENCIES")
        type = ADD_DEPENDENCIES;
}

list_reader_t::list_reader_t(configuration_t* config, directory_t* directory)
 : config(config), directory(directory), is_managed(false), interpret(false), section(section_Directory), nesting(0)
{
}

bool list_reader_t::managed() const
{
    return is_managed;
}

void list_reader_t::whitespace(const char*, const char*)
{
}
void list_reader_t::comment(const char* c, const char* end)
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
            interpret = !interpret;
            section = section_Packages;
        }
        else if(cmt == "#<< Directory Properties >>##")
        {
            interpret = !interpret;
            section = section_Directory;
        }
        else if(cmt == "#<< File Properties >>##")
        {
            interpret = !interpret;
            section = section_File;
        }
        else if(boost::starts_with(cmt, "#<< Target ") && boost::ends_with(cmt, " Properties >>##"))
        {
            interpret = !interpret;
            section = section_Target;
        }
    }
}
void list_reader_t::begin_command(const char* c, const char* end)
{
    command = command_t{{c, end}};
    nesting = 0;
}
void list_reader_t::open_bracket()
{
    if(nesting) command.args.push_back("(");
    ++nesting;
}
void list_reader_t::close_bracket()
{
    --nesting;
    if(nesting) command.args.push_back(")");
}
void list_reader_t::argument(const char* c, const char* end, bool /*quoted*/)
{
    command.args.push_back({c, end});
}
void list_reader_t::end_command()
{
    if(!interpret)
    {
        if(command.type == command_t::PROJECT)
        {
            if(!command.args.empty() && config)
                config->name = command.args[0];
        }
        return;
    }
    
    if(command.type == command_t::UNKNOWN)
        throw error("Unrecognised command " + command.name);
    
    switch(section)
    {
        case section_Packages:
        {
            if(command.type != command_t::FIND_PACKAGE)
                throw error(std::string("Unexpected command in Managed Packages: ") + command.name);
            
            if(command.args.size() != 1)
                throw error("Unexpected arguments to FIND_PACKAGE command in Managed Packages");
            
            // Safe to dereference config here because section_Package is only set above
            // If config != nullptr
            config->packages.insert(command.args[0]);
            break;
        }
        case section_Directory:
        {
            switch(command.type)
            {
                case command_t::ADD_DEFINITIONS:
                {
                    for(auto& define : command.args)
                        directory->definitions.push_back(define);
                    break;
                }
                case command_t::INCLUDE_DIRECTORIES:
                {
                    for(auto& include : command.args)
                        directory->includes.push_back(include);
                    break;
                }
                case command_t::SET:
                {
                    if(command.args.size() != 2)
                        throw error("Unexpected arguments to SET command in Directory Properties");
                    if(command.args[0] == "CMAKE_CXX_FLAGS")
                    {
                        if(command.args[1].find("${CMAKE_CXX_FLAGS}") != 0)
                            throw error("Unexpected argument format to SET command in Directory Properties");
                        
                        auto flags = command.args[1].substr(18);
                        if(!flags.empty() && flags[0] == ' ')
                            flags = flags.substr(1);
                        directory->compile_flags.cxx = flags;
                    }
                    else if(command.args[0] == "CMAKE_C_FLAGS")
                    {
                        if(command.args[1].find("${CMAKE_C_FLAGS}") != 0)
                            throw error("Unexpected argument format to SET command in Directory Properties");
                        
                        auto flags = command.args[1].substr(16);
                        if(!flags.empty() && flags[0] == ' ')
                            flags = flags.substr(1);
                        directory->compile_flags.c = flags;
                    }
                    else
                    {
                        throw error("Unexpected variable to SET command in Directory Properties");
                    }
                    break;
                }
                case command_t::CONFIGURE_FILE:
                {
                    if(command.args.size() != 2)
                        throw error("Unexpected arguments to CONFIGURE_FILE command in Directory Properties");
                    
                    directory_t::configure_file_t configure_file{command.args[0], command.args[1]};
                    directory->configure_files.push_back(configure_file);
                    
                    break;
                }
                case command_t::ADD_SUBDIRECTORY:
                {
                    if(command.args.size() != 1)
                        throw error("Unexpected arguments to ADD_SUBDIRECTORY command in Directory Properties");
                    
                    directory->subdirectories.push_back(std::make_pair(command.args[0], directory_t()));
                    
                    break;
                }
                default:
                    throw error(std::string("Unexpected command in Directory Properties: ") + command.name);
            }
            break;
        }
        case section_File:
        {
            if(command.type != command_t::SET_PROPERTY)
                throw error(std::string("Unexpected command in File Properties: ") + command.name);
            
            if(command.args.size() < 6)
                throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
            
            if(command.args[0] != "SOURCE")
                throw error("Unexpected arguments to SET_PROPERTY command in File Properties");

            auto filename = command.args[1];
            
            file_t* file = nullptr;
            for(auto& fl : directory->files)
            {
                if(fl.name == filename)
                {
                    file = &fl;
                    break;
                }
            }
            if(!file)
            {
                directory->files.emplace_back();
                file = &directory->files.back();
                file->name = filename;
            }
            
            if(command.args[2] != "APPEND" && command.args[2] != "APPEND_STRING")
                throw error("Unexpected arguments to SET_PROPERTY command in File Properties");

            if(command.args[3] != "PROPERTY")
                throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
            
            if(command.args[4] == "COMPILE_DEFINITIONS")
            {
                for(size_t i = 5; i < command.args.size(); ++i)
                {
                    auto define = command.args[i];
                    if(!define.empty() && define[0] != '-')
                        define = "-D" + define;
                    file->definitions.push_back(define);
                }
            }
            else if(command.args[4] == "COMPILE_FLAGS")
            {
                file->compile_flags = command.args[5];
                if(!file->compile_flags.empty() && file->compile_flags[0] == ' ')
                    file->compile_flags = file->compile_flags.substr(1);
            }
            else
                throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
            
            break;
        }
        case section_Target:
        {
            auto target_nx = [this](std::string name) -> target_t*
            {
                target_t* target = nullptr;
                for(auto& t : directory->targets)
                {
                    if(t.name == name)
                        return &t;
                }
                
                boost::to_upper(name);
                if(!target)
                {
                    for(auto& t : directory->targets)
                    {
                        auto tname = t.name;
                        boost::to_upper(tname);
                        if(tname == name)
                            return &t;
                    }
                }
                
                target_t t;
                t.name = name;   // Uppercased
                directory->targets.push_back(t);
                
                return &directory->targets.back();
            };
            switch(command.type)
            {
                case command_t::SET:
                {
                    if(command.args.size() < 2)
                        throw error("Unexpected arguments to SET command in Target Properties");
                    
                    auto var = command.args[0];
                    auto pos = var.find('_');
                    if(pos == std::string::npos)
                        throw error("Unexpected variable to SET command in Target Properties");
                    auto target_name = var.substr(0, pos);
                    var = var.substr(pos);
                    
                    auto target = target_nx(target_name);
                    
                    if(var == "_SOURCES")
                        for(size_t i = 1; i < command.args.size(); ++i)
                            target->sources.push_back(command.args[i]);
                    else if(var == "_MAJOR_VERSION")
                        target->version.major = atoi(command.args[1].c_str());
                    else if(var == "_MINOR_VERSION")
                        target->version.minor = atoi(command.args[1].c_str());
                    else if(var == "_PATCH_VERSION")
                        target->version.patch = atoi(command.args[1].c_str());
                    else if(var == "_VERSION")
                        ; // Generated
                    else
                        throw error("Unexpected variable to SET command in Target Properties");
                    
                    break;
                }
                case command_t::ADD_EXECUTABLE:
                {
                    if(command.args.size() != 2)
                        throw error("Unexpected arguments to ADD_EXECUTABLE command in Target Properties");
                    
                    auto target_name = command.args[0];
                    
                    auto target = target_nx(target_name);
                    
                    target->name = target_name;
                    target->type = target_t::executable;
                    break;
                }
                case command_t::ADD_LIBRARY:
                {
                    if(command.args.size() != 3)
                        throw error("Unexpected arguments to ADD_LIBRARY command in Target Properties");
                    
                    auto target_name = command.args[0];
                    auto lib_type = command.args[1];
                    
                    auto target = target_nx(target_name);
                    
                    if(lib_type == "SHARED")
                        target->type = target_t::shared_library;
                    else if(lib_type == "STATIC")
                        target->type = target_t::static_library;
                    else
                        throw error(std::string("Unexpected library type in Target Properties: ") + lib_type);
                    
                    target->name = target_name;
                    break;
                }
                case command_t::SET_TARGET_PACKAGES:
                {
                    if(command.args.size() < 4)
                        throw error("Unexpected arguments to SET_TARGET_PACKAGES command in Target Properties");
                    
                    if(command.args[0] != "TARGET")
                        throw error("Unexpected arguments to SET_TARGET_PACKAGES command in Target Properties");
                    
                    auto target_name = command.args[1];
                    
                    auto target = target_nx(target_name);

                    if(command.args[2] != "PACKAGES")
                        throw error("Unexpected arguments to SET_TARGET_PACKAGES command in Target Properties");

                    for(size_t i = 3; i < command.args.size(); ++i)
                        target->packages.push_back(command.args[i]);
                    break;
                }
                case command_t::SET_PROPERTY:
                {
                    if(command.args.size() < 4)
                        throw error("Unexpected arguments to SET_PROPERTY command in Target Properties");
                    
                    if(command.args[0] != "TARGET")
                        throw error("Unexpected arguments to SET_PROPERTY command in Target Properties");
                    
                    auto target_name = command.args[1];
                    
                    auto target = target_nx(target_name);
                    
                    auto it = std::find(begin(command.args), end(command.args), "PROPERTY");
                    
                    if(it == end(command.args))
                        throw error("Unexpected arguments to SET_PROPERTY command in Target Properties");
                    
                    if(++it == end(command.args))
                        throw error("Unexpected arguments to SET_PROPERTY command in Target Properties");
                    
                    auto prop = *it;
                    
                    if(++it == end(command.args))
                        throw error("Unexpected arguments to SET_PROPERTY command in Target Properties");
                    
                    if(prop == "COMPILE_FLAGS")
                    {
                        auto flags = *it;
                        if(!flags.empty() && flags[0] == ' ')
                            flags = flags.substr(1);
                        target->compile_flags = flags;
                    }
                    else if(prop == "COMPILE_DEFINITIONS")
                    {
                        for(; it != end(command.args); ++it)
                        {
                            auto define = *it;
                            if(!define.empty() && define[0] != '-')
                                define = "-D" + define;
                            target->definitions.push_back(define);
                        }
                    }
                    else if(prop == "INCLUDE_DIRECTORIES")
                    {
                        for(; it != end(command.args); ++it)
                            target->includes.push_back(*it);
                    }
                    else if(prop == "PROJECT_LABEL")
                    {
                        target->label = *it;
                    }
                    else if(prop == "LINK_FLAGS")
                    {
                        auto flags = *it;
                        if(!flags.empty() && flags[0] == ' ')
                            flags = flags.substr(1);
                        target->link_flags = flags;
                    }
                    else if(prop == "VERSION")
                    {
                        // Generated from VERSION_*
                    }
                    else if(prop == "SOVERSION")
                    {
                        // Generated from VERSION_*
                    }
                    else
                        throw error(std::string("Unexpected property in Target Properties: ") + prop);
                    
                    break;
                }
                case command_t::SET_TARGET_LIBRARIES:
                {
                    if(command.args.size() < 4)
                        throw error("Unexpected arguments to SET_TARGET_LIBRARIES command in Target Properties");
                    
                    if(command.args[0] != "TARGET")
                        throw error("Unexpected arguments to SET_TARGET_LIBRARIES command in Target Properties");
                    
                    auto target_name = command.args[1];
                    
                    auto target = target_nx(target_name);

                    if(command.args[2] != "LIBS")
                        throw error("Unexpected arguments to SET_TARGET_LIBRARIES command in Target Properties");

                    for(size_t i = 3; i < command.args.size(); ++i)
                        target->libs.push_back(command.args[i]);
                    break;
                }
                case command_t::ADD_DEPENDENCIES:
                {
                    if(command.args.size() < 2)
                        throw error("Unexpected arguments to ADD_DEPENDENCIES command in Target Properties");
                    
                    auto target_name = command.args[0];
                    
                    auto target = target_nx(target_name);
                    
                    for(size_t i = 1; i < command.args.size(); ++i)
                        target->depends.push_back(command.args[i]);
                    break;
                }
                default:
                    throw error(std::string("Unexpected command in Directory Properties: ") + command.name);
            }
            break;
        }
    }
}
void list_reader_t::eof()
{
}

}
}
}

