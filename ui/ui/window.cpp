/*
 * window.cpp
 *
 *  Created on: 04/01/2013
 *      Author: nicholas
 */

#include "window.h"
#include "make_unique.h"
#include "pango/context.h"

namespace ui
{

void window_t::_draw(cairo_t* cr)
{
}

window_t::window_t(x11::display_t& display, const rectangle_t<int>& rect) throw (error)
 : cairo_window_t(display, rect), context(make_unique<pango::context_t>(cr))
{
}
window_t::window_t(window_t& parent, const rectangle_t<int>& rect) throw (error)
 : cairo_window_t(parent, rect), context(make_unique<pango::context_t>(cr))
{
}

void window_t::on_key_event(const x11::key_event_t& key_event)
{
	cairo_window_t::on_key_event(key_event);
}
void window_t::on_mouse_event(const x11::mouse_event_t& mouse_event)
{
	cairo_window_t::on_mouse_event(mouse_event);
}
void window_t::on_window_event(const x11::window_event_t& window_event)
{
	cairo_window_t::on_window_event(window_event);

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
			context->update(cr);
			break;
		}
		case window_event_t::Close:
			break;
	}
}

window_t::~window_t()
{
}

} /* namespace ui */
