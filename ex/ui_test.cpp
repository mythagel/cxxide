/*
 * ui_test.cpp
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */
#include "display.h"
#include "cairowindow.h"
#include <cstdio>

#include "drawing.h"
#include "drawstate.h"
#include "stack.h"

using namespace cxxide;

struct button : cairo::cairo_window_t
{
    bool highlight;
    
	button(x11::window_t& parent)
	 : cairo_window_t(parent, x11::rectangle_t<int>{0, 0, 100, 35}),
	   highlight(false)
	{
		select_events(  ButtonPressMask | ButtonReleaseMask |
						EnterWindowMask | LeaveWindowMask |
						StructureNotifyMask | ExposureMask);
	}
	
    virtual void _draw(cairo_t* cr) override
    {
        using namespace cairo::drawing;
        
        draw_state state;
        if(highlight)
        {
            auto draw_fn = cairo::make_stack(
                rounded_rectangle(0, 0, 100, 35, 1, 5, state),
                colour(1, 0, 0.1, 1.0, state),
                fill(false, state)
                );
            
            draw_fn(cr);
        }
        else
        {
            auto draw_fn = cairo::make_stack(
                rounded_rectangle(0, 0, 100, 35, 1, 5, state),
                colour(0.5, 0, 0.1, 1.0, state),
                fill(false, state)
                );
            
            draw_fn(cr);
        }
    }
    
	virtual void on_window_event(const x11::window_event_t& window_event) override
	{
		//window_t::on_window_event(window_event);

		using namespace x11;
		switch(window_event.type)
		{
			case window_event_t::Enter:
				fprintf(stderr, "Button Enter\n");
				highlight = true;
				draw();
				break;
			case window_event_t::Leave:
				fprintf(stderr, "Button Leave\n");
				highlight = false;
				draw();
				break;
			case window_event_t::Focus_In:
				fprintf(stderr, "Button Focus_In\n");
				break;
			case window_event_t::Focus_Out:
				fprintf(stderr, "Button Focus_Out\n");
				break;
			case window_event_t::Resize:
				fprintf(stderr, "Button Resize\n");
				break;
			case window_event_t::Close:
				fprintf(stderr, "Button Close\n");
				display.end_display_event_loop();
				break;
		}
	}
};

class main_window : public cairo::cairo_window_t
{
    button b;
public:
	main_window(x11::display_t& display)
	 : cairo_window_t(display, x11::rectangle_t<int>{0, 0, 1024, 576}),
	   b(*this)
	{
	    b.map();
	}

	virtual void on_key_event(const x11::key_event_t& key_event) override
	{
		//window_t::on_key_event(key_event);

		using namespace x11;
		switch(key_event.type)
		{
			case key_event_t::Down:
				fprintf(stderr, "Key Down: %s\n", key_event.key.c_str());
				break;
			case key_event_t::Up:
				fprintf(stderr, "Key Up: %s\n", key_event.key.c_str());
				break;
			case key_event_t::Repeat:
				fprintf(stderr, "Key Repeat: %s\n", key_event.key.c_str());
				break;
		}
	}
	
    virtual void _draw(cairo_t* cr) override
    {
        using namespace cairo::drawing;
        
    }
	
	virtual void on_mouse_event(const x11::mouse_event_t& mouse_event) override
	{
		//window_t::on_mouse_event(mouse_event);

		using namespace x11;
		switch(mouse_event.type)
		{
			case mouse_event_t::Down:
				fprintf(stderr, "Mouse Down, Button: %d\n", mouse_event.button);
				break;
			case mouse_event_t::Up:
				fprintf(stderr, "Mouse Up, Button: %d\n", mouse_event.button);
				break;
			case mouse_event_t::Move:
				fprintf(stderr, "Mouse Move %dx%d\n", mouse_event.position.x, mouse_event.position.y);
				break;
		}
	}
	virtual void on_window_event(const x11::window_event_t& window_event) override
	{
		//window_t::on_window_event(window_event);

		using namespace x11;
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
	using namespace x11;

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
