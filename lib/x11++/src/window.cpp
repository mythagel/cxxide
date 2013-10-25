/*
 * window.cpp
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#include "window.h"
#include "display.h"

namespace cxxide
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

	Atom WM_DELETE_WINDOW = display.get_atom("WM_DELETE_WINDOW", true);
	if(WM_DELETE_WINDOW)
    	XSetWMProtocols(*display, window, &WM_DELETE_WINDOW, 1);

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

void window_t::set_title(const std::string& name)
{
    XStoreName(*display, window, name.c_str());
}

bool window_t::toggle_fullscreen()
{
    auto NET_WM_STATE_FULLSCREEN = display.get_atom("_NET_WM_STATE_FULLSCREEN", true);

    if (NET_WM_STATE_FULLSCREEN)
    {
        auto NET_WM_STATE = display.get_atom("_NET_WM_STATE");

        XEvent ev;
        ev.type = ClientMessage;
        ev.xclient.serial = 0;
        ev.xclient.send_event = True;
        ev.xclient.display = *display;
        ev.xclient.window = window;
        ev.xclient.message_type = NET_WM_STATE;
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = 2; // _NET_WM_STATE_TOGGLE
        ev.xclient.data.l[1] = NET_WM_STATE_FULLSCREEN;
        ev.xclient.data.l[2] = 0L;

        XSendEvent(*display, DefaultRootWindow(*display), False, SubstructureNotifyMask, &ev);
        return true;
    }
    return false;
}

window_t::operator Window() const
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
