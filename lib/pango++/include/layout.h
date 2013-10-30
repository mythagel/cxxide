/*
 * layout.h
 *
 *  Created on: 03/01/2013
 *      Author: nicholas
 */

#ifndef UIPANGOLAYOUT_H_
#define UIPANGOLAYOUT_H_
#include <pango/pangocairo.h>
#include <boost/intrusive_ptr.hpp>
#include <memory>
#include <string>
#include "context.h"

namespace cxxide
{
namespace pango
{

void intrusive_ptr_add_ref(PangoLayout* layout);
void intrusive_ptr_release(PangoLayout* layout);

//void intrusive_ptr_add_ref(PangoAttrList* list);
//void intrusive_ptr_release(PangoAttrList* list);

class layout_t
{
friend class context_t;
private:
	boost::intrusive_ptr<PangoLayout> layout;
	std::shared_ptr<PangoFontDescription> font_description;
public:
	explicit layout_t(boost::intrusive_ptr<PangoContext>& context);

	void font(const std::string& font_spec);
	void font(const std::shared_ptr<PangoFontDescription>& font_desc);
	void text(const std::string& text);
	void size(int* width = 0, int* height = 0);

	void draw(cairo_t* cr);

	PangoLayout* get();

	~layout_t();
};

} /* namespace pango */
} /* namespace ui */
#endif /* UIPANGOLAYOUT_H_ */
