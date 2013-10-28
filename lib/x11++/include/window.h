/*
 * window.h
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#ifndef UIX11WINDOW_H_
#define UIX11WINDOW_H_
#include <X11/Xlib.h>
#include <stdexcept>
#include "rectangle.h"
#include "events.h"

namespace cxxide
{
namespace x11
{

class display_t;

class window_t
{
protected:
	display_t& display;
	Window window = Window();
public:

	struct error : public std::runtime_error
	{
		error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};

	window_t(display_t& display, const rectangle_t<int>& rect);
	window_t(window_t& parent, const rectangle_t<int>& rect);

	window_t(const window_t&) = delete;
	window_t& operator=(const window_t&) = delete;

	void select_events(long events);

	void map() const;
	void unmap() const;
	
	void set_title(const std::string& name);
	bool toggle_fullscreen();
	void move_resize(const rectangle_t<int>& rect);

	virtual void draw() = 0;

	virtual void on_key_event(const key_event_t& key_event) = 0;
	virtual void on_mouse_event(const mouse_event_t& mouse_event) = 0;
	virtual void on_window_event(const window_event_t& window_event) = 0;

	operator Window() const;

	virtual ~window_t();
};

} /* namespace x11 */
} /* namespace ui */
#endif /* UIX11WINDOW_H_ */
