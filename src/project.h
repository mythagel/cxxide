/* 
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
 * project.h
 *
 *  Created on: 2013-10-03
 *      Author: nicholas
 */

#ifndef PROJECT_H_
#define PROJECT_H_
#include <string>
#include "git.h"
#include "cmake.h"
#include <stdexcept>

namespace cxxide
{
namespace project
{

struct error : std::runtime_error
{
    error(const std::string& what);
    virtual ~error() noexcept;
};

class project_t
{
friend project_t create(const std::string& name, const std::string& path, const std::string& build_path);

private:
    std::string name;
    git::repo_t repo;
    cmake::project_t config;
    std::string build_path;
public:
//    // dvcs
//    push();
//    pull();
//    commit();
//    branch();
//    merge();
//    
//    // build
//    void configure();
//    void build();
//    
//    // code insight
    ~project_t();
};

/*
// check path exists
// check path/name does NOT exist
// 
// create folder path/name
// git init
// create cmake decl adapter file
// create CMakelists.txt file
//*/
project_t create(const std::string& name, const std::string& path, const std::string& build_path);

///*
// open path/CMakelists.txt
// open git repo
//*/
//project_t open(const std::string& source_path, const std::string& build_path);

///*
// git clone url into path
// then open path.
//*/
//project_t clone(const std::string& url, const std::string& source_path, const std::string& build_path);

}
}

#endif /* PROJECT_H_ */
