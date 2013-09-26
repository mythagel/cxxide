#include <cstdio>

#include <X11/Intrinsic.h>

#include <X11/Xaw/Cardinals.h>
// Widget headers
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Tree.h>

// Resources
String fallback_resources[] = {
    "*label1.label:            One",
    "*label2.label:            Two",
    "*label3.label:            Three",
    "*label4.label:            Four",
    "*paned.orientation:       horizontal",
    "*tree.gravity:            west",
    NULL
};

int main(int argc, char* argv[])
{
    XtAppContext app_con;
    Widget toplevel;
    Widget outer;
    Widget tree;

    toplevel = XtAppInitialize(&app_con, "frame", nullptr, ZERO, &argc, argv, fallback_resources, nullptr, ZERO);

    outer = XtCreateManagedWidget("paned", panedWidgetClass, toplevel, nullptr, ZERO);

    
    tree = XtVaCreateManagedWidget("tree", treeWidgetClass, outer, nullptr);
    Widget one = XtVaCreateManagedWidget("label1", labelWidgetClass, tree, nullptr);
    Widget three = XtVaCreateManagedWidget("label3", labelWidgetClass, tree, nullptr);
    Widget four = XtVaCreateManagedWidget("label4", labelWidgetClass, tree, XtNtreeParent, three, nullptr);
    
    
    XtCreateManagedWidget("label2", labelWidgetClass, outer, nullptr, ZERO);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_con);
    
    return 0;
}

