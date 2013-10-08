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
 * exec.cpp
 *
 *  Created on: 2013-10-08
 *      Author: nicholas
 */

#include "exec.h"

namespace system
{

int exec(const std::string& cwd, const std::vector<std::string>& args, const std::string& in, std::string* out)
{
    std::vector<const char*> argv;
    
    for(auto& arg : args)
        argv.push_back(arg.c_str());
    argv.push_back(nullptr);
    
    int infd[2];
    int outfd[2];
    pipe(infd);
    pipe(outfd);
    
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
        {
            throw std::runtime_error("fork");
        }
        case 0:
        {
            close(infd[1]);
            close(outfd[0]);

            dup2(infd[0], 0);
            dup2(outfd[1], 1);
            dup2(outfd[1], 2);

            close(infd[0]);
            close(outfd[1]);
            
            if(!cwd.empty())
            {
                int err = chdir(cwd.c_str());
                if(err)
                    throw std::runtime_error("chdir");
            }

            
            execvp(argv[0].c_str(), argv.data());
            throw std::runtime_error("execvpe");
        }
        default:
        {
            close(infd[0]);
            close(outfd[1]);
            
            // TODO write to stdin
            write(infd[1], in.c_str(), in.size());
            close(infd[1]);
            
            // TODO read from stdout
            char buffer[2048];
            while()
            close(outfd[0]);
            
            int status;
            int err = waitpid(pid, &status, 0);
            if(err)
                throw std::runtime_error("waitpid");
            
            if(WIFEXITED(status))
                return WEXITSTATUS(status);
            throw std::runtime_error("Failed to exec child.");
        }
    }
    throw std::logic_error("exec");
}

}
