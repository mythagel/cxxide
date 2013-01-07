/*
 * scopedcontext.h
 *
 *  Created on: 07/01/2013
 *      Author: nicholas
 */

#ifndef SCOPEDCONTEXT_H_
#define SCOPEDCONTEXT_H_
#include <cairo.h>

namespace ui
{
namespace cairo
{

class scoped_context
{
private:
	cairo_t* cr;
public:
	explicit scoped_context(cairo_t* cr)
	 : cr(cr)
	{
		cairo_save(cr);
	}
	~scoped_context()
	{
		cairo_restore(cr);
	}
};

} /* namespace cairo */
} /* namespace ui */
#endif /* SCOPEDCONTEXT_H_ */
