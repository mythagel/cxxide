/*
 * rectangle.h
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#ifndef UIRECTANGLE_H_
#define UIRECTANGLE_H_
#include "point.h"

namespace ui
{

template<typename value_t>
struct rectangle_t
{
	point_t<value_t> min;
	point_t<value_t> max;

	rectangle_t() = default;
	rectangle_t(const point_t<value_t>& min, const point_t<value_t>& max)
	 : min(min), max(max)
	{
	}
	rectangle_t(value_t x1, value_t y1, value_t x2, value_t y2)
	 : min(x1, y1), max(x2, y2)
	{
	}

	value_t width() const
	{
		return max.x - min.x;
	}

	value_t height() const
	{
		return max.y - min.y;
	}
};

} /* namespace ui */
#endif /* UIRECTANGLE_H_ */
