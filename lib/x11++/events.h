/*
 * events.h
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */

#ifndef UIX11EVENTS_H_
#define UIX11EVENTS_H_
#include <cstdint>
#include <string>

namespace cxxide
{
namespace x11
{

struct key_event_t
{
	enum
	{
		Down,
		Up,
		Repeat
	} type = Up;
	enum
	{
		Shift 	= 1<<0,
		Lock 	= 1<<1,
		Control = 1<<2,
	};
	std::uint32_t mask = 0;
	std::string key;
};

struct mouse_event_t
{
	enum
	{
		Down,
		Up,
		Move
	} type = Up;
	enum
	{
		One 	= 1<<0,
		Two 	= 1<<1,
		Three 	= 1<<2,
		Four 	= 1<<3,
		Five 	= 1<<4
	};
	uint32_t button = 0;
	struct
	{
		int x = 0;
		int y = 0;
	} position;
};

struct window_event_t
{
	enum
	{
		Enter,
		Leave,
		Focus_In,
		Focus_Out,
		Resize,
		Close
	} type = Enter;
	struct
	{
		int width = 0;
		int height = 0;
	} size;
};

} /* namespace x11 */
} /* namespace ui */
#endif /* UIX11EVENTS_H_ */
