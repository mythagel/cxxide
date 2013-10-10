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
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>
#include <stdexcept>

namespace cxxide
{
namespace system
{

namespace
{

/*
 * Helper for exec functions. Note that returned vector is char* but
 * the contents must NOT be modified.
 */
std::vector<char*> convert_args(const std::vector<std::string>& args)
{
    std::vector<char*> argv;
    
    for(auto& arg : args)
        argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr);
    
    return argv;
}

}

int exec(const std::string& wd, const std::vector<std::string>& args)
{
    auto argv = convert_args(args);
    
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
        {
            throw std::system_error(errno, std::system_category(), "fork");
        }
        case 0:
        {
            if(!wd.empty())
            {
                int err = chdir(wd.c_str());
                if(err)
                {
                    std::string cmd;
                    cmd += "chdir(" + wd + ")";
                    perror(cmd.c_str());
                    abort();
                }
            }
            
            execvp(argv[0], argv.data());
            abort();
        }
        default:
        {
            int status;
            waitpid(pid, &status, 0);
            
            if(WIFEXITED(status))
                return WEXITSTATUS(status);
            
            throw std::runtime_error("Abnormal child termination.");
        }
    }
    
    throw std::logic_error("exec");
}
int exec(const std::string& wd, const std::vector<std::string>& args, stream_t* stream)
{
    auto argv = convert_args(args);
    
    int infd[2];
    int outfd[2];
    int errfd[2];
    pipe(infd);
    pipe(outfd);
    pipe(errfd);
    
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
        {
            throw std::system_error(errno, std::system_category(), "fork");
        }
        case 0:
        {
            close(infd[1]);
            close(outfd[0]);
            close(errfd[0]);

            dup2(infd[0], 0);
            dup2(outfd[1], 1);
            dup2(errfd[1], 2);

            close(infd[0]);
            close(outfd[1]);
            close(errfd[1]);
            
            if(!wd.empty())
            {
                int err = chdir(wd.c_str());
                if(err)
                {
                    std::string cmd;
                    cmd += "chdir(" + wd + ")";
                    perror(cmd.c_str());
                    abort();
                }
            }
            
            execvp(argv[0], argv.data());
            abort();
        }
        default:
        {
            close(infd[0]);
            close(outfd[1]);
            close(errfd[1]);
            
            write(infd[1], stream->in.c_str(), stream->in.size());
            close(infd[1]);
            
            char buffer[2048];
            while(true)
            {
                ssize_t n = read(outfd[0], buffer, sizeof(buffer));
                if(n == 0) break;
                if(n == -1) break; // todo How to signal partial read?
                stream->out.insert(stream->out.end(), buffer, buffer + n);
            }
            close(outfd[0]);
            
            while(true)
            {
                ssize_t n = read(errfd[0], buffer, sizeof(buffer));
                if(n == 0) break;
                if(n == -1) break; // todo How to signal partial read?
                stream->err.insert(stream->err.end(), buffer, buffer + n);
            }
            close(errfd[0]);
            
            int status;
            waitpid(pid, &status, 0);
            
            if(WIFEXITED(status))
                return WEXITSTATUS(status);
            
            throw std::runtime_error("Abnormal child termination: " + stream->err);
        }
    }
    
    throw std::logic_error("exec");
}

}
}

