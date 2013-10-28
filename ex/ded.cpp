/*
 * main.cpp
 *
 *  Created on: 11/09/2012
 *      Author: nicholas
 */
#include <vector>
#include <set>
#include <stdint.h>
#include <cstdio>
#include "LLRB.h"

#include <stdexcept>

#include "UI/X11/WindowX11.h"
#include "UI/Drawing.h"
#include "UI/Colour.h"

// Testing only
#include "UI/Widgets/Button.h"
#include "Indexer/Indexer.h"
#include "Project/Makefile.h"
#include "Project/Target.h"
#include <iostream>
#include <fstream>
#include "Project/ListFile.h"
#include "Text/Buffer.h"
#include <string>
#include <ctime>
#include "UI/Drawable.h"
#include <boost/intrusive_ptr.hpp>
#include "UI/Text.h"

#include "UI/Signal.h"

class profiler_t
{
private:
	const char* msg;
	clock_t begin;

	inline std::size_t elapsed_ms()
	{
		return ((clock() - begin) * 1000) / CLOCKS_PER_SEC;
	}
public:
	profiler_t(const char* msg)
	 : msg(msg), begin(clock())
	{
	}

	~profiler_t()
	{
		fprintf(stderr, "%s: %zu ms\n", msg, elapsed_ms());
	}
};

class Component : public UI::Drawable
{
public:
	struct Point
	{
		double x = 0;
		double y = 0;
	};
	struct Rectangle
	{
		Point min;
		Point max;
	};
protected:
	UI::Drawing::drawfn_t dfn_;
	Rectangle rect_;

	virtual void _draw(cairo_t* cr)
	{
		dfn_(cr);
	}
public:
	Component() = default;
	virtual ~Component() = default;
};

template<typename T>
struct MouseOver
{
	UI::Signal<T*> onmouseover;
	UI::Signal<T*> onmouseout;

	void OnMouseOver()
	{
		onmouseover(this);
	}
	void OnMouseOut()
	{
		onmouseout(this);
	}
};

/*
 * Look at:
 * http://docs.garagegames.com/tge/official/content/documentation/Engine/Reference/GUI.html
 * for event distribution ideas
 */
template<typename T>
struct MouseMove
{
	UI::Signal<T*, int, int> onmousemove;

	void OnMouseMove(int x, int y)
	{
		onmousemove(this, x, y);
	}
};

struct Button
 : public Component,
   public MouseOver<Button>,
   public MouseMove<Button>
{
	Button()
	{
		onmouseover.connect([](Button* button)
		{
			using namespace UI::Drawing;

			// TODO don't recreate the stack each time.
			DrawState state;
			DrawStack<> ds;
			ds.push(RoundedRectangle(0.0, 0.0, 100.0, 25.0, 1.0, 25.0/10.0, &state));
			ds.push(DrawStack<>()
				.push(Colour(0.5, 0.5, 1.0, 1.0, &state))
				.push(Fill(true, &state))
				);
			ds.push(DrawStack<>()
				.push(Colour(0.5, 0.0, 0.0, 0.5, &state))
				.push(Line(CAIRO_LINE_CAP_BUTT, CAIRO_LINE_JOIN_MITER, 4.0, &state))
				);
			ds.push(Stroke(false, &state));

			button->dfn_ = ds;;
		});
		onmouseout.connect([](Button* button)
		{

		});
	}
};

class scoped_cairo
{
private:
	cairo_t* cr;
public:
	scoped_cairo(cairo_t* cr)
	 : cr(cr)
	{
		cairo_save(cr);
	}
	~scoped_cairo()
	{
		cairo_restore(cr);
	}
};

/*
 * I'm so sorry, you cannot be seated without the proper attire.
 * D.E.D ded.
 * The Robin Hood project.
 */
int main()
{
//	{
//		profiler_t profiler("Indexer");
//		indexer();
//	}

//	{
////		profiler_t profiler("Buffer");
//		using namespace Text;
//		Buffer buf;
//
//		assert(read_buffer("../LLRB.h", buf));
//
////		for(auto c : buf)
////			std::cout << c;
//
//		write_buffer(buf, "output.buf");
//
////		Buffer::iterator it = buf.begin();
////		std::cout << *it << "\n";
////		++it;
////		std::cout << *it << "\n";
//
//	}

//	{
//		profiler_t profiler("LLRB");
//		LLRB<uint64_t, void*> llrb;
//
//		for(int i = 0; i < 100000; ++i)
//			llrb.insert(i, (void*) "a");
//
//		for(int i = 0; i < 100000; ++i)
//			llrb.erase(i);
//
//		assert(llrb.empty());
//	}
//
//	{
//		profiler_t profiler("map");
//		std::map<uint64_t, void*> map;
//
//		for(int i = 0; i < 100000; ++i)
//			map.insert(std::make_pair(i, (void*) "a"));
//
//		for(int i = 0; i < 100000; ++i)
//			map.erase(i);
//
//		assert(map.empty());
//	}

	{
//		{
//			using namespace Build;
//	//		Command c("add_library");
//	//		c.AddArgument("die_eclipse_die");
//	//		c.AddArgument("main.cpp");
//	//		c.AddArgument("stupid name.cpp");
//	//		c.AddArgument("stupid name\"2\".cpp");
//	//		c.AddArgument("multi\nline\nargument");
//	//		std::cout << c;
//
//	//		Target t{"die_eclipse_die", Target::Type::executable};
//	//		t.AddSource("main.cpp");
//	//		t.AddSource("Target.cpp");
//	//		t.AddSource("Rect.cpp");
//	//		t.AddSource("Drawing.cpp");
//	//		t.AddSource("Makefile.cpp");
//	//		t.AddSource("Spring.cpp");
//	//		std::cout << t << "\n";
//		}
//
//		{
//			using namespace Build::List;
//
//			ListFile list;
//
//			{
//				Command cmd{"if"};
//				cmd.name_ws.emplace_back(' ', 1);
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("NOT", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("CGAL_FOUND", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				list.items.emplace_back(new Command(cmd));
//			}
//			list.items.push_back(make_ws('\n', 1));
//			list.items.push_back(make_ws(' ', 2));
//			{
//				Command cmd{"find_package"};
//				cmd.items.push_back(make_argument("CGAL", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("REQUIRED", false));
//				list.items.emplace_back(new Command(cmd));
//			}
//			list.items.push_back(make_ws('\n', 1));
//			{
//				Command cmd;
//				cmd.name = "endif";
//				list.items.emplace_back(new Command(cmd));
//			}
//
//			std::cout << list.str() << "\n";
//		}
//		{
//			using namespace Build::List;
//			ListFile list;
///*
//
//  set_target_properties( CGAL_Core PROPERTIES DEBUG_OUTPUT_NAME          "${CGAL_Core_DEBUG_MANGLED_NAME}"
//                                              RELEASE_OUTPUT_NAME        "${CGAL_Core_RELEASE_MANGLED_NAME}"
//                                              MINSIZEREL_OUTPUT_NAME     "${CGAL_Core_MINSIZEREL_MANGLED_NAME}"
//                                              RELWITHDEBINFO_OUTPUT_NAME "${CGAL_Core_RELWITHDEBINFO_MANGLED_NAME}"
//                      )
//
//*/
//			list.items.push_back(make_ws(' ', 2));
//			{
//				Command cmd{"set_target_properties"};
//				cmd.items.push_back(make_argument("CGAL_Core", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("PROPERTIES", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("DEBUG_OUTPUT_NAME", false));
//				cmd.items.push_back(make_ws(' ', 10));
//				cmd.items.push_back(make_argument("${CGAL_Core_DEBUG_MANGLED_NAME}", true));
//				cmd.items.push_back(make_ws('\n', 1));
//
//				cmd.items.push_back(make_ws(' ', 45));
//				cmd.items.push_back(make_argument("RELEASE_OUTPUT_NAME", false));
//				cmd.items.push_back(make_ws(' ', 8));
//				cmd.items.push_back(make_argument("${CGAL_Core_RELEASE_MANGLED_NAME}", true));
//				cmd.items.push_back(make_ws('\n', 1));
//
//				cmd.items.push_back(make_ws(' ', 45));
//				cmd.items.push_back(make_argument("MINSIZEREL_OUTPUT_NAME", false));
//				cmd.items.push_back(make_ws(' ', 5));
//				cmd.items.push_back(make_argument("${CGAL_Core_MINSIZEREL_MANGLED_NAME}", true));
//				cmd.items.push_back(make_ws('\n', 1));
//
//				cmd.items.push_back(make_ws(' ', 45));
//				cmd.items.push_back(make_argument("RELWITHDEBINFO_OUTPUT_NAME", false));
//				cmd.items.push_back(make_ws(' ', 1));
//				cmd.items.push_back(make_argument("${CGAL_Core_RELWITHDEBINFO_MANGLED_NAME}", true));
//				cmd.items.push_back(make_ws('\n', 1));
//
//				cmd.items.push_back(make_ws(' ', 22));
//
//				list.items.emplace_back(new Command(cmd));
//			}
//			list.items.push_back(make_ws('\n', 1));
//			std::cout << list.str() << "\n";
//		}
//		{
//			using namespace Build::List;
//			//uniquely_add_flags ( CGAL_CXX_FLAGS "-D_CRT_SECURE_NO_DEPRECATE;-D_SCL_SECURE_NO_DEPRECATE;-D_CRT_SECURE_NO_WARNINGS;-D_SCL_SECURE_NO_WARNINGS" )
//			Command cmd{"uniquely_add_flags"};
//			cmd.name_ws.emplace_back(' ', 1);
//			cmd.items.push_back(make_ws(' ', 1));
//			cmd.items.push_back(make_argument("CGAL_CXX_FLAGS", false));
//			cmd.items.push_back(make_ws(' ', 1));
//
//			SemicolonList sl;
//			sl.quoted = true;
//			sl.items.push_back(make_argument("-D_CRT_SECURE_NO_DEPRECATE", false));
//			sl.items.push_back(make_argument("-D_SCL_SECURE_NO_DEPRECATE", false));
//			sl.items.push_back(make_argument("-D_CRT_SECURE_NO_WARNINGS", false));
//			sl.items.push_back(make_argument("-D_SCL_SECURE_NO_WARNINGS", false));
//			cmd.items.emplace_back(new SemicolonList(sl));
//			cmd.items.push_back(make_ws(' ', 1));
//
//			std::cout << cmd.str() << "\n";
//		}

//		{
//			using namespace Build::List;
//			ListFile list;
//
//			std::ifstream ifs("CMakeLists2.txt");
//
//			ListFile::Parse(ifs, &list);
//
//			std::ofstream ofs("data.out");
//			ofs << list.str();
//			std::cout << list.describe();
//		}
	}

	class CustomWindow : public UI::Window<UI::Window_X11>
	{
	private:
		UI::Button b;
	public:
		CustomWindow()
		 : UI::Window<UI::Window_X11>("Die Eclipse Die.", 0, 0, 1024, 576)
		{
			UI::Drawing::DrawStack<> ds;
			UI::Drawing::DrawState state;
			ds.push(UI::Drawing::Colour(UI::Colour(77, 77, 77), 1.0, &state));
			ds.push(UI::Drawing::Paint(&state));
			m_Background = ds;

			window_event.connect([this](UI::WindowEvent we){ this->onWindowEvent(we); });
		}

		void onWindowEvent(const UI::WindowEvent& window_event)
		{
			switch(window_event.type)
			{
				case UI::WindowEvent::Type::Resize:
				{

					break;
				}
			}
		}

		static UI::Drawing::drawfn_t PowerBar(const std::string& text, UI::Drawing::DrawState* state)
		{
			using namespace UI::Drawing;

			int height = 15;
			int width = 60;

			return BuildStack(
				MoveTo(0, 0, state),
				LineTo(width - (height / 2), 0, state),
				LineTo(width, height/2, state),
				LineTo(width - (height / 2), height, state),
				LineTo(0, height, state),
				LineTo(height / 2, height / 2, state),
				LineTo(0, 0, state),
				ClosePath(state),
				Fill(false, state)
			);
		}

		virtual void _draw(cairo_t* cr)
		{
			fprintf(stderr, "draw\n");

			{
				using namespace UI;
				using namespace UI::Drawing;

				DrawStack<> left;
				DrawStack<> full;
				{
					int height = 15;
					int width = 360;

					{
						Drawing::DrawState state;
						full.push(Drawing::Colour(UI::Colour(100, 100, 100), 1.0, &state));
						full.push(MoveTo(0, 0, &state));
						full.push(LineTo(width - (height/2), 0, &state));
						full.push(LineTo(width, height/2, &state));
						full.push(LineTo(width - (height / 2), height, &state));
						full.push(LineTo(0, height, &state));
						full.push(LineTo(0, 0, &state));
						full.push(ClosePath(&state));
						full.push(Fill(false, &state));
					}
				}

				{
					int height = 15;
					int width = 60;

					{
						Drawing::DrawState state;
						left.push(MoveTo(0, 0, &state));
						left.push(LineTo(width - (height/2), 0, &state));
						left.push(LineTo(width, height/2, &state));
						left.push(LineTo(width - (height / 2), height, &state));
						left.push(LineTo(0, height, &state));
						left.push(LineTo(0, 0, &state));
						left.push(ClosePath(&state));
						left.push(Stroke(false, &state));
					}
//					{
//						Drawing::DrawState state;
//						mid.push(MoveTo(0, 0, &state));
//						mid.push(LineTo(width - (height/2), 0, &state));
//						mid.push(LineTo(width, height/2, &state));
//						mid.push(LineTo(width - (height / 2), height, &state));
//						mid.push(LineTo(0, height, &state));
//						mid.push(LineTo(height/2, height/2, &state));
//						mid.push(LineTo(0, 0, &state));
//						mid.push(ClosePath(&state));
//						mid.push(Fill(false, &state));
//					}
				}

				{
					Drawing::DrawState state;
					drawfn_t mid = PowerBar("nick", &state);

					{
						scoped_cairo save_restore(cr);
						cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
						cairo_translate(cr, 10, 10);
						mid(cr);
					}
					{
						scoped_cairo save_restore(cr);
						cairo_set_source_rgb(cr, 0.5, 1.0, 0.5);
						cairo_translate(cr, 64.5, 10);
						mid(cr);
					}
					{
						scoped_cairo save_restore(cr);
						cairo_set_source_rgb(cr, 1, 0.5, 0.5);
						cairo_translate(cr, 119.5, 10);
						mid(cr);
					}
				}
				{
					scoped_cairo save_restore(cr);
					cairo_translate(cr, 10, 40);
					full(cr);
				}
			}

			{
				scoped_cairo save_restore(cr);

				/*
				 * Context can (and should) be reused,
				 * if cr changes then:
				 *
				 * pango_cairo_update_context (cr, context)
				 * for each layout
				 *   pango_layout_context_changed(layout)
				 *
				 */

				cairo_translate(cr, 0, 100);

				context->Font("Monospace 10");

				auto layout = context->CreateLayout();

				/* Set the text for the layout - not 100% clear here
				 * I think the best thing is to pass just the text to be displayed.
				 * At most there will be a duplicate of a screen of text.
				 */
				layout->Text("void rendertext(cairo_t *cr) {\n	PangoLayout *layout;							// layout for a paragraph of text\n	PangoFontDescription *desc;						// this structure stores a description of the style of font you'd most like\n	\n	cairo_translate(cr, 10, 20);						// set the origin of cairo instance 'cr' to (10,20) (i.e. this is where\n										// drawing will start from).\n	layout = pango_cairo_create_layout(cr);					// init pango layout ready for use\n	pango_layout_set_text(layout, \"Hello World!\", -1);			// sets the text to be associated with the layout (final arg is length, -1\n										// to calculate automatically when passing a nul-terminated string)\n	desc = pango_font_description_from_string(\"Sans Bold 12\");		// specify the font that would be ideal for your particular use\n	pango_layout_set_font_description(layout, desc);			// assign the previous font description to the layout\n	pango_font_description_free(desc);					// free the description\n\n	cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);				// set the colour to blue\n	pango_cairo_update_layout(cr, layout);					// if the target surface or transformation properties of the cairo instance\n										// have changed, update the pango layout to reflect this\n	pango_cairo_show_layout(cr, layout);					// draw the pango layout onto the cairo surface\n\n	g_object_unref(layout);							// free the layout\n}");

				/* Text attributes
				 * Attributes will come from the syntax backend
				 */
				{
					boost::intrusive_ptr<PangoAttrList> list( pango_attr_list_new() );

					{
						PangoAttribute* attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
						attr->start_index = 0;
						attr->end_index = 4;
						pango_attr_list_insert(list.get(), attr);
					}

					pango_layout_set_attributes(layout->get(), list.get());
				}

				// Adjustable tab width
				{
					/*
					 * Pango takes the tab width in points/pixels
					 * Need to measure the width of the spaces to set it.
					 * Probably only need to do this if the font changes.
					 *
					 * tab_width will be updated from characters to cairo units that
					 * represent that number of space characters.
					 * needs to know the current font and size to do so.
					 */
					int tab_width = 8;
					{
						std::string spaces(tab_width, ' ');
						auto layout = context->CreateLayout();
						layout->Text(spaces);
						layout->Size(&tab_width, 0);
					}

					std::shared_ptr<PangoTabArray> tab_array( pango_tab_array_new(1, false), std::ptr_fun(pango_tab_array_free) );
					pango_tab_array_set_tab(tab_array.get(), 0, PANGO_TAB_LEFT, tab_width);
					pango_layout_set_tabs(layout->get(), tab_array.get());
				}

				// Word wrapping
				size_t wrap_width(width());
				if(wrap_width)
				{
					pango_layout_set_wrap (layout->get(), PANGO_WRAP_WORD_CHAR);
					pango_layout_set_width(layout->get(), wrap_width * PANGO_SCALE);
				}

				cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
				layout->draw(cr);
			}

//			b.draw(cr);
		}

		virtual ~CustomWindow() = default;
	};

	CustomWindow window;

	window.key_event.connect([](UI::KeyEvent ke)
		{
			if(ke.type == UI::KeyEvent::Type::Down)
				fprintf(stderr, "KeyDown: %s %d\n", ke.key.c_str(), ke.mask);
			else
				fprintf(stderr, "KeyUp  : %s %d\n", ke.key.c_str(), ke.mask);
		});
	window.mouse_event.connect([](UI::MouseEvent me)
		{
			if(me.type == UI::MouseEvent::Type::Move)
				fprintf(stderr, "Move: %d %d\n", me.position.x, me.position.y);
		});

	UI::Window<UI::Window_X11>::HandleEvents();
	fprintf(stderr, "Event loop ended\n");
	return 0;
}
