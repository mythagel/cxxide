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
#include "scopedcontext.h"
#include "context.h"
#include "layout.h"
#include "signal.h"

using namespace cxxide;

// REALLY going to need to remember this... 
// http://www.cairographics.org/FAQ/#sharp_lines

/*
Containers need measure and then layout passes.
then draw will use the layed out position
x11::window_t needs to remember its current geometry
callback should not have to explicitly call parent - use std::function for callback list.
window registers with its own callbacks for what is needed.
no value types - shared_ptr and non-copyable.
*/

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class component : public cairo::cairo_window_t
{
protected:
    std::unique_ptr<pango::context_t> context;
    int x;
    int y;
    int width;
    int height;
public:
    component(x11::window_t& parent, const x11::rectangle_t<int>& rect)
     : cairo_window_t(parent, rect), context(make_unique<pango::context_t>(cr)),
       x(rect.min.x), y(rect.min.y), width(rect.width()), height(rect.height())
    {
    }
    
    cxxide::signal<x11::key_event_t> key_event;
    cxxide::signal<x11::mouse_event_t> mouse_event;
    cxxide::signal<x11::window_event_t> window_event;

    virtual void on_key_event(const x11::key_event_t& key_event) final
    {
        cairo_window_t::on_key_event(key_event);
        this->key_event(key_event);
    }
    virtual void on_mouse_event(const x11::mouse_event_t& mouse_event) final
    {
        cairo_window_t::on_mouse_event(mouse_event);
        this->mouse_event(mouse_event);
    }
    virtual void on_window_event(const x11::window_event_t& window_event) final
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
			    context->update(cr);
			    x = window_event.size.x;
			    y = window_event.size.y;
			    width = window_event.size.width;
			    height = window_event.size.height;
			    break;
		    case window_event_t::Close:
			    break;
	    }
	    
	    this->window_event(window_event);
    }
};

/* Widgets needed

button (perform action text and image label)
    states: inactive, idle, hover, hold
label (display information icon and text)
edit (user input)
switch (on/off toggle)
progress (progress notification bar and spinner style)
progress spinner (indetermine progress notification)
treeview (arrows visible on hover. nesting indicated by whitespace)
   treeview does not have to be generic.
   can be specific to purpose (also will show arrow for current document.)
file selection text box (show autocomplete options as scrolling list)
list box (for code completion selection)
code editor (main widget)
    not generic - specifc to purpose with formatting etc.
    primary storage of edited text content.
drop down / radio button (select between small number of alternatives)
checkbox (select between options not for on/off stuff)

cmake editor
maybe an active text box
i.e. type in text gets split by spaces into individual items
each tiem has a small x to remove it. could click to reorder.

future (not currently used)
value spinner (+/- to increment value)

*/

/*

x11 display has to distribute events to existing windows
    knows about all created windows
    uses callbacks to send events to window
    window callback impl calls signal distribution function
    then handled.

window class
top level window
    contains x11::window, cairo::window (perhaps unneeded)
    wrap pango in as well.
    (pango created on demand)
    contains list of component as child windows
    
component
    child window
    basically same as above (cairo pango etc.)
    parent window tracked

need layout passes in window class.
needs to measure and layout child components (windows)

*/

cairo::drawfn_t pango_layout(std::shared_ptr<pango::layout_t> layout, cairo::drawing::draw_state&)
{
    return [layout](cairo_t* cr) { layout->draw(cr); };
}

struct button : component
{
    enum {
        inactive,
        active,
        hover,
        down
    } bstate;
    
    std::shared_ptr<pango::layout_t> text;
    
	button(x11::window_t& parent)
	 : component(parent, x11::rectangle_t<int>{50, 50, 50+200, 50+50}),
	   bstate(active)
	{
		select_events(  ButtonPressMask | ButtonReleaseMask |
						EnterWindowMask | LeaveWindowMask |
						StructureNotifyMask | ExposureMask);
		
		mouse_event.connect([this](const x11::mouse_event_t& e)
		{
		    handle_mouse_event(e);
		});
		window_event.connect([this](const x11::window_event_t& e)
		{
		    handle_window_event(e);
		});
		
		text = context->make_layout();
		text->font("Sans 16");
	}
	
	void handle_mouse_event(const x11::mouse_event_t& mouse_event)
	{
		//window_t::on_mouse_event(mouse_event);

		using namespace x11;
		switch(mouse_event.type)
		{
			case mouse_event_t::Down:
				fprintf(stderr, "Button Mouse Down, Button: %d\n", mouse_event.button);
				break;
			case mouse_event_t::Up:
				fprintf(stderr, "Button Mouse Up, Button: %d\n", mouse_event.button);
				break;
		}
	}
	
    virtual void _draw(cairo_t* cr) override
    {
        using namespace cairo::drawing;
        
        text->text("build");
        
        int width;
        int height;
        text->size(&width, &height);
        
        fprintf(stderr, "%d, %d\n", width, height);
        move_resize({50, 50, 50+width+20, 50+height+20});
        
        draw_state state;
        
        /*
          Highlight / contrast colour for entire UI is based on current project name (as used to
          generate the hash image.)
         */
        switch(bstate)
        {
            case inactive:
            {
                auto draw_fn = cairo::make_stack(
                    colour(0, 0, 0, 1.0, state),
                    paint(state),
                    line(CAIRO_LINE_CAP_SQUARE, CAIRO_LINE_JOIN_MITER, 1, state),
                    colour(0.3, 0.3, 0.3, 1.0, state),
                    rectangle(0.5, 0.5, width+19, height+19, state),
                    stroke(false, state),
                    move_to(10, 10, state),
                    pango_layout(text, state)
                    );
                
                draw_fn(cr);
                break;
            }
            case active:
            {
                auto draw_fn = cairo::make_stack(
                    colour(0, 0, 0, 1.0, state),
                    paint(state),
                    line(CAIRO_LINE_CAP_SQUARE, CAIRO_LINE_JOIN_MITER, 1, state),
                    colour(0.7, 0.7, 0.7, 1.0, state),
                    rectangle(0.5, 0.5, width+19, height+19, state),
                    stroke(false, state),
                    move_to(10, 10, state),
                    pango_layout(text, state)
                    );
                
                draw_fn(cr);
                break;
            }
            case hover:
            {
                auto draw_fn = cairo::make_stack(
                    colour(0, 0, 0, 1.0, state),
                    paint(state),
                    line(CAIRO_LINE_CAP_SQUARE, CAIRO_LINE_JOIN_MITER, 1, state),
                    colour(0.03, 0.15, 1, 1.0, state),
                    rectangle(0.5, 0.5, width+19, height+19, state),
                    stroke(false, state),
                    move_to(10, 10, state),
                    colour(1, 1, 1, 1.0, state),
                    pango_layout(text, state)
                    );
                
                draw_fn(cr);
                break;
            }
            case down:
            {
                break;
            }
        }
    }
    
	void handle_window_event(const x11::window_event_t& window_event)
	{
		//window_t::on_window_event(window_event);

		using namespace x11;
		switch(window_event.type)
		{
			case window_event_t::Enter:
				fprintf(stderr, "Button Enter\n");
				bstate = hover;
				draw();
				break;
			case window_event_t::Leave:
				fprintf(stderr, "Button Leave\n");
				bstate = active;
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
				toggle_fullscreen();
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

    window.set_title("cxxide widget test");
	window.select_events(	KeyPressMask | KeyReleaseMask |
							ButtonPressMask | ButtonReleaseMask |
							//PointerMotionMask |
							EnterWindowMask | LeaveWindowMask |
							StructureNotifyMask | ExposureMask);
	window.map();

    /*
     TODO investigate game loops for how to do animations
     need to be able to invalidate individual rects while in the update phase
     and then redraw in the redraw phase - WHILE generally sitting idle when there
     is nothing to draw.
     */
	display.run_display_event_loop();

	return 0;
}
