/*
 * cairowindow.cpp
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */

#include "cairowindow.h"
#include "display.h"
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

namespace cxxide
{
namespace cairo
{

cairo_window_t::cairo_window_t(x11::display_t& display, const x11::rectangle_t<int>& rect) throw (error)
 : window_t(display, rect)
{
	int screen = DefaultScreen(*display);
	Visual* visual = DefaultVisual(*display, screen);

	cs = cairo_xlib_surface_create(*display, window, visual, rect.width(), rect.height());
	if(!cs)
		throw error("Unable to create cairo_surface");

	cr = cairo_create(cs);
	if(!cr)
		throw error("Unable to create cairo canvas");
}
cairo_window_t::cairo_window_t(window_t& parent, const x11::rectangle_t<int>& rect) throw (error)
 : window_t(parent, rect)
{
	int screen = DefaultScreen(*display);
	Visual* visual = DefaultVisual(*display, screen);

	cs = cairo_xlib_surface_create(*display, window, visual, rect.width(), rect.height());
	if(!cs)
		throw error("Unable to create cairo_surface");

	cr = cairo_create(cs);
	if(!cr)
		throw error("Unable to create cairo canvas");
}

void cairo_window_t::draw()
{
	_draw(cr);

	if (cairo_status(cr))
	{
		printf("Cairo is unhappy: %s\n", cairo_status_to_string(cairo_status(cr)));
		exit(0);
	}
}

void cairo_window_t::on_key_event(const x11::key_event_t& key_event)
{
}
void cairo_window_t::on_mouse_event(const x11::mouse_event_t& mouse_event)
{
}
void cairo_window_t::on_window_event(const x11::window_event_t& window_event)
{
	using namespace x11;

	switch(window_event.type)
	{
		case window_event_t::Enter:
			break;
		case window_event_t::Leave:
			break;
		case window_event_t::Focus_In:
			break;
		case window_event_t::Focus_Out:
			break;
		case window_event_t::Resize:
		{
			cairo_xlib_surface_set_size(cs, window_event.size.width, window_event.size.height);
			break;
		}
		case window_event_t::Close:
			break;
	}
}

cairo_window_t::~cairo_window_t()
{
	cairo_destroy(cr);
	cairo_surface_destroy(cs);
}

} /* namespace cairo */
} /* namespace ui */
