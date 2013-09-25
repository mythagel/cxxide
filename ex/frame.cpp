#include <cstdio>

#include <X11/Intrinsic.h>

#include <X11/Xaw/Cardinals.h>
// Widget headers
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>

// Resources
String fallback_resources[] = {
    "*label1.label:            One",
    "*label2.label:            Two",
    "*paned.orientation:       horizontal",
    NULL
};

int main(int argc, char* argv[])
{
    XtAppContext app_con;
    Widget toplevel;
    Widget outer;

    toplevel = XtAppInitialize(&app_con, "frame", nullptr, ZERO, &argc, argv, fallback_resources, nullptr, ZERO);

    outer = XtCreateManagedWidget("paned", panedWidgetClass, toplevel, nullptr, ZERO);

    XtCreateManagedWidget("label1", labelWidgetClass, outer, nullptr, ZERO);
    XtCreateManagedWidget("label2", labelWidgetClass, outer, nullptr, ZERO);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_con);
    
    return 0;
}

