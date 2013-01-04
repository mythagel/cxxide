/*
 * cairowindow.h
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */

#ifndef UICAIROCAIROWINDOW_H_
#define UICAIROCAIROWINDOW_H_
#include "../x11/window.h"
#include <stdexcept>
#include <cairo.h>
#include <memory>

namespace ui
{

namespace pango
{
class context_t;
}

namespace cairo
{

/*
 * TODO refactor cairo_window_t to remove the pango font stuff.
 *
 * Really should have a top level ui::window_t that combines the x11, cairo and pango stuff
 */
class cairo_window_t : public x11::window_t
{
protected:
	cairo_surface_t* cs = nullptr;
	cairo_t* cr = nullptr;
	std::unique_ptr<pango::context_t> context;

	virtual void _draw(cairo_t* cr);
public:

	struct error : public std::runtime_error
	{
		error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};

	cairo_window_t(x11::display_t& display, const rectangle_t<int>& rect) throw (error);
	cairo_window_t(window_t& parent, const rectangle_t<int>& rect) throw (error);

	cairo_window_t(const window_t&) = delete;
	cairo_window_t& operator=(const window_t&) = delete;

	virtual void draw() override;

	virtual void on_key_event(const x11::key_event_t& key_event) override;
	virtual void on_mouse_event(const x11::mouse_event_t& mouse_event) override;
	virtual void on_window_event(const x11::window_event_t& window_event) override;

	virtual ~cairo_window_t();
};

} /* namespace cairo */
} /* namespace ui */
#endif /* UICAIROCAIROWINDOW_H_ */
