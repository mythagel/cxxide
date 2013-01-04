/*
 * ui_test.cpp
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */
#include "ui/x11/display.h"
#include "ui/window.h"
#include <cstdio>

class main_window : public ui::window_t
{
public:
	main_window(ui::x11::display_t& display)
	 : window_t(display, ui::rectangle_t<int>{0, 0, 1024, 576})
	{
	}

	virtual void on_key_event(const ui::x11::key_event_t& key_event)
	{
		ui::window_t::on_key_event(key_event);

		using namespace ui::x11;
		switch(key_event.type)
		{
			case key_event_t::Down:
				fprintf(stderr, "Key Down\n");
				break;
			case key_event_t::Up:
				fprintf(stderr, "Key Up\n");
				break;
			case key_event_t::Repeat:
				fprintf(stderr, "Key Repeat\n");
				break;
		}
	}
	virtual void on_mouse_event(const ui::x11::mouse_event_t& mouse_event)
	{
		ui::window_t::on_mouse_event(mouse_event);

		using namespace ui::x11;
		switch(mouse_event.type)
		{
			case mouse_event_t::Down:
				fprintf(stderr, "Mouse Down\n");
				break;
			case mouse_event_t::Up:
				fprintf(stderr, "Mouse Up\n");
				break;
			case mouse_event_t::Move:
				fprintf(stderr, "Mouse Move %dx%d\n", mouse_event.position.x, mouse_event.position.y);
				break;
		}
	}
	virtual void on_window_event(const ui::x11::window_event_t& window_event)
	{
		ui::window_t::on_window_event(window_event);

		using namespace ui::x11;
		switch(window_event.type)
		{
			case window_event_t::Enter:
				fprintf(stderr, "Enter\n");
				break;
			case window_event_t::Leave:
				fprintf(stderr, "Leave\n");
				break;
			case window_event_t::Focus_In:
				fprintf(stderr, "Focus_In\n");
				break;
			case window_event_t::Focus_Out:
				fprintf(stderr, "Focus_Out\n");
				break;
			case window_event_t::Resize:
				fprintf(stderr, "Resize\n");
				break;
			case window_event_t::Close:
				fprintf(stderr, "Close\n");
				display.end_display_event_loop();
				break;
		}
	}
};

int main()
{
	using namespace ui;
	using namespace ui::x11;
	using namespace ui::cairo;

	/*
	 * Initialise connection to X11
	 */
	display_t display{getenv("DISPLAY")};

	main_window window{display};

	window.select_events(	KeyPressMask | KeyReleaseMask |
							ButtonPressMask | ButtonReleaseMask |
							PointerMotionMask |
							EnterWindowMask | LeaveWindowMask |
							StructureNotifyMask | ExposureMask);
	window.map();

	display.run_display_event_loop();

	return 0;
}
