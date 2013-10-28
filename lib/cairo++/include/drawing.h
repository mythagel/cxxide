/*
 * drawing.h
 *
 *  Created on: 22/12/2012
 *      Author: nicholas
 */

#ifndef UICAIRODRAWING_H_
#define UICAIRODRAWING_H_
#include <cairo.h>
#include <string>
#include "stack.h"
#include "colour.h"
#include "draw_fn.h"

namespace cxxide
{
namespace cairo
{
namespace drawing
{

struct draw_state;

drawfn_t noop();
drawfn_t colour(double r, double g, double b, double a, draw_state& state);
drawfn_t colour(const colour_t& colour, double a, draw_state& state);
drawfn_t fill(bool preserve, draw_state& state);
drawfn_t line(cairo_line_cap_t cap, cairo_line_join_t join, double width, draw_state& state);
drawfn_t stroke(bool preserve, draw_state& state);
drawfn_t font(const std::string& family, cairo_font_slant_t slant, cairo_font_weight_t weight, double size, draw_state& state);
drawfn_t move_to(double x, double y, draw_state& state);
drawfn_t paint(draw_state& state);

drawfn_t arc(double x, double y, double radius, double angle1, double angle2, draw_state& state);
drawfn_t circle(double x, double y, double radius, draw_state& state);
drawfn_t rectangle(double x, double y, double width, double height, draw_state& state);
drawfn_t rounded_rectangle(double x, double y, double width, double height, double aspect, double corner_radius, draw_state& state);
drawfn_t text(const std::string& utf8, bool path, draw_state& state);
drawfn_t line_path(double x1, double y1, double x2, double y2, draw_state& state);
drawfn_t line_to(double x, double y, draw_state& state);
drawfn_t close_path(draw_state& state);

} /* namespace drawing */
} /* namespace cairo */
} /* namespace ui */
#endif /* UICAIRODRAWING_H_ */
