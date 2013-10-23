/*
 * keysym.h
 *
 *  Created on: 21/12/2012
 *      Author: nicholas
 */

#ifndef UIX11KEYSYM_H_
#define UIX11KEYSYM_H_
#include <string>
#include <X11/XKBlib.h>

namespace cxxide
{
namespace x11
{

std::string wchar2utf8(wchar_t wc);
wchar_t keysym2ucs(KeySym keysym);

} /* namespace x11 */
} /* namespace ui */
#endif /* UIX11KEYSYM_H_ */
