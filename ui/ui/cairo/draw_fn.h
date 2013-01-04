/*
 * draw_fn.h
 *
 *  Created on: 22/12/2012
 *      Author: nicholas
 */

#ifndef UICAIRODRAW_FN_H_
#define UICAIRODRAW_FN_H_
#include <functional>

namespace ui
{
namespace cairo
{

typedef std::function<void(cairo_t*)> drawfn_t;

} /* namespace cairo */
} /* namespace ui */
#endif /* UICAIRODRAW_FN_H_ */
