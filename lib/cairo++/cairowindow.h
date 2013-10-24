/*
 * cairowindow.h
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */

#ifndef UICAIROCAIROWINDOW_H_
#define UICAIROCAIROWINDOW_H_
#include "window.h"
#include <stdexcept>
#include <memory>
#include "cairo_fwd.h"

namespace cxxide
{
namespace cairo
{

class cairo_window_t : public x11::window_t
{
protected:
	cairo_surface_t* cs = nullptr;
	cairo_t* cr = nullptr;

	virtual void _draw(cairo_t* cr) =0;
public:

	struct error : public std::runtime_error
	{
		error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};

	cairo_window_t(x11::display_t& display, const x11::rectangle_t<int>& rect) throw (error);
	cairo_window_t(x11::window_t& parent, const x11::rectangle_t<int>& rect) throw (error);

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
