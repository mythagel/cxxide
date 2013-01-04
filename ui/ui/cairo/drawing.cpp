/*
 * drawing.cpp
 *
 *  Created on: 22/12/2012
 *      Author: nicholas
 */

#include "drawing.h"
#include "drawstate.h"
#include <cmath>

namespace ui
{
namespace cairo
{
namespace drawing
{

drawfn_t noop()
{
	return [](cairo_t* cr) {};
}

drawfn_t colour(double r, double g, double b, double a, draw_state& state)
{
	if
	(
		!state.Colour.undefined &&
		state.Colour.r == r &&
		state.Colour.g == g &&
		state.Colour.b == b &&
		state.Colour.a == a
	)
		return noop();

	state.Colour.undefined = false;
	state.Colour.r = r;
	state.Colour.g = g;
	state.Colour.b = b;
	state.Colour.a = a;

	return [r, g, b, a](cairo_t* cr) { cairo_set_source_rgba(cr, r, g, b, a); };
}
drawfn_t colour(const colour_t& colour, double a, draw_state& state)
{
	double r = colour.red;
	double g = colour.green;
	double b = colour.blue;

	if
	(
		!state.Colour.undefined &&
		state.Colour.r == r &&
		state.Colour.g == g &&
		state.Colour.b == b &&
		state.Colour.a == a
	)
		return noop();

	state.Colour.undefined = false;
	state.Colour.r = r;
	state.Colour.g = g;
	state.Colour.b = b;
	state.Colour.a = a;

	return [r, g, b, a](cairo_t* cr) { cairo_set_source_rgba(cr, r, g, b, a); };
}

drawfn_t fill(bool preserve, draw_state& state)
{
	if(preserve)
	{
		return [](cairo_t* cr) { cairo_fill_preserve(cr); };
	}
	else
	{
		state.Path = false;
		return [](cairo_t* cr) { cairo_fill(cr); };
	}
}

drawfn_t line(cairo_line_cap_t cap, cairo_line_join_t join, double width, draw_state& state)
{
	if
	(
		!state.Line.undefined &&
		state.Line.cap == cap &&
		state.Line.join == join &&
		state.Line.width == width
	)
		return noop();

	state.Line.undefined = false;
	state.Line.cap = cap;
	state.Line.join = join;
	state.Line.width = width;

	return [cap, join, width](cairo_t* cr)
		{
			cairo_set_line_cap(cr, cap);
			cairo_set_line_join(cr, join);
			cairo_set_line_width(cr, width);
		};
}

drawfn_t stroke(bool preserve, draw_state& state)
{
	if(preserve)
	{
		return [](cairo_t* cr) { cairo_stroke_preserve(cr); };
	}
	else
	{
		state.Path = false;
		return [](cairo_t* cr) { cairo_stroke(cr); };
	}
}

drawfn_t font(const std::string& family, cairo_font_slant_t slant, cairo_font_weight_t weight, double size, draw_state& state)
{
	if
	(
		!state.Font.undefined &&
		state.Font.family == family &&
		state.Font.slant == slant &&
		state.Font.weight == weight &&
		state.Font.size == size
	)
		return noop();

	state.Font.undefined = false;
	state.Font.family = family;
	state.Font.slant = slant;
	state.Font.weight = weight;
	state.Font.size = size;

	return [family, slant, weight, size](cairo_t* cr)
		{
			cairo_select_font_face (cr, family.c_str(), slant, weight);
			cairo_set_font_size (cr, size);
		};
}

drawfn_t move_to(double x, double y, draw_state& state)
{
	if
	(
		!state.Position.undefined &&
		state.Position.x == x &&
		state.Position.y == y
	)
		return noop();

	state.Position.undefined = false;
	state.Position.x = x;
	state.Position.y = y;

	return [x, y](cairo_t* cr)
		{
			cairo_move_to(cr, x, y);
		};
}

drawfn_t paint(draw_state& state)
{
	return [](cairo_t* cr)
		{
			cairo_paint(cr);
		};
}

drawfn_t arc(double x, double y, double radius, double angle1, double angle2, draw_state& state)
{
	state.Path = true;
	return [x, y, radius, angle1, angle2](cairo_t* cr) { cairo_arc(cr, x, y, radius, angle1, angle2); };
}

drawfn_t circle(double x, double y, double radius, draw_state& state)
{
	state.Path = true;
	return [x, y, radius](cairo_t* cr) { cairo_arc(cr, x, y, radius, 0, 2*M_PI); };
}

drawfn_t rectangle(double x, double y, double width, double height, draw_state& state)
{
	state.Path = true;
	return [x, y, width, height](cairo_t* cr) { cairo_rectangle (cr, x, y, width, height); };
}

drawfn_t rounded_rectangle(double x, double y, double width, double height, double aspect, double corner_radius, draw_state& state)
{
	state.Path = true;
	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	return [x, y, width, height, aspect, corner_radius, radius, degrees](cairo_t* cr)
		{
			cairo_new_sub_path(cr);
			cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
			cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
			cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
			cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
			cairo_close_path(cr);
		};
}

drawfn_t text(const std::string& utf8, bool path, draw_state& state)
{
	if(path)
	{
		state.Path = true;
		return [utf8](cairo_t* cr) { cairo_text_path(cr, utf8.c_str()); };
	}
	else
	{
		return [utf8](cairo_t* cr) { cairo_show_text(cr, utf8.c_str()); };
	}
}

drawfn_t line_path(double x1, double y1, double x2, double y2, draw_state& state)
{
	bool move_to(true);
	if
	(
		!state.Position.undefined &&
		state.Position.x == x1 &&
		state.Position.y == y1
	)
		move_to = false;

	state.Position.undefined = false;
	state.Position.x = x2;
	state.Position.y = y2;

	if(move_to)
	{
		return [x1, y1, x2, y2](cairo_t* cr)
			{
				cairo_move_to(cr, x1, y1);
				cairo_line_to(cr, x2, y2);
			};
	}
	else
	{
		return [x2, y2](cairo_t* cr)
			{
				cairo_line_to(cr, x2, y2);
			};
	}
}

drawfn_t line_to(double x, double y, draw_state& state)
{
	if
	(
		!state.Position.undefined &&
		state.Position.x == x &&
		state.Position.y == y
	)
		return noop();

	return [x, y](cairo_t* cr)
		{
			cairo_line_to(cr, x, y);
		};
}

drawfn_t close_path(draw_state& state)
{
	return [](cairo_t* cr)
		{
			cairo_close_path(cr);
		};
}

} /* namespace drawing */
} /* namespace cairo */
} /* namespace ui */
