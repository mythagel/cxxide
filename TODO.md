TODO / Task List
================

Git project init / open complete
 * Other tasks can be implemented later as needed

Cmake project init / open complete
 * Can create an manage a logical project structure.
 * Need to revisit serialisation of targets - should be one target per section so they can be made conditional.

Need to implement source manager
 * Creates source file(s) from templates

Sould base project object have concept of license (i.e. project license)
 * could then automate license headers in files and include license text
 * Need license list
 * ~/.cxxide/licences/MIT/LICENSE - license text
 * ~/.cxxide/licences/MIT/header - header text
 * header text is a short text summary of the license
 * the source manager will enclose the text in comments and inject at top of file.

Cmake style variables, example from eclipse

    ${project_name} ${project_summary}
    ${license}
    
    /*
     * ${filename}
     *
     *  Created on: ${datetime}
     *      Author: ${user}
     */
    
    #ifndef ${include_guard}
    #define ${include_guard}
    
    #endif

Clang tasks do not exist
 * libclang all the way.

cxxide functions and what is required from clang
================================================

 * Semantic highlighting
    * Need to categorise every token with how to colour/style it.
    * properties to be set: colour, bold, italic, underline
    * Not considered in this module is diagnostics
    * same text annotation subsystem but different annotations
    * Edit loop highlighting to re-lex current text edit range and do syntax highlighting only.
 * Code completion
    * recommendation is to get all completions at last logical point (i.e. var->blah requests clang completion at var->.
    * Then client filters for currently entered text.
    * This reduces latency given code completion results take noticible time.
    * Need to be requested async and then filtered as user continues typing.
    * Clang interface to code completion is incredibly simple given how complicated the actual function is.
