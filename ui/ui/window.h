/*
 * window.h
 *
 *  Created on: 04/01/2013
 *      Author: nicholas
 */

#ifndef UIWINDOW_H_
#define UIWINDOW_H_
#include "cairo/cairowindow.h"

namespace ui
{

namespace pango
{
class context_t;
}

class window_t : public cairo::cairo_window_t
{
protected:
	std::unique_ptr<pango::context_t> context;

	virtual void _draw(cairo_t* cr) override;
public:

	struct error : public std::runtime_error
	{
		error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};

	window_t(x11::display_t& display, const rectangle_t<int>& rect) throw (error);
	window_t(window_t& parent, const rectangle_t<int>& rect) throw (error);

	window_t(const window_t&) = delete;
	window_t& operator=(const window_t&) = delete;

	virtual void on_key_event(const x11::key_event_t& key_event) override;
	virtual void on_mouse_event(const x11::mouse_event_t& mouse_event) override;
	virtual void on_window_event(const x11::window_event_t& window_event) override;

	~window_t();
};

} /* namespace ui */
#endif /* UIWINDOW_H_ */
