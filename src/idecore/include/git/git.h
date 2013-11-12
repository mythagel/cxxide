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
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace cxxide
{
namespace git
{

struct error : std::runtime_error
{
    error(const std::string& what);
    virtual ~error() noexcept;
};

struct repo_t
{
    boost::filesystem::path root;
//    open();
//    clone();
//    
//    push();
//    pull();
//    commit();
//    branch();
//    merge();

    struct index_t
    {
        // git update-index --add -q -z --stdin < files
//        void add(const std::vector<std::string>& files);
//        // git update-index -q -z --stdin < files
//        void update(const std::vector<std::string>& files);
//        // git update-index --remove -q -z --stdin < files
//        void remove(const std::vector<std::string>& files);
//        
//        // tree=$(git write-tree)
//        // commit=$(echo commit_msg | git commit-tree $tree)
//        // if HEAD not valid ref
//        // git show-ref --verify `git symbolic-ref -q HEAD`
//        // git update-ref HEAD $commit
//        void commit();
    } index;
};

struct init_opts
{
    std::string template_dir;
    bool bare;
    /*struct shared_t
    {
        enum
        {
            false_,
            true_,
            umask,
            group,
            all,
            world,
            everybody,
            octal
        } shared;
        int permissions;
        
        shared_t()
         : shared(false_), permissions()
        {
        }
    } shared;*/
    std::string separate_git_dir;
    
    init_opts()
     : template_dir(), bare()/*, shared()*/, separate_git_dir()
    {
    }
};
repo_t init(const boost::filesystem::path& path, const init_opts& opts = {});
repo_t open(const boost::filesystem::path& path);


//cat-file
//commit-tree
//count-objects
//diff-index
//hash-object
//merge-base
//read-tree
//rev-list
//rev-parse
//show-ref
//symbolic-ref
//update-index
//update-ref
//verify-pack
//write-tree

}
}

#endif /* DVCSGIT_H_ */
