#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xft/Xft.h>
#include <string>
#include <vector>

#define DEFAULT_BDWIDTH 1 /* border width */

int main( int argc, char *argv[] )
{
   int                     ytext, wtext, htext, gravity, geo_mask;
   int                     Done = 0, highlight = 0, force_expose = 0;
   Window                  mainW;
   XSetWindowAttributes    xswa;
   XftDraw                 *xftdraw;
   XRenderColor            xrcolor;
   XftColor                xftcolor, hlcolor;
   XEvent                  event;
   char                    user_geo[80];
 
    std::vector<std::string> text;
    text.push_back("int main()");
    text.push_back("{");
    text.push_back("    int main_local;");
    text.push_back("    ");
    text.push_back("    test t;");
    text.push_back("    t.member = 5;");
    text.push_back("    for(int i = 0; i < 10; ++i)");
    text.push_back("        main_local += t.mem_func() + i;");
    text.push_back("    ");
    text.push_back("    return main_local;");
    text.push_back("}");
   
   /* open connection to X display */
   auto dpy = XOpenDisplay( NULL );
   
   /* set up font */
   auto font = XftFontOpenName( dpy, DefaultScreen( dpy ), "Source Code Pro-12" ); 
   
   /* colors */
   auto fgpix = BlackPixel( dpy, DefaultScreen( dpy ) );
   auto bgpix = WhitePixel( dpy, DefaultScreen( dpy ) );
   
   /* create top level window */
   mainW = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
               800, 600, DEFAULT_BDWIDTH, fgpix, bgpix );

    Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False); 
    XSetWMProtocols(dpy, mainW, &WM_DELETE_WINDOW, 1);
   
   /* Xft draw context */
   xftdraw = XftDrawCreate( dpy, mainW, DefaultVisual(dpy,DefaultScreen(dpy)),
               DefaultColormap( dpy, DefaultScreen(dpy) ) );
   /* Xft text color */
   xrcolor.red = 0x0;
   xrcolor.green = 0x0;
   xrcolor.blue = 0x0;
   xrcolor.alpha = 0xffff;
   XftColorAllocValue( dpy, DefaultVisual(dpy,DefaultScreen(dpy)),
               DefaultColormap( dpy, DefaultScreen(dpy) ), &xrcolor, &xftcolor );
   /* Xft highlight color */
   xrcolor.red = 0xafff;
   xrcolor.green = 0xafff;
   xrcolor.blue = 0xffff;
   xrcolor.alpha = 0xffff;
   XftColorAllocValue( dpy, DefaultVisual(dpy,DefaultScreen(dpy)),
               DefaultColormap( dpy, DefaultScreen(dpy) ), &xrcolor, &hlcolor );
   
   /* set window attributes */
   xswa.colormap = DefaultColormap( dpy, DefaultScreen(dpy) );
   xswa.bit_gravity = CenterGravity;
   XChangeWindowAttributes( dpy, mainW, (CWColormap|CWBitGravity), &xswa );
   
   /* select inputs */
   XSelectInput( dpy, mainW, ExposureMask|ButtonPressMask|EnterWindowMask|LeaveWindowMask );
   
   /* make window visible */
   XMapWindow( dpy, mainW );
   
   /* retrieve and process events */
   while ( !Done ) {
      XNextEvent( dpy, &event );
      if ( event.xany.window == mainW ) {
         switch ( event.type ) {
            case EnterNotify:
            case LeaveNotify:
               if ( event.type==EnterNotify) highlight = 1;
               else highlight = 0;
               force_expose = 1;
            case Expose: 
               if ( event.xexpose.count == 0 || force_expose ) {
                  int             itmp;
                  unsigned int   w, h, utmp;
                  Window         wtmp;

                  if ( XGetGeometry( dpy, mainW, &wtmp, &itmp, &itmp, &w, &h, &utmp, &utmp )==0 )
                     break;
                  XClearWindow( dpy, mainW );
                  
                  int y = 20;
                  for(auto& line : text)
                  {
                      XGlyphInfo extents;
                      XftTextExtentsUtf8 (dpy, font, (const FcChar8*)line.c_str(), line.size(), &extents);
                      XftDrawStringUtf8( xftdraw, &xftcolor, font, 10, y, (const FcChar8*)line.c_str(), line.size() );
                      y = y + 4 + extents.height;
                  }
                  force_expose = 0;
               }
               break;
            case ClientMessage:
            case ButtonPress:
               Done = 1;
               printf("good-bye!\n");
               break;
            default:
               break;
         }
      }
   }

   /* close connection to display */
   XftDrawDestroy( xftdraw );
   XftColorFree( dpy, DefaultVisual(dpy,DefaultScreen(dpy)),
               DefaultColormap( dpy, DefaultScreen(dpy) ), &xftcolor );
   XftColorFree( dpy, DefaultVisual(dpy,DefaultScreen(dpy)),
               DefaultColormap( dpy, DefaultScreen(dpy) ), &hlcolor );
   XDestroyWindow( dpy, mainW );
   XCloseDisplay( dpy );
              
   return(0);
}
