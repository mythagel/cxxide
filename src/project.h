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

class project_t
{
    // vcs
    push();
    pull();
    commit();
    branch();
    merge();
    
    // configuration
    configure();
    build();
    
    // stats
    
};

/*
 check path exists
 check path/name does NOT exist
 
 create folder path/name
 git init
 create cmake decl adapter file
 create CMakelists.txt file
*/
project_t create(const std::string& name, const std::string& source_path, const std::string& build_path);

/*
 open path/CMakelists.txt
 open git repo
*/
project_t open(const std::string& source_path, const std::string& build_path);

/*
 git clone url into path
 then open path.
*/
project_t clone(const std::string& url, const std::string& source_path, const std::string& build_path);

#endif /* PROJECT_H_ */
