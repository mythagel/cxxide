/*
 * point.h
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#ifndef UIPOINT_H_
#define UIPOINT_H_

namespace ui
{

template<typename value_t>
struct point_t
{
	value_t x = value_t();
	value_t y = value_t();

	point_t() = default;
	point_t(value_t x, value_t y)
	 : x(x), y(y)
	{
	}
};

} /* namespace ui */
#endif /* UIPOINT_H_ */
