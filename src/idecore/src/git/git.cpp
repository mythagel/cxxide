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
 * git.cpp
 *
 *  Created on: 2013-10-04
 *      Author: nicholas
 */

#include "git/git.h"
#include "system/exec.h"
#include <exception>

namespace fs = boost::filesystem;

namespace cxxide
{
namespace git
{

error::error(const std::string& what)
 : std::runtime_error(what)
{
}
error::~error() noexcept
{
}

auto base_opts(std::initializer_list<std::string> opts) -> std::vector<std::string>
{
    return opts;
}

repo_t init(const fs::path& path, const init_opts& opts)
{
    try
    {
        system::stream_t stream;
        
        auto args = base_opts({"git", "init", "-q"});
        auto add_if = [&args](bool cond, std::initializer_list<std::string> opts) -> void
        {
            if(cond) args.insert(end(args), opts);
        };
        add_if(!opts.template_dir.empty(), {"--template", opts.template_dir});
        add_if(opts.bare, {"--bare"});
        add_if(!opts.separate_git_dir.empty(), {"--separate-git-dir", opts.separate_git_dir});
        
        int err = system::exec(path.native(), args, &stream);
        if(err) throw error("git: " + stream.err);
        
        return { path, {} };
    }
    catch(...)
    {
        std::throw_with_nested(error("git::init failed"));
    }
}

repo_t open(const fs::path& path)
{
    try
    {
        system::stream_t stream;
        int err = system::exec(path.native(), {"git", "rev-parse", "--show-toplevel"}, &stream);
        if(err) throw error("git: " + stream.err);
        
        return { stream.out, {} };
    }
    catch(...)
    {
        std::throw_with_nested(error("git init failed"));
    }
}

}
}

