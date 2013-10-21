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
