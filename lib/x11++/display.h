/*
 * display.h
 *
 *  Created on: 20/12/2012
 *      Author: nicholas
 */

#ifndef UIX11DISPLAY_H_
#define UIX11DISPLAY_H_
#include <X11/Xlib.h>
#include <stdexcept>
#include <set>
#include <string>
#include <unordered_map>

namespace cxxide
{
namespace x11
{

class window_t;

class display_t
{
private:
	Display* display = nullptr;
	std::set<window_t*> windows;
	bool terminiate = false;
	std::unordered_map<std::string, Atom> atoms;
public:

	struct error : public std::runtime_error
	{
		explicit error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};


	explicit display_t(const char* display_name = nullptr);

	display_t(const display_t&) = delete;
	display_t& operator=(const display_t&) = delete;


	bool register_window(window_t* window);
	bool unregister_window(window_t* window);


	Atom get_atom(const std::string& atom_name, bool only_if_exists = false, bool purge_cached = false);

	void run_display_event_loop();
	void end_display_event_loop();

	Display* operator*() const;

	~display_t();
};

} /* namespace x11 */
} /* namespace ui */
#endif /* UIX11DISPLAY_H_ */
