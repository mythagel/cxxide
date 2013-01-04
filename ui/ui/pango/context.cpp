/*
 * context.cpp
 *
 *  Created on: 03/01/2013
 *      Author: nicholas
 */

#include "context.h"
#include <algorithm>
#include "layout.h"

void intrusive_ptr_add_ref(PangoContext* context)
{
	g_object_ref(context);
}
void intrusive_ptr_release(PangoContext* context)
{
	g_object_unref(context);
}

namespace ui
{
namespace pango
{

context_t::context_t(cairo_t* cr)
 : 	pango_context( pango_cairo_create_context(cr) )
{
	if(!pango_context)
		throw error("Unable to create pango context");
}

void context_t::update(cairo_t* cr)
{
	pango_cairo_update_context(cr, pango_context.get());

	layouts.erase(std::remove_if(layouts.begin(), layouts.end(), [](const std::weak_ptr<layout_t>& layout) -> bool { return layout.expired(); }), layouts.end());

	for(auto& layout_ref : layouts)
	{
		if(auto layout = layout_ref.lock())
			pango_layout_context_changed(layout->layout.get());
	}
}

void context_t::font(const std::string& font_spec)
{
	font_description = std::shared_ptr<PangoFontDescription>( pango_font_description_from_string(font_spec.c_str()), std::ptr_fun(pango_font_description_free) );
}

std::shared_ptr<layout_t> context_t::make_layout()
{
	auto layout = std::make_shared<layout_t>(pango_context);
	layout->font(font_description);
	layouts.push_back(layout);
	return layout;
}

context_t::~context_t()
{
}

} /* namespace pango */
} /* namespace ui */
