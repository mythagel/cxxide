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

#include "git.h"
#include <sys/types.h>
#include <unistd.h>


repo_t init(const std::string& path)
{
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
        {
            throw std::runtime_error("vfork");
        }
        case 0:
        {
            int err;
            
            err = execvpe(const char *file, char *const argv[], char *const envp[]);
        }
        default:
        {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
