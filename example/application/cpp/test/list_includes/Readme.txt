A Standard compliant C preprocessor
Sample: List include dependencies of a given source file

Copyright (c) 2001-2003 Hartmut Kaiser
http://spirit.sourceforge.net/

See Copyright.txt for full copyright notices and acknowledgements.

-------------------------------------------------------------------------------

The 'list_includes' sample shows a simple way, how to use the C preprocessor
iterator to extract a list of included files from a given source file. To 
get a hint which commandline options are supported, call it with the --help
option.

Note: This sample requires the program_options library written by Vladimir Prus,
which is currently under Boost review. 
It is available here: http://zigzag.cs.msu.su:7813/program_options.

The build process is straightforward. The only thing you should do 
is to include the root directory of the cpp sample into the include search
path of your compiler (usually through a -I../.. command line argument).
