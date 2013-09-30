#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xft/Xft.h>
#include <string>

#define DEFAULT_BDWIDTH 1 /* border width */

Display *dpy;

int main( int argc, char *argv[] )
{
   int                     ytext, wtext, htext, gravity, geo_mask;
   int                     Done = 0, highlight = 0, force_expose = 0;
   XSizeHints              xsh;
   XWMHints                xwmh;
   unsigned long           fgpix, bgpix;
   Window                  mainW;
   XSetWindowAttributes    xswa;
   XftFont                 *font;
   XftDraw                 *xftdraw;
   XRenderColor            xrcolor;
   XftColor                xftcolor, hlcolor;
   XGlyphInfo              extents;
   XEvent                  event;
   char                    user_geo[80];
 
   std::string text;
   text = "int main()\n\
{\n\
    int main_local;\n\
    \n\
    test t;\n\
    t.member = 5;\n\
    for(int i = 0; i < 10; ++i)\n\
        main_local += t.mem_func() + i;\n\
    \n\
    return main_local;\n\
}";
   
   /* open connection to X display */
   dpy = XOpenDisplay( NULL );
   
   /* set up font */
   font = XftFontOpenName( dpy, DefaultScreen( dpy ), "Source Code Pro-9" ); 
   
   /* colors */
   fgpix = BlackPixel( dpy, DefaultScreen( dpy ) );
   bgpix = WhitePixel( dpy, DefaultScreen( dpy ) );
   
   /* position and size of top window (XSizeHints) */
   XftTextExtents8( dpy, font, (const FcChar8*)text.c_str(), text.size(), &extents );
   ytext = extents.height - extents.y;
   wtext = extents.width - extents.x;
   htext = ytext + 4;   
   xsh.flags = ( PPosition | PSize | PMinSize );
   xsh.height = htext + 10;
   xsh.min_height = xsh.height;
   xsh.width = wtext;
   xsh.min_width = xsh.width;
   xsh.x = 50;
   xsh.y = 50;
   
   /* construct a geometry string */
   sprintf( user_geo, "%dx%d+%d+%d", xsh.width, xsh.height, xsh.x, xsh.y );
   
   /* process geometry specification */
   geo_mask = XWMGeometry( dpy, DefaultScreen(dpy), user_geo, NULL/*def_geo*/,
                  DEFAULT_BDWIDTH, &xsh, &xsh.x, &xsh.y, &xsh.width, &xsh.height,
                  &gravity );
   
   /* check geometry bitmask and set size hints */
   if ( geo_mask & (XValue|YValue) ) xsh.flags |= USPosition;
   if ( geo_mask & (WidthValue|HeightValue) ) xsh.flags |= USSize;
   
   /* create top level window */
   mainW = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), xsh.x, xsh.y, 
               xsh.width, xsh.height, DEFAULT_BDWIDTH, fgpix, bgpix );
   
   /* set window manager properties */
   XSetStandardProperties( dpy, mainW, "xhello", "xhello", None, argv, argc, &xsh );
   
   /* set window manager hints */
   xwmh.flags = (InputHint|StateHint);
   xwmh.input = False;
   xwmh.initial_state = NormalState;
   XSetWMHints( dpy, mainW, &xwmh );

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
   printf("click on the window to exit\n");
   
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
                  int            x, y, itmp;
                  unsigned int   w, h, utmp;
                  Window         wtmp;

                  if ( XGetGeometry( dpy, mainW, &wtmp, &itmp, &itmp, &w, &h, &utmp, &utmp )==0 )
                     break;
                  XClearWindow( dpy, mainW );
                  x = ( w - extents.width ) / 2;
                  y = htext + ( h - htext + extents.height )/2;
                  if (highlight) XftDrawRect( xftdraw, &hlcolor, x, y-extents.height, wtext, extents.height  );
                  XftDrawString8( xftdraw, &xftcolor, font, x, y, (const FcChar8*)text.c_str(), text.size() );
                  force_expose = 0;
               }
               break;
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
