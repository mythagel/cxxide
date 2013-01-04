/*
 * drawstate.h
 *
 *  Created on: 22/12/2012
 *      Author: nicholas
 */

#ifndef UICAIRODRAWSTATE_H_
#define UICAIRODRAWSTATE_H_
#include <cairo.h>
#include <string>

namespace ui
{
namespace cairo
{
namespace drawing
{

struct draw_state
{
	struct
	{
		bool undefined = true;
		double r = 0.0;
		double g = 0.0;
		double b = 0.0;
		double a = 0.0;
	} Colour;

	struct
	{
		bool undefined = true;
		cairo_line_cap_t cap = CAIRO_LINE_CAP_BUTT;
		cairo_line_join_t join = CAIRO_LINE_JOIN_MITER;
		double width = 0.0;
	} Line;

	struct
	{
		bool undefined = true;
		std::string family;
		cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
		cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;
		double size = 0.0;
	} Font;

	struct
	{
		bool undefined = true;
		cairo_fill_rule_t fill_rule = CAIRO_FILL_RULE_WINDING;
	} Fill;

	struct
	{
		bool undefined = true;
		double x = 0.0;
		double y = 0.0;
	} Position;

	bool Path = false;
};

} /* namespace drawing */
} /* namespace cairo */
} /* namespace ui */
#endif /* UICAIRODRAWSTATE_H_ */
