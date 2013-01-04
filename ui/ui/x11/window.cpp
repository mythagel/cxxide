/*
 * window.cpp
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#include "window.h"
#include "display.h"

namespace ui
{
namespace x11
{

window_t::window_t(display_t& display, const rectangle_t<int>& rect)
 : display(display)
{
	int screen = DefaultScreen(*display);
	window = XCreateSimpleWindow(*display, DefaultRootWindow(*display), rect.min.x, rect.min.y, rect.width(), rect.height(), 0, BlackPixel(*display, screen), BlackPixel(*display, screen));

	if(!window)
		throw error("Unable to create window");

	Atom wm_delete_window = display.get_atom("WM_DELETE_WINDOW");
	XSetWMProtocols(*display, window, &wm_delete_window, 1);

	display.register_window(this);
}
window_t::window_t(window_t& parent, const rectangle_t<int>& rect)
 : display(parent.display)
{
	int screen = DefaultScreen(*display);
	window = XCreateSimpleWindow(*display, parent.window, rect.min.x, rect.min.y, rect.width(), rect.height(), 0, BlackPixel(*display, screen), BlackPixel(*display, screen));

	if(!window)
		throw error("Unable to create child window");

	display.register_window(this);
}

void window_t::select_events(long events)
{
	XSelectInput(*display, window, events);
}

void window_t::map() const
{
	XMapWindow(*display, window);
}
void window_t::unmap() const
{
	XUnmapWindow(*display, window);
}

Window window_t::operator*() const
{
	return window;
}

window_t::~window_t()
{
	display.unregister_window(this);

	XDestroyWindow(*display, window);
}

} /* namespace x11 */
} /* namespace ui */
