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
 * git.h
 *
 *  Created on: 2013-10-04
 *      Author: nicholas
 
 create folder
git init
create CMakeLists.txt
local clang components and scan folder.
git add CMakeLists.txt
git commit -m Initial
create build folder
cmake source_path -GNinja

 */

#ifndef DVCSGIT_H_
#define DVCSGIT_H_
#include <string>

namespace git
{

class repo_t
{
    open();
    init();
    clone();
    
    push();
    pull();
    commit();
    branch();
    merge();
};

repo_t init(const std::string& path);

cat-file
commit-tree
count-objects
diff-index
hash-object
merge-base
read-tree
rev-list
rev-parse
show-ref
symbolic-ref
update-index
update-ref
verify-pack
write-tree

}

#endif /* DVCSGIT_H_ */
