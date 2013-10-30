/*
 * display.cpp
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#include "display.h"
#include <algorithm>
#include "window.h"
#include "events.h"
#include "keysym.h"
#include <poll.h>

namespace cxxide
{
namespace x11
{

struct thread_init_t
{
    thread_init_t()
    {
        XInitThreads();
    }
} thread_init;

display_t::display_t(const char* display_name)
 : display(XOpenDisplay(display_name))
{
	if(!display)
		throw error(std::string("Unable to open display: ") + display_name);
}

int display_t::fd() const
{
    return ConnectionNumber(display);
}

bool display_t::register_window(window_t* window)
{
	return windows.insert(window).second;
}
bool display_t::unregister_window(window_t* window)
{
	return windows.erase(window) > 0;
}

Atom display_t::get_atom(const std::string& atom_name, bool only_if_exists, bool purge_cached)
{
	if(purge_cached)
		atoms.erase(atom_name);

	auto it = atoms.find(atom_name);
	if(it == atoms.end())
	{
		Atom atom = XInternAtom(display, atom_name.c_str(), only_if_exists ? True : False);
		atoms.insert(std::make_pair(atom_name, atom));
		return atom;
	}

	return it->second;
}

void display_t::run_display_event_loop()
{
//    pollfd x11;
//    x11.fd = fd();
//    x11.events = POLLIN;
//    x11.revents = 0;
    
	terminiate = false;
	XEvent xevent;
	while(!terminiate)
	{
	    //int err = poll(&x11, 1, );
		XNextEvent(display, &xevent);

		window_t* window { nullptr };
		auto win_it = std::find_if(windows.begin(), windows.end(), [&xevent](window_t* window) -> bool { return *window == xevent.xany.window; });
		if(win_it != end(windows))
			window = *win_it;

		switch(xevent.type)
		{
			case KeyPress:
			case KeyRelease:
			{
				XKeyEvent& xkey = xevent.xkey;

				key_event_t key_event;
				key_event.type = xkey.type == KeyPress ? key_event_t::Down : key_event_t::Up;

				if(xevent.type == KeyRelease && XEventsQueued(display, QueuedAfterReading))
				{
					XEvent xevent_next;
					XPeekEvent(display, &xevent_next);

					if (xevent_next.type == KeyPress && xevent_next.xkey.time == xkey.time && xevent_next.xkey.keycode == xkey.keycode)
					{
						key_event.type = key_event_t::Repeat;
						XNextEvent(display, &xevent_next);
					}
				}

				if(xkey.state & ShiftMask)
					key_event.mask |= key_event_t::Shift;
				if(xkey.state & LockMask)
					key_event.mask |= key_event_t::Lock;
				if(xkey.state & ControlMask)
					key_event.mask |= key_event_t::Control;

				// Investigate Xutf8LookupString
				key_event.key = wchar2utf8(keysym2ucs(XkbKeycodeToKeysym(display, xkey.keycode, 0, 0)));

				window->on_key_event(key_event);
				break;
			}
			case ButtonPress:
			case ButtonRelease:
			{
				XButtonEvent& xbutton = xevent.xbutton;

				mouse_event_t mouse_event;
				mouse_event.type = xbutton.type == ButtonPress ? mouse_event_t::Down : mouse_event_t::Up;

				mouse_event.button = xbutton.button;
				mouse_event.position.x = xbutton.x;
				mouse_event.position.y = xbutton.y;

				window->on_mouse_event(mouse_event);
				break;
			}
			case MotionNotify:
			{
				XMotionEvent& xmotion = xevent.xmotion;

				mouse_event_t mouse_event;
				mouse_event.type = mouse_event_t::Move;
				mouse_event.position.x = xmotion.x;
				mouse_event.position.y = xmotion.y;

				window->on_mouse_event(mouse_event);
				break;
			}
			case EnterNotify:
			{
//				XCrossingEvent& xcrossing = xevent.xcrossing;

				window_event_t window_event;
				window_event.type = window_event_t::Enter;

				window->on_window_event(window_event);
				break;
			}
			case LeaveNotify:
			{
//				XCrossingEvent& xcrossing = xevent.xcrossing;

				window_event_t window_event;
				window_event.type = window_event_t::Leave;

				window->on_window_event(window_event);
				break;
			}
			case FocusIn:
			{
//				XFocusChangeEvent& xfocus = xevent.xfocus;

				window_event_t window_event;
				window_event.type = window_event_t::Focus_In;

				window->on_window_event(window_event);
				break;
			}
			case FocusOut:
			{
//				XFocusChangeEvent& xfocus = xevent.xfocus;

				window_event_t window_event;
				window_event.type = window_event_t::Focus_Out;

				window->on_window_event(window_event);
				break;
			}
			case KeymapNotify:
			{
//				XKeymapEvent& xkeymap = xevent.xkeymap;
				break;
			}
			case Expose:
			{
				XExposeEvent& xexpose = xevent.xexpose;

				if (xexpose.count == 0)
					window->draw();
				break;
			}
			case GraphicsExpose:
			{
//				XGraphicsExposeEvent& xgraphicsexpose = xevent.xgraphicsexpose;
				break;
			}
			case NoExpose:
			{
//				XNoExposeEvent& xnoexpose = xevent.xnoexpose;
				break;
			}
			case VisibilityNotify:
			{
//				XVisibilityEvent& xvisibility = xevent.xvisibility;
				break;
			}
			case CreateNotify:
			{
//				XCreateWindowEvent& xcreatewindow = xevent.xcreatewindow;
				break;
			}
			case DestroyNotify:
			{
//				XDestroyWindowEvent& xdestroywindow = xevent.xdestroywindow;
				break;
			}
			case UnmapNotify:
			{
//				XUnmapEvent& xunmap = xevent.xunmap;
				break;
			}
			case MapNotify:
			{
//				XMapEvent& xmap = xevent.xmap;
				break;
			}
			case MapRequest:
			{
//				XMapRequestEvent& xmaprequest = xevent.xmaprequest;
				break;
			}
			case ReparentNotify:
			{
//				XReparentEvent& xreparent = xevent.xreparent;
				break;
			}
			case ConfigureNotify:
			{
				XConfigureEvent& xconfigure = xevent.xconfigure;

				window_event_t window_event;
				window_event.type = window_event_t::Resize;
				window_event.size.x = xconfigure.x;
				window_event.size.y = xconfigure.y;
				window_event.size.width = xconfigure.width;
				window_event.size.height = xconfigure.height;

				window->on_window_event(window_event);
				break;
			}
			case ConfigureRequest:
			{
//				XConfigureRequestEvent& xconfigurerequest = xevent.xconfigurerequest;
				break;
			}
			case GravityNotify:
			{
//				XGravityEvent& xgravity = xevent.xgravity;
				break;
			}
			case ResizeRequest:
			{
//				XResizeRequestEvent& xresizerequest = xevent.xresizerequest;
				break;
			}
			case CirculateNotify:
			{
//				XCirculateEvent& xcirculate = xevent.xcirculate;
				break;
			}
			case CirculateRequest:
			{
//				XCirculateRequestEvent& xcirculaterequest = xevent.xcirculaterequest;
				break;
			}
			case PropertyNotify:
			{
//				XPropertyEvent& xproperty = xevent.xproperty;
				break;
			}
			case SelectionClear:
			{
//				XSelectionClearEvent& xselectionclear = xevent.xselectionclear;
				break;
			}
			case SelectionRequest:
			{
//				XSelectionRequestEvent& xselectionrequest = xevent.xselectionrequest;
				break;
			}
			case SelectionNotify:
			{
//				XSelectionEvent& xselection = xevent.xselection;
				break;
			}
			case ColormapNotify:
			{
//				XColormapEvent& xcolormap = xevent.xcolormap;
				break;
			}
			case ClientMessage:
			{
				XClientMessageEvent& xclient = xevent.xclient;

				auto WM_DELETE_WINDOW = get_atom("WM_DELETE_WINDOW");
				if(xclient.data.l[0] == WM_DELETE_WINDOW)
				{
					window_event_t window_event;
					window_event.type = window_event_t::Close;

					window->on_window_event(window_event);
				}
				break;
			}
			case MappingNotify:
			{
				XMappingEvent& xmapping = xevent.xmapping;
				XRefreshKeyboardMapping(&xmapping);
				break;
			}
			case GenericEvent:
			{
//				XGenericEvent& xgeneric = xevent.xgeneric;
//				XGenericEventCookie& xcookie = xevent.xcookie;
				break;
			}
		}
	}
}
void display_t::end_display_event_loop()
{
	terminiate = true;
}

Display* display_t::operator*() const
{
	return display;
}

display_t::~display_t()
{
	XCloseDisplay(display);
}

} /* namespace x11 */
} /* namespace ui */
