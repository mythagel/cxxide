/*
 * make_unique.h
 *
 *  Created on: 30/11/2012
 *      Author: nicholas
 */

#ifndef MAKE_UNIQUE_H_
#define MAKE_UNIQUE_H_
#include <memory>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif /* MAKE_UNIQUE_H_ */
