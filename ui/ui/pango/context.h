/*
 * context.h
 *
 *  Created on: 03/01/2013
 *      Author: nicholas
 */

#ifndef UIPANGOCONTEXT_H_
#define UIPANGOCONTEXT_H_
#include <pango/pangocairo.h>
#include <boost/intrusive_ptr.hpp>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

void intrusive_ptr_add_ref(PangoContext* context);
void intrusive_ptr_release(PangoContext* context);

namespace ui
{
namespace pango
{

class layout_t;

class context_t
{
private:
	boost::intrusive_ptr<PangoContext> pango_context;
	std::vector<std::weak_ptr<layout_t> > layouts;
	std::shared_ptr<PangoFontDescription> font_description;
public:
	struct error : public std::runtime_error
	{
		error(const std::string& what)
		 : std::runtime_error(what)
		{}
	};

	explicit context_t(cairo_t* cr);

	void update(cairo_t* cr);

	void font(const std::string& font_spec);

	std::shared_ptr<layout_t> make_layout();

	~context_t();
};

} /* namespace pango */
} /* namespace ui */
#endif /* UIPANGOCONTEXT_H_ */
