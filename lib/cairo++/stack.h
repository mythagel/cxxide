/*
 * stack.h
 *
 *  Created on: 22/12/2012
 *      Author: nicholas
 */

#ifndef UICAIROSTACK_H_
#define UICAIROSTACK_H_
#include <array>
#include <vector>
#include <cstddef>
#include <cairo/cairo.h>
#include "draw_fn.h"

namespace cxxide
{
namespace cairo
{

template<std::size_t N = 0>
class stack
{
private:
	std::array<drawfn_t, N> fns;
	std::size_t n = 0;
public:
	stack<N>& push(drawfn_t&& fn);
	void operator()(cairo_t* cr);
};

namespace detail
{
template<std::size_t N, typename Arg>
stack<N>&& make_stack(stack<N>&& stack, Arg fn)
{
	return std::move(stack.push(std::move(fn)));
}
template<std::size_t N, typename Arg, typename... Args>
stack<N>&& make_stack(stack<N>&& stack, Arg fn, Args... args)
{
	return make_stack(std::move(stack.push(std::move(fn))), args...);
}
}

template<typename... Args>
auto make_stack(Args&&... args) -> stack<sizeof...(Args)>
{
	return detail::make_stack<sizeof...(Args), Args...>(stack<sizeof...(Args)>{}, args...);
}

template<>
class stack <0>
{
private:
	std::vector<drawfn_t> fns;
public:
	inline stack<0>& push(drawfn_t&& fn)
	{
		fns.push_back(fn);
		return *this;
	}

	inline void operator()(cairo_t* cr)
	{
		for(auto& fn : fns)
			fn(cr);
	}
};

template<>
class stack <1>
{
private:
	drawfn_t fn;
public:
	inline stack<1>& push(drawfn_t&& fn)
	{
		this->fn = fn;
		return *this;
	}

	inline void operator()(cairo_t* cr)
	{
		fn(cr);
	}
};

template<std::size_t N>
inline stack<N>& stack<N>::push(drawfn_t&& fn)
{
	fns[n++] = fn;
	return *this;
}

template<std::size_t N>
inline void stack<N>::operator()(cairo_t* cr)
{
	for(auto& fn : fns)
		fn(cr);
}

} /* namespace cairo */
} /* namespace ui */
#endif /* UICAIROSTACK_H_ */
