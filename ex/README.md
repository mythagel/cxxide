
vi basics (commands operating on buffer)

cmake json compilation db generation

clang lexed tokens
clang ast visitor

clang LibFormat for code formatter.


X11
===============
http://xdvi.cvs.sourceforge.net/viewvc/xdvi/xdvik/texk/xdvik/xdvi.c?view=markup
Good examples of icons, document display, fullscreen, set window size

 * set_icon_and_title
 * set_windowsize
 * net_wm_toggle_fullscreen
 * reconfigure_window
 * 

libgit basics

requirements
================

git provides
 - clone
 - init
 - branch name
 - push code
 - pull code
 - create branch
 - switch to branch
 - change status
 - commit code
 - merge
 - ALL commands are common case. Advanced commands executed manually.
 - Requirement is that gui be responsive to manual changes, i.e. perfectly valid to commit code from cli while gui is open. Update must be reflected in reasonable time.


cmake provides
 - 2 variants
    - managed
       - Subset of cmake language
       - this is to cmake as DSC is to Postscript
          - except without interpreted comments if at all possible
       - command case configuration
       - Automatically managed
       - new source file updates source target list for selected target.
       - new source folder creates and references new list file.
    - unmanaged
       - Generic CMakeLists.txt usage
       - No GUI control
       - No automatic editing
       - After adding source files - must update manually
       - Adding new source folders must update manually
       - Anything - must update manually
 - list of compile commands, list of sources + target mapping
 - ability to regenerate above
 - ability to build project (--build)
 - build progress
 - files to be rebuilt (or is this make?)
 

take compile commands and pass to clang
clang provides
 - lexed tokens and types - for syntax highlighting
 - list of defined symbols
 - semantic tokens - for syntax highlighting
 - class / function boundaries
    - source references for all types related to a particular class
 - code completion
 - source location to AST object (for type lookup)
 - diagnostics & semantic analysis (fixups etc.)
 - AST type tree for browser.
 - Tokens to be searched
 - (to be developed) semantic function annotation (pure, related to class, variables / globals accessed / modified)
 
