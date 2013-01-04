/*
 * colour_t.cpp
 *
 *  Created on: 28/09/2012
 *      Author: nicholas
 */

#include "colour.h"

namespace ui
{
namespace cairo
{

const colour_t colour_t::indian_red = colour_t(176, 23, 31);
const colour_t colour_t::crimson = colour_t(220, 20, 60);
const colour_t colour_t::lightpink = colour_t(255, 182, 193);
const colour_t colour_t::lightpink_1 = colour_t(255, 174, 185);
const colour_t colour_t::lightpink_2 = colour_t(238, 162, 173);
const colour_t colour_t::lightpink_3 = colour_t(205, 140, 149);
const colour_t colour_t::lightpink_4 = colour_t(139, 95, 101);
const colour_t colour_t::pink = colour_t(255, 192, 203);
const colour_t colour_t::pink_1 = colour_t(255, 181, 197);
const colour_t colour_t::pink_2 = colour_t(238, 169, 184);
const colour_t colour_t::pink_3 = colour_t(205, 145, 158);
const colour_t colour_t::pink_4 = colour_t(139, 99, 108);
const colour_t colour_t::palevioletred = colour_t(219, 112, 147);
const colour_t colour_t::palevioletred_1 = colour_t(255, 130, 171);
const colour_t colour_t::palevioletred_2 = colour_t(238, 121, 159);
const colour_t colour_t::palevioletred_3 = colour_t(205, 104, 137);
const colour_t colour_t::palevioletred_4 = colour_t(139, 71, 93);
const colour_t colour_t::lavenderblush_1__lavenderblush_ = colour_t(255, 240, 245);
const colour_t colour_t::lavenderblush_2 = colour_t(238, 224, 229);
const colour_t colour_t::lavenderblush_3 = colour_t(205, 193, 197);
const colour_t colour_t::lavenderblush_4 = colour_t(139, 131, 134);
const colour_t colour_t::violetred_1 = colour_t(255, 62, 150);
const colour_t colour_t::violetred_2 = colour_t(238, 58, 140);
const colour_t colour_t::violetred_3 = colour_t(205, 50, 120);
const colour_t colour_t::violetred_4 = colour_t(139, 34, 82);
const colour_t colour_t::hotpink = colour_t(255, 105, 180);
const colour_t colour_t::hotpink_1 = colour_t(255, 110, 180);
const colour_t colour_t::hotpink_2 = colour_t(238, 106, 167);
const colour_t colour_t::hotpink_3 = colour_t(205, 96, 144);
const colour_t colour_t::hotpink_4 = colour_t(139, 58, 98);
const colour_t colour_t::raspberry = colour_t(135, 38, 87);
const colour_t colour_t::deeppink_1__deeppink_ = colour_t(255, 20, 147);
const colour_t colour_t::deeppink_2 = colour_t(238, 18, 137);
const colour_t colour_t::deeppink_3 = colour_t(205, 16, 118);
const colour_t colour_t::deeppink_4 = colour_t(139, 10, 80);
const colour_t colour_t::maroon_1 = colour_t(255, 52, 179);
const colour_t colour_t::maroon_2 = colour_t(238, 48, 167);
const colour_t colour_t::maroon_3 = colour_t(205, 41, 144);
const colour_t colour_t::maroon_4 = colour_t(139, 28, 98);
const colour_t colour_t::mediumvioletred = colour_t(199, 21, 133);
const colour_t colour_t::violetred = colour_t(208, 32, 144);
const colour_t colour_t::orchid = colour_t(218, 112, 214);
const colour_t colour_t::orchid_1 = colour_t(255, 131, 250);
const colour_t colour_t::orchid_2 = colour_t(238, 122, 233);
const colour_t colour_t::orchid_3 = colour_t(205, 105, 201);
const colour_t colour_t::orchid_4 = colour_t(139, 71, 137);
const colour_t colour_t::thistle = colour_t(216, 191, 216);
const colour_t colour_t::thistle_1 = colour_t(255, 225, 255);
const colour_t colour_t::thistle_2 = colour_t(238, 210, 238);
const colour_t colour_t::thistle_3 = colour_t(205, 181, 205);
const colour_t colour_t::thistle_4 = colour_t(139, 123, 139);
const colour_t colour_t::plum_1 = colour_t(255, 187, 255);
const colour_t colour_t::plum_2 = colour_t(238, 174, 238);
const colour_t colour_t::plum_3 = colour_t(205, 150, 205);
const colour_t colour_t::plum_4 = colour_t(139, 102, 139);
const colour_t colour_t::plum = colour_t(221, 160, 221);
const colour_t colour_t::violet = colour_t(238, 130, 238);
const colour_t colour_t::magenta__fuchsia_vga_ = colour_t(255, 0, 255);
const colour_t colour_t::magenta_2 = colour_t(238, 0, 238);
const colour_t colour_t::magenta_3 = colour_t(205, 0, 205);
const colour_t colour_t::magenta_4__darkmagenta_ = colour_t(139, 0, 139);
const colour_t colour_t::purple_vga = colour_t(128, 0, 128);
const colour_t colour_t::mediumorchid = colour_t(186, 85, 211);
const colour_t colour_t::mediumorchid_1 = colour_t(224, 102, 255);
const colour_t colour_t::mediumorchid_2 = colour_t(209, 95, 238);
const colour_t colour_t::mediumorchid_3 = colour_t(180, 82, 205);
const colour_t colour_t::mediumorchid_4 = colour_t(122, 55, 139);
const colour_t colour_t::darkviolet = colour_t(148, 0, 211);
const colour_t colour_t::darkorchid = colour_t(153, 50, 204);
const colour_t colour_t::darkorchid_1 = colour_t(191, 62, 255);
const colour_t colour_t::darkorchid_2 = colour_t(178, 58, 238);
const colour_t colour_t::darkorchid_3 = colour_t(154, 50, 205);
const colour_t colour_t::darkorchid_4 = colour_t(104, 34, 139);
const colour_t colour_t::indigo = colour_t(75, 0, 130);
const colour_t colour_t::blueviolet = colour_t(138, 43, 226);
const colour_t colour_t::purple_1 = colour_t(155, 48, 255);
const colour_t colour_t::purple_2 = colour_t(145, 44, 238);
const colour_t colour_t::purple_3 = colour_t(125, 38, 205);
const colour_t colour_t::purple_4 = colour_t(85, 26, 139);
const colour_t colour_t::mediumpurple = colour_t(147, 112, 219);
const colour_t colour_t::mediumpurple_1 = colour_t(171, 130, 255);
const colour_t colour_t::mediumpurple_2 = colour_t(159, 121, 238);
const colour_t colour_t::mediumpurple_3 = colour_t(137, 104, 205);
const colour_t colour_t::mediumpurple_4 = colour_t(93, 71, 139);
const colour_t colour_t::darkslateblue = colour_t(72, 61, 139);
const colour_t colour_t::lightslateblue = colour_t(132, 112, 255);
const colour_t colour_t::mediumslateblue = colour_t(123, 104, 238);
const colour_t colour_t::slateblue = colour_t(106, 90, 205);
const colour_t colour_t::slateblue_1 = colour_t(131, 111, 255);
const colour_t colour_t::slateblue_2 = colour_t(122, 103, 238);
const colour_t colour_t::slateblue_3 = colour_t(105, 89, 205);
const colour_t colour_t::slateblue_4 = colour_t(71, 60, 139);
const colour_t colour_t::ghostwhite = colour_t(248, 248, 255);
const colour_t colour_t::lavender = colour_t(230, 230, 250);
const colour_t colour_t::blue_vga = colour_t(0, 0, 255);
const colour_t colour_t::blue_2 = colour_t(0, 0, 238);
const colour_t colour_t::blue_3__mediumblue_ = colour_t(0, 0, 205);
const colour_t colour_t::blue_4__darkblue_ = colour_t(0, 0, 139);
const colour_t colour_t::navy_vga = colour_t(0, 0, 128);
const colour_t colour_t::midnightblue = colour_t(25, 25, 112);
const colour_t colour_t::cobalt = colour_t(61, 89, 171);
const colour_t colour_t::royalblue = colour_t(65, 105, 225);
const colour_t colour_t::royalblue_1 = colour_t(72, 118, 255);
const colour_t colour_t::royalblue_2 = colour_t(67, 110, 238);
const colour_t colour_t::royalblue_3 = colour_t(58, 95, 205);
const colour_t colour_t::royalblue_4 = colour_t(39, 64, 139);
const colour_t colour_t::cornflowerblue = colour_t(100, 149, 237);
const colour_t colour_t::lightsteelblue = colour_t(176, 196, 222);
const colour_t colour_t::lightsteelblue_1 = colour_t(202, 225, 255);
const colour_t colour_t::lightsteelblue_2 = colour_t(188, 210, 238);
const colour_t colour_t::lightsteelblue_3 = colour_t(162, 181, 205);
const colour_t colour_t::lightsteelblue_4 = colour_t(110, 123, 139);
const colour_t colour_t::lightslategray = colour_t(119, 136, 153);
const colour_t colour_t::slategray = colour_t(112, 128, 144);
const colour_t colour_t::slategray_1 = colour_t(198, 226, 255);
const colour_t colour_t::slategray_2 = colour_t(185, 211, 238);
const colour_t colour_t::slategray_3 = colour_t(159, 182, 205);
const colour_t colour_t::slategray_4 = colour_t(108, 123, 139);
const colour_t colour_t::dodgerblue_1__dodgerblue_ = colour_t(30, 144, 255);
const colour_t colour_t::dodgerblue_2 = colour_t(28, 134, 238);
const colour_t colour_t::dodgerblue_3 = colour_t(24, 116, 205);
const colour_t colour_t::dodgerblue_4 = colour_t(16, 78, 139);
const colour_t colour_t::aliceblue = colour_t(240, 248, 255);
const colour_t colour_t::steelblue = colour_t(70, 130, 180);
const colour_t colour_t::steelblue_1 = colour_t(99, 184, 255);
const colour_t colour_t::steelblue_2 = colour_t(92, 172, 238);
const colour_t colour_t::steelblue_3 = colour_t(79, 148, 205);
const colour_t colour_t::steelblue_4 = colour_t(54, 100, 139);
const colour_t colour_t::lightskyblue = colour_t(135, 206, 250);
const colour_t colour_t::lightskyblue_1 = colour_t(176, 226, 255);
const colour_t colour_t::lightskyblue_2 = colour_t(164, 211, 238);
const colour_t colour_t::lightskyblue_3 = colour_t(141, 182, 205);
const colour_t colour_t::lightskyblue_4 = colour_t(96, 123, 139);
const colour_t colour_t::skyblue_1 = colour_t(135, 206, 255);
const colour_t colour_t::skyblue_2 = colour_t(126, 192, 238);
const colour_t colour_t::skyblue_3 = colour_t(108, 166, 205);
const colour_t colour_t::skyblue_4 = colour_t(74, 112, 139);
const colour_t colour_t::skyblue = colour_t(135, 206, 235);
const colour_t colour_t::deepskyblue_1__deepskyblue_ = colour_t(0, 191, 255);
const colour_t colour_t::deepskyblue_2 = colour_t(0, 178, 238);
const colour_t colour_t::deepskyblue_3 = colour_t(0, 154, 205);
const colour_t colour_t::deepskyblue_4 = colour_t(0, 104, 139);
const colour_t colour_t::peacock = colour_t(51, 161, 201);
const colour_t colour_t::lightblue = colour_t(173, 216, 230);
const colour_t colour_t::lightblue_1 = colour_t(191, 239, 255);
const colour_t colour_t::lightblue_2 = colour_t(178, 223, 238);
const colour_t colour_t::lightblue_3 = colour_t(154, 192, 205);
const colour_t colour_t::lightblue_4 = colour_t(104, 131, 139);
const colour_t colour_t::powderblue = colour_t(176, 224, 230);
const colour_t colour_t::cadetblue_1 = colour_t(152, 245, 255);
const colour_t colour_t::cadetblue_2 = colour_t(142, 229, 238);
const colour_t colour_t::cadetblue_3 = colour_t(122, 197, 205);
const colour_t colour_t::cadetblue_4 = colour_t(83, 134, 139);
const colour_t colour_t::turquoise_1 = colour_t(0, 245, 255);
const colour_t colour_t::turquoise_2 = colour_t(0, 229, 238);
const colour_t colour_t::turquoise_3 = colour_t(0, 197, 205);
const colour_t colour_t::turquoise_4 = colour_t(0, 134, 139);
const colour_t colour_t::cadetblue = colour_t(95, 158, 160);
const colour_t colour_t::darkturquoise = colour_t(0, 206, 209);
const colour_t colour_t::azure_1__azure_ = colour_t(240, 255, 255);
const colour_t colour_t::azure_2 = colour_t(224, 238, 238);
const colour_t colour_t::azure_3 = colour_t(193, 205, 205);
const colour_t colour_t::azure_4 = colour_t(131, 139, 139);
const colour_t colour_t::lightcyan_1__lightcyan_ = colour_t(224, 255, 255);
const colour_t colour_t::lightcyan_2 = colour_t(209, 238, 238);
const colour_t colour_t::lightcyan_3 = colour_t(180, 205, 205);
const colour_t colour_t::lightcyan_4 = colour_t(122, 139, 139);
const colour_t colour_t::paleturquoise_1 = colour_t(187, 255, 255);
const colour_t colour_t::paleturquoise_2__paleturquoise_ = colour_t(174, 238, 238);
const colour_t colour_t::paleturquoise_3 = colour_t(150, 205, 205);
const colour_t colour_t::paleturquoise_4 = colour_t(102, 139, 139);
const colour_t colour_t::darkslategray = colour_t(47, 79, 79);
const colour_t colour_t::darkslategray_1 = colour_t(151, 255, 255);
const colour_t colour_t::darkslategray_2 = colour_t(141, 238, 238);
const colour_t colour_t::darkslategray_3 = colour_t(121, 205, 205);
const colour_t colour_t::darkslategray_4 = colour_t(82, 139, 139);
const colour_t colour_t::cyan__aqua_vga = colour_t(0, 255, 255);
const colour_t colour_t::cyan_2 = colour_t(0, 238, 238);
const colour_t colour_t::cyan_3 = colour_t(0, 205, 205);
const colour_t colour_t::cyan_4__darkcyan_ = colour_t(0, 139, 139);
const colour_t colour_t::teal_vga = colour_t(0, 128, 128);
const colour_t colour_t::mediumturquoise = colour_t(72, 209, 204);
const colour_t colour_t::lightseagreen = colour_t(32, 178, 170);
const colour_t colour_t::manganeseblue = colour_t(3, 168, 158);
const colour_t colour_t::turquoise = colour_t(64, 224, 208);
const colour_t colour_t::coldgrey = colour_t(128, 138, 135);
const colour_t colour_t::turquoiseblue = colour_t(0, 199, 140);
const colour_t colour_t::aquamarine_1__aquamarine_ = colour_t(127, 255, 212);
const colour_t colour_t::aquamarine_2 = colour_t(118, 238, 198);
const colour_t colour_t::aquamarine_3__mediumaquamarine_ = colour_t(102, 205, 170);
const colour_t colour_t::aquamarine_4 = colour_t(69, 139, 116);
const colour_t colour_t::mediumspringgreen = colour_t(0, 250, 154);
const colour_t colour_t::mintcream = colour_t(245, 255, 250);
const colour_t colour_t::springgreen = colour_t(0, 255, 127);
const colour_t colour_t::springgreen_1 = colour_t(0, 238, 118);
const colour_t colour_t::springgreen_2 = colour_t(0, 205, 102);
const colour_t colour_t::springgreen_3 = colour_t(0, 139, 69);
const colour_t colour_t::mediumseagreen = colour_t(60, 179, 113);
const colour_t colour_t::seagreen_1 = colour_t(84, 255, 159);
const colour_t colour_t::seagreen_2 = colour_t(78, 238, 148);
const colour_t colour_t::seagreen_3 = colour_t(67, 205, 128);
const colour_t colour_t::seagreen_4__seagreen_ = colour_t(46, 139, 87);
const colour_t colour_t::emeraldgreen = colour_t(0, 201, 87);
const colour_t colour_t::mint = colour_t(189, 252, 201);
const colour_t colour_t::cobaltgreen = colour_t(61, 145, 64);
const colour_t colour_t::honeydew_1__honeydew_ = colour_t(240, 255, 240);
const colour_t colour_t::honeydew_2 = colour_t(224, 238, 224);
const colour_t colour_t::honeydew_3 = colour_t(193, 205, 193);
const colour_t colour_t::honeydew_4 = colour_t(131, 139, 131);
const colour_t colour_t::darkseagreen = colour_t(143, 188, 143);
const colour_t colour_t::darkseagreen_1 = colour_t(193, 255, 193);
const colour_t colour_t::darkseagreen_2 = colour_t(180, 238, 180);
const colour_t colour_t::darkseagreen_3 = colour_t(155, 205, 155);
const colour_t colour_t::darkseagreen_4 = colour_t(105, 139, 105);
const colour_t colour_t::palegreen = colour_t(152, 251, 152);
const colour_t colour_t::palegreen_1 = colour_t(154, 255, 154);
const colour_t colour_t::palegreen_2__lightgreen_ = colour_t(144, 238, 144);
const colour_t colour_t::palegreen_3 = colour_t(124, 205, 124);
const colour_t colour_t::palegreen_4 = colour_t(84, 139, 84);
const colour_t colour_t::limegreen = colour_t(50, 205, 50);
const colour_t colour_t::forestgreen = colour_t(34, 139, 34);
const colour_t colour_t::green_1__lime_vga_ = colour_t(0, 255, 0);
const colour_t colour_t::green_2 = colour_t(0, 238, 0);
const colour_t colour_t::green_3 = colour_t(0, 205, 0);
const colour_t colour_t::green_4 = colour_t(0, 139, 0);
const colour_t colour_t::green_vga = colour_t(0, 128, 0);
const colour_t colour_t::darkgreen = colour_t(0, 100, 0);
const colour_t colour_t::sapgreen = colour_t(48, 128, 20);
const colour_t colour_t::lawngreen = colour_t(124, 252, 0);
const colour_t colour_t::chartreuse_1__chartreuse_ = colour_t(127, 255, 0);
const colour_t colour_t::chartreuse_2 = colour_t(118, 238, 0);
const colour_t colour_t::chartreuse_3 = colour_t(102, 205, 0);
const colour_t colour_t::chartreuse_4 = colour_t(69, 139, 0);
const colour_t colour_t::greenyellow = colour_t(173, 255, 47);
const colour_t colour_t::darkolivegreen_1 = colour_t(202, 255, 112);
const colour_t colour_t::darkolivegreen_2 = colour_t(188, 238, 104);
const colour_t colour_t::darkolivegreen_3 = colour_t(162, 205, 90);
const colour_t colour_t::darkolivegreen_4 = colour_t(110, 139, 61);
const colour_t colour_t::darkolivegreen = colour_t(85, 107, 47);
const colour_t colour_t::olivedrab = colour_t(107, 142, 35);
const colour_t colour_t::olivedrab_1 = colour_t(192, 255, 62);
const colour_t colour_t::olivedrab_2 = colour_t(179, 238, 58);
const colour_t colour_t::olivedrab_3__yellowgreen_ = colour_t(154, 205, 50);
const colour_t colour_t::olivedrab_4 = colour_t(105, 139, 34);
const colour_t colour_t::ivory_1__ivory_ = colour_t(255, 255, 240);
const colour_t colour_t::ivory_2 = colour_t(238, 238, 224);
const colour_t colour_t::ivory_3 = colour_t(205, 205, 193);
const colour_t colour_t::ivory_4 = colour_t(139, 139, 131);
const colour_t colour_t::beige = colour_t(245, 245, 220);
const colour_t colour_t::lightyellow_1__lightyellow_ = colour_t(255, 255, 224);
const colour_t colour_t::lightyellow_2 = colour_t(238, 238, 209);
const colour_t colour_t::lightyellow_3 = colour_t(205, 205, 180);
const colour_t colour_t::lightyellow_4 = colour_t(139, 139, 122);
const colour_t colour_t::lightgoldenrodyellow = colour_t(250, 250, 210);
const colour_t colour_t::yellow_1__yellow_vga_ = colour_t(255, 255, 0);
const colour_t colour_t::yellow_2 = colour_t(238, 238, 0);
const colour_t colour_t::yellow_3 = colour_t(205, 205, 0);
const colour_t colour_t::yellow_4 = colour_t(139, 139, 0);
const colour_t colour_t::warmgrey = colour_t(128, 128, 105);
const colour_t colour_t::olive_vga = colour_t(128, 128, 0);
const colour_t colour_t::darkkhaki = colour_t(189, 183, 107);
const colour_t colour_t::khaki_1 = colour_t(255, 246, 143);
const colour_t colour_t::khaki_2 = colour_t(238, 230, 133);
const colour_t colour_t::khaki_3 = colour_t(205, 198, 115);
const colour_t colour_t::khaki_4 = colour_t(139, 134, 78);
const colour_t colour_t::khaki = colour_t(240, 230, 140);
const colour_t colour_t::palegoldenrod = colour_t(238, 232, 170);
const colour_t colour_t::lemonchiffon_1__lemonchiffon_ = colour_t(255, 250, 205);
const colour_t colour_t::lemonchiffon_2 = colour_t(238, 233, 191);
const colour_t colour_t::lemonchiffon_3 = colour_t(205, 201, 165);
const colour_t colour_t::lemonchiffon_4 = colour_t(139, 137, 112);
const colour_t colour_t::lightgoldenrod_1 = colour_t(255, 236, 139);
const colour_t colour_t::lightgoldenrod_2 = colour_t(238, 220, 130);
const colour_t colour_t::lightgoldenrod_3 = colour_t(205, 190, 112);
const colour_t colour_t::lightgoldenrod_4 = colour_t(139, 129, 76);
const colour_t colour_t::banana = colour_t(227, 207, 87);
const colour_t colour_t::gold_1__gold_ = colour_t(255, 215, 0);
const colour_t colour_t::gold_2 = colour_t(238, 201, 0);
const colour_t colour_t::gold_3 = colour_t(205, 173, 0);
const colour_t colour_t::gold_4 = colour_t(139, 117, 0);
const colour_t colour_t::cornsilk_1__cornsilk_ = colour_t(255, 248, 220);
const colour_t colour_t::cornsilk_2 = colour_t(238, 232, 205);
const colour_t colour_t::cornsilk_3 = colour_t(205, 200, 177);
const colour_t colour_t::cornsilk_4 = colour_t(139, 136, 120);
const colour_t colour_t::goldenrod = colour_t(218, 165, 32);
const colour_t colour_t::goldenrod_1 = colour_t(255, 193, 37);
const colour_t colour_t::goldenrod_2 = colour_t(238, 180, 34);
const colour_t colour_t::goldenrod_3 = colour_t(205, 155, 29);
const colour_t colour_t::goldenrod_4 = colour_t(139, 105, 20);
const colour_t colour_t::darkgoldenrod = colour_t(184, 134, 11);
const colour_t colour_t::darkgoldenrod_1 = colour_t(255, 185, 15);
const colour_t colour_t::darkgoldenrod_2 = colour_t(238, 173, 14);
const colour_t colour_t::darkgoldenrod_3 = colour_t(205, 149, 12);
const colour_t colour_t::darkgoldenrod_4 = colour_t(139, 101, 8);
const colour_t colour_t::orange_1__orange_ = colour_t(255, 165, 0);
const colour_t colour_t::orange_2 = colour_t(238, 154, 0);
const colour_t colour_t::orange_3 = colour_t(205, 133, 0);
const colour_t colour_t::orange_4 = colour_t(139, 90, 0);
const colour_t colour_t::floralwhite = colour_t(255, 250, 240);
const colour_t colour_t::oldlace = colour_t(253, 245, 230);
const colour_t colour_t::wheat = colour_t(245, 222, 179);
const colour_t colour_t::wheat_1 = colour_t(255, 231, 186);
const colour_t colour_t::wheat_2 = colour_t(238, 216, 174);
const colour_t colour_t::wheat_3 = colour_t(205, 186, 150);
const colour_t colour_t::wheat_4 = colour_t(139, 126, 102);
const colour_t colour_t::moccasin = colour_t(255, 228, 181);
const colour_t colour_t::papayawhip = colour_t(255, 239, 213);
const colour_t colour_t::blanchedalmond = colour_t(255, 235, 205);
const colour_t colour_t::navajowhite_1__navajowhite_ = colour_t(255, 222, 173);
const colour_t colour_t::navajowhite_2 = colour_t(238, 207, 161);
const colour_t colour_t::navajowhite_3 = colour_t(205, 179, 139);
const colour_t colour_t::navajowhite_4 = colour_t(139, 121, 94);
const colour_t colour_t::eggshell = colour_t(252, 230, 201);
const colour_t colour_t::tan = colour_t(210, 180, 140);
const colour_t colour_t::brick = colour_t(156, 102, 31);
const colour_t colour_t::cadmiumyellow = colour_t(255, 153, 18);
const colour_t colour_t::antiquewhite = colour_t(250, 235, 215);
const colour_t colour_t::antiquewhite_1 = colour_t(255, 239, 219);
const colour_t colour_t::antiquewhite_2 = colour_t(238, 223, 204);
const colour_t colour_t::antiquewhite_3 = colour_t(205, 192, 176);
const colour_t colour_t::antiquewhite_4 = colour_t(139, 131, 120);
const colour_t colour_t::burlywood = colour_t(222, 184, 135);
const colour_t colour_t::burlywood_1 = colour_t(255, 211, 155);
const colour_t colour_t::burlywood_2 = colour_t(238, 197, 145);
const colour_t colour_t::burlywood_3 = colour_t(205, 170, 125);
const colour_t colour_t::burlywood_4 = colour_t(139, 115, 85);
const colour_t colour_t::bisque_1__bisque_ = colour_t(255, 228, 196);
const colour_t colour_t::bisque_2 = colour_t(238, 213, 183);
const colour_t colour_t::bisque_3 = colour_t(205, 183, 158);
const colour_t colour_t::bisque_4 = colour_t(139, 125, 107);
const colour_t colour_t::melon = colour_t(227, 168, 105);
const colour_t colour_t::carrot = colour_t(237, 145, 33);
const colour_t colour_t::darkorange = colour_t(255, 140, 0);
const colour_t colour_t::darkorange_1 = colour_t(255, 127, 0);
const colour_t colour_t::darkorange_2 = colour_t(238, 118, 0);
const colour_t colour_t::darkorange_3 = colour_t(205, 102, 0);
const colour_t colour_t::darkorange_4 = colour_t(139, 69, 0);
const colour_t colour_t::orange = colour_t(255, 128, 0);
const colour_t colour_t::tan_1 = colour_t(255, 165, 79);
const colour_t colour_t::tan_2 = colour_t(238, 154, 73);
const colour_t colour_t::tan_3__peru_ = colour_t(205, 133, 63);
const colour_t colour_t::tan_4 = colour_t(139, 90, 43);
const colour_t colour_t::linen = colour_t(250, 240, 230);
const colour_t colour_t::peachpuff_1__peachpuff_ = colour_t(255, 218, 185);
const colour_t colour_t::peachpuff_2 = colour_t(238, 203, 173);
const colour_t colour_t::peachpuff_3 = colour_t(205, 175, 149);
const colour_t colour_t::peachpuff_4 = colour_t(139, 119, 101);
const colour_t colour_t::seashell_1__seashell_ = colour_t(255, 245, 238);
const colour_t colour_t::seashell_2 = colour_t(238, 229, 222);
const colour_t colour_t::seashell_3 = colour_t(205, 197, 191);
const colour_t colour_t::seashell_4 = colour_t(139, 134, 130);
const colour_t colour_t::sandybrown = colour_t(244, 164, 96);
const colour_t colour_t::rawsienna = colour_t(199, 97, 20);
const colour_t colour_t::chocolate = colour_t(210, 105, 30);
const colour_t colour_t::chocolate_1 = colour_t(255, 127, 36);
const colour_t colour_t::chocolate_2 = colour_t(238, 118, 33);
const colour_t colour_t::chocolate_3 = colour_t(205, 102, 29);
const colour_t colour_t::chocolate_4__saddlebrown_ = colour_t(139, 69, 19);
const colour_t colour_t::ivoryblack = colour_t(41, 36, 33);
const colour_t colour_t::flesh = colour_t(255, 125, 64);
const colour_t colour_t::cadmiumorange = colour_t(255, 97, 3);
const colour_t colour_t::burntsienna = colour_t(138, 54, 15);
const colour_t colour_t::sienna = colour_t(160, 82, 45);
const colour_t colour_t::sienna_1 = colour_t(255, 130, 71);
const colour_t colour_t::sienna_2 = colour_t(238, 121, 66);
const colour_t colour_t::sienna_3 = colour_t(205, 104, 57);
const colour_t colour_t::sienna_4 = colour_t(139, 71, 38);
const colour_t colour_t::lightsalmon_1__lightsalmon_ = colour_t(255, 160, 122);
const colour_t colour_t::lightsalmon_2 = colour_t(238, 149, 114);
const colour_t colour_t::lightsalmon_3 = colour_t(205, 129, 98);
const colour_t colour_t::lightsalmon_4 = colour_t(139, 87, 66);
const colour_t colour_t::coral = colour_t(255, 127, 80);
const colour_t colour_t::orangered_1__orangered_ = colour_t(255, 69, 0);
const colour_t colour_t::orangered_2 = colour_t(238, 64, 0);
const colour_t colour_t::orangered_3 = colour_t(205, 55, 0);
const colour_t colour_t::orangered_4 = colour_t(139, 37, 0);
const colour_t colour_t::sepia = colour_t(94, 38, 18);
const colour_t colour_t::darksalmon = colour_t(233, 150, 122);
const colour_t colour_t::salmon_1 = colour_t(255, 140, 105);
const colour_t colour_t::salmon_2 = colour_t(238, 130, 98);
const colour_t colour_t::salmon_3 = colour_t(205, 112, 84);
const colour_t colour_t::salmon_4 = colour_t(139, 76, 57);
const colour_t colour_t::coral_1 = colour_t(255, 114, 86);
const colour_t colour_t::coral_2 = colour_t(238, 106, 80);
const colour_t colour_t::coral_3 = colour_t(205, 91, 69);
const colour_t colour_t::coral_4 = colour_t(139, 62, 47);
const colour_t colour_t::burntumber = colour_t(138, 51, 36);
const colour_t colour_t::tomato_1__tomato_ = colour_t(255, 99, 71);
const colour_t colour_t::tomato_2 = colour_t(238, 92, 66);
const colour_t colour_t::tomato_3 = colour_t(205, 79, 57);
const colour_t colour_t::tomato_4 = colour_t(139, 54, 38);
const colour_t colour_t::salmon = colour_t(250, 128, 114);
const colour_t colour_t::mistyrose_1__mistyrose_ = colour_t(255, 228, 225);
const colour_t colour_t::mistyrose_2 = colour_t(238, 213, 210);
const colour_t colour_t::mistyrose_3 = colour_t(205, 183, 181);
const colour_t colour_t::mistyrose_4 = colour_t(139, 125, 123);
const colour_t colour_t::snow_1__snow_ = colour_t(255, 250, 250);
const colour_t colour_t::snow_2 = colour_t(238, 233, 233);
const colour_t colour_t::snow_3 = colour_t(205, 201, 201);
const colour_t colour_t::snow_4 = colour_t(139, 137, 137);
const colour_t colour_t::rosybrown = colour_t(188, 143, 143);
const colour_t colour_t::rosybrown_1 = colour_t(255, 193, 193);
const colour_t colour_t::rosybrown_2 = colour_t(238, 180, 180);
const colour_t colour_t::rosybrown_3 = colour_t(205, 155, 155);
const colour_t colour_t::rosybrown_4 = colour_t(139, 105, 105);
const colour_t colour_t::lightcoral = colour_t(240, 128, 128);
const colour_t colour_t::indianred = colour_t(205, 92, 92);
const colour_t colour_t::indianred_1 = colour_t(255, 106, 106);
const colour_t colour_t::indianred_2 = colour_t(238, 99, 99);
const colour_t colour_t::indianred_4 = colour_t(139, 58, 58);
const colour_t colour_t::indianred_3 = colour_t(205, 85, 85);
const colour_t colour_t::brown = colour_t(165, 42, 42);
const colour_t colour_t::brown_1 = colour_t(255, 64, 64);
const colour_t colour_t::brown_2 = colour_t(238, 59, 59);
const colour_t colour_t::brown_3 = colour_t(205, 51, 51);
const colour_t colour_t::brown_4 = colour_t(139, 35, 35);
const colour_t colour_t::firebrick = colour_t(178, 34, 34);
const colour_t colour_t::firebrick_1 = colour_t(255, 48, 48);
const colour_t colour_t::firebrick_2 = colour_t(238, 44, 44);
const colour_t colour_t::firebrick_3 = colour_t(205, 38, 38);
const colour_t colour_t::firebrick_4 = colour_t(139, 26, 26);
const colour_t colour_t::red_1__red_vga_ = colour_t(255, 0, 0);
const colour_t colour_t::red_2 = colour_t(238, 0, 0);
const colour_t colour_t::red_3 = colour_t(205, 0, 0);
const colour_t colour_t::red_4__darkred_ = colour_t(139, 0, 0);
const colour_t colour_t::maroon_vga = colour_t(128, 0, 0);
const colour_t colour_t::sgi_beet = colour_t(142, 56, 142);
const colour_t colour_t::sgi_slateblue = colour_t(113, 113, 198);
const colour_t colour_t::sgi_lightblue = colour_t(125, 158, 192);
const colour_t colour_t::sgi_teal = colour_t(56, 142, 142);
const colour_t colour_t::sgi_chartreuse = colour_t(113, 198, 113);
const colour_t colour_t::sgi_olivedrab = colour_t(142, 142, 56);
const colour_t colour_t::sgi_brightgray = colour_t(197, 193, 170);
const colour_t colour_t::sgi_salmon = colour_t(198, 113, 113);
const colour_t colour_t::sgi_darkgray = colour_t(85, 85, 85);
const colour_t colour_t::sgi_gray_12 = colour_t(30, 30, 30);
const colour_t colour_t::sgi_gray_16 = colour_t(40, 40, 40);
const colour_t colour_t::sgi_gray_32 = colour_t(81, 81, 81);
const colour_t colour_t::sgi_gray_36 = colour_t(91, 91, 91);
const colour_t colour_t::sgi_gray_52 = colour_t(132, 132, 132);
const colour_t colour_t::sgi_gray_56 = colour_t(142, 142, 142);
const colour_t colour_t::sgi_lightgray = colour_t(170, 170, 170);
const colour_t colour_t::sgi_gray_72 = colour_t(183, 183, 183);
const colour_t colour_t::sgi_gray_76 = colour_t(193, 193, 193);
const colour_t colour_t::sgi_gray_92 = colour_t(234, 234, 234);
const colour_t colour_t::sgi_gray_96 = colour_t(244, 244, 244);
const colour_t colour_t::white_vga = colour_t(255, 255, 255);
const colour_t colour_t::gainsboro = colour_t(220, 220, 220);
const colour_t colour_t::lightgrey = colour_t(211, 211, 211);
const colour_t colour_t::silver_vga = colour_t(192, 192, 192);
const colour_t colour_t::darkgray = colour_t(169, 169, 169);
const colour_t colour_t::gray_vga = colour_t(128, 128, 128);
const colour_t colour_t::black_vga = colour_t(0, 0, 0);
const colour_t colour_t::gray_99 = colour_t(252, 252, 252);
const colour_t colour_t::gray_98 = colour_t(250, 250, 250);
const colour_t colour_t::gray_97 = colour_t(247, 247, 247);
const colour_t colour_t::white_smoke__gray_96_ = colour_t(245, 245, 245);
const colour_t colour_t::gray_95 = colour_t(242, 242, 242);
const colour_t colour_t::gray_94 = colour_t(240, 240, 240);
const colour_t colour_t::gray_93 = colour_t(237, 237, 237);
const colour_t colour_t::gray_92 = colour_t(235, 235, 235);
const colour_t colour_t::gray_91 = colour_t(232, 232, 232);
const colour_t colour_t::gray_90 = colour_t(229, 229, 229);
const colour_t colour_t::gray_89 = colour_t(227, 227, 227);
const colour_t colour_t::gray_88 = colour_t(224, 224, 224);
const colour_t colour_t::gray_87 = colour_t(222, 222, 222);
const colour_t colour_t::gray_86 = colour_t(219, 219, 219);
const colour_t colour_t::gray_85 = colour_t(217, 217, 217);
const colour_t colour_t::gray_84 = colour_t(214, 214, 214);
const colour_t colour_t::gray_83 = colour_t(212, 212, 212);
const colour_t colour_t::gray_82 = colour_t(209, 209, 209);
const colour_t colour_t::gray_81 = colour_t(207, 207, 207);
const colour_t colour_t::gray_80 = colour_t(204, 204, 204);
const colour_t colour_t::gray_79 = colour_t(201, 201, 201);
const colour_t colour_t::gray_78 = colour_t(199, 199, 199);
const colour_t colour_t::gray_77 = colour_t(196, 196, 196);
const colour_t colour_t::gray_76 = colour_t(194, 194, 194);
const colour_t colour_t::gray_75 = colour_t(191, 191, 191);
const colour_t colour_t::gray_74 = colour_t(189, 189, 189);
const colour_t colour_t::gray_73 = colour_t(186, 186, 186);
const colour_t colour_t::gray_72 = colour_t(184, 184, 184);
const colour_t colour_t::gray_71 = colour_t(181, 181, 181);
const colour_t colour_t::gray_70 = colour_t(179, 179, 179);
const colour_t colour_t::gray_69 = colour_t(176, 176, 176);
const colour_t colour_t::gray_68 = colour_t(173, 173, 173);
const colour_t colour_t::gray_67 = colour_t(171, 171, 171);
const colour_t colour_t::gray_66 = colour_t(168, 168, 168);
const colour_t colour_t::gray_65 = colour_t(166, 166, 166);
const colour_t colour_t::gray_64 = colour_t(163, 163, 163);
const colour_t colour_t::gray_63 = colour_t(161, 161, 161);
const colour_t colour_t::gray_62 = colour_t(158, 158, 158);
const colour_t colour_t::gray_61 = colour_t(156, 156, 156);
const colour_t colour_t::gray_60 = colour_t(153, 153, 153);
const colour_t colour_t::gray_59 = colour_t(150, 150, 150);
const colour_t colour_t::gray_58 = colour_t(148, 148, 148);
const colour_t colour_t::gray_57 = colour_t(145, 145, 145);
const colour_t colour_t::gray_56 = colour_t(143, 143, 143);
const colour_t colour_t::gray_55 = colour_t(140, 140, 140);
const colour_t colour_t::gray_54 = colour_t(138, 138, 138);
const colour_t colour_t::gray_53 = colour_t(135, 135, 135);
const colour_t colour_t::gray_52 = colour_t(133, 133, 133);
const colour_t colour_t::gray_51 = colour_t(130, 130, 130);
const colour_t colour_t::gray_50 = colour_t(127, 127, 127);
const colour_t colour_t::gray_49 = colour_t(125, 125, 125);
const colour_t colour_t::gray_48 = colour_t(122, 122, 122);
const colour_t colour_t::gray_47 = colour_t(120, 120, 120);
const colour_t colour_t::gray_46 = colour_t(117, 117, 117);
const colour_t colour_t::gray_45 = colour_t(115, 115, 115);
const colour_t colour_t::gray_44 = colour_t(112, 112, 112);
const colour_t colour_t::gray_43 = colour_t(110, 110, 110);
const colour_t colour_t::gray_42 = colour_t(107, 107, 107);
const colour_t colour_t::dimgray__gray_42_ = colour_t(105, 105, 105);
const colour_t colour_t::gray_40 = colour_t(102, 102, 102);
const colour_t colour_t::gray_39 = colour_t(99, 99, 99);
const colour_t colour_t::gray_38 = colour_t(97, 97, 97);
const colour_t colour_t::gray_37 = colour_t(94, 94, 94);
const colour_t colour_t::gray_36 = colour_t(92, 92, 92);
const colour_t colour_t::gray_35 = colour_t(89, 89, 89);
const colour_t colour_t::gray_34 = colour_t(87, 87, 87);
const colour_t colour_t::gray_33 = colour_t(84, 84, 84);
const colour_t colour_t::gray_32 = colour_t(82, 82, 82);
const colour_t colour_t::gray_31 = colour_t(79, 79, 79);
const colour_t colour_t::gray_30 = colour_t(77, 77, 77);
const colour_t colour_t::gray_29 = colour_t(74, 74, 74);
const colour_t colour_t::gray_28 = colour_t(71, 71, 71);
const colour_t colour_t::gray_27 = colour_t(69, 69, 69);
const colour_t colour_t::gray_26 = colour_t(66, 66, 66);
const colour_t colour_t::gray_25 = colour_t(64, 64, 64);
const colour_t colour_t::gray_24 = colour_t(61, 61, 61);
const colour_t colour_t::gray_23 = colour_t(59, 59, 59);
const colour_t colour_t::gray_22 = colour_t(56, 56, 56);
const colour_t colour_t::gray_21 = colour_t(54, 54, 54);
const colour_t colour_t::gray_20 = colour_t(51, 51, 51);
const colour_t colour_t::gray_19 = colour_t(48, 48, 48);
const colour_t colour_t::gray_18 = colour_t(46, 46, 46);
const colour_t colour_t::gray_17 = colour_t(43, 43, 43);
const colour_t colour_t::gray_16 = colour_t(41, 41, 41);
const colour_t colour_t::gray_15 = colour_t(38, 38, 38);
const colour_t colour_t::gray_14 = colour_t(36, 36, 36);
const colour_t colour_t::gray_13 = colour_t(33, 33, 33);
const colour_t colour_t::gray_12 = colour_t(31, 31, 31);
const colour_t colour_t::gray_11 = colour_t(28, 28, 28);
const colour_t colour_t::gray_10 = colour_t(26, 26, 26);
const colour_t colour_t::gray_9 = colour_t(23, 23, 23);
const colour_t colour_t::gray_8 = colour_t(20, 20, 20);
const colour_t colour_t::gray_7 = colour_t(18, 18, 18);
const colour_t colour_t::gray_6 = colour_t(15, 15, 15);
const colour_t colour_t::gray_5 = colour_t(13, 13, 13);
const colour_t colour_t::gray_4 = colour_t(10, 10, 10);
const colour_t colour_t::gray_3 = colour_t(8, 8, 8);
const colour_t colour_t::gray_2 = colour_t(5, 5, 5);
const colour_t colour_t::gray_1 = colour_t(3, 3, 3);

} /* namespace cairo */
} /* namespace ui */
