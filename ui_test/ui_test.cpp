/*
 * ui_test.cpp
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */
#include "ui/x11/display.h"
#include "ui/cairo/cairowindow.h"

int main()
{
	using namespace ui;
	using namespace ui::x11;
	using namespace ui::cairo;

	/*
	 * Initialise connection to X11
	 */
	display_t display{getenv("DISPLAY")};

	cairo_window_t window{display, rectangle_t<int>{0, 0, 1024, 576}};

	window.map();

	display.run_display_event_loop();

	return 0;
}
