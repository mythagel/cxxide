/*
 * layout.cpp
 *
 *  Created on: 03/01/2013
 *      Author: nicholas
 */

#include "layout.h"

void intrusive_ptr_add_ref(PangoLayout* layout)
{
	g_object_ref(layout);
}
void intrusive_ptr_release(PangoLayout* layout)
{
	g_object_unref(layout);
}

//void intrusive_ptr_add_ref(PangoAttrList* list)
//{
//	pango_attr_list_ref (list);
//}
//void intrusive_ptr_release(PangoAttrList* list)
//{
//	pango_attr_list_unref (list);
//}

namespace ui
{
namespace pango
{

layout_t::layout_t(boost::intrusive_ptr<PangoContext>& context)
 : layout( pango_layout_new(context.get()) )
{
}

void layout_t::font(const std::string& font_spec)
{
	font_description = std::shared_ptr<PangoFontDescription>( pango_font_description_from_string(font_spec.c_str()), std::ptr_fun(pango_font_description_free) );
	pango_layout_set_font_description(layout.get(), font_description.get());
}

void layout_t::font(const std::shared_ptr<PangoFontDescription>& font_desc)
{
	font_description = font_desc;
	pango_layout_set_font_description(layout.get(), font_description.get());
}

void layout_t::text(const std::string& text)
{
	pango_layout_set_text(layout.get(), text.c_str(), text.size());
}

void layout_t::size(int* width, int* height)
{
	pango_layout_get_size(layout.get(), width, height);
}

void layout_t::draw(cairo_t* cr)
{
	pango_cairo_show_layout(cr, layout.get());
}

PangoLayout* layout_t::get()
{
	return layout.get();
}

layout_t::~layout_t()
{
}

} /* namespace pango */
} /* namespace ui */
