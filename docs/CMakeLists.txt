cmake_minimum_required (VERSION 3.15)

# dot (graphviz) is not required but strongly preferred.
find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_CURRENT_BINARY_DIR}/Ookii.CommandLine.Cpp.tag")
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_STRIP_FROM_INC_PATH "../include/")
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_SORT_BRIEF_DOCS YES)
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
    set(DOXYGEN_EXCLUDE_SYMBOLS "ookii::details;ookii::vt::details;OOKII_CONSOLE_FUNC;OOKII_CONSOLE_FUNC_HAS_BODY")
    set(DOXYGEN_PROJECT_NAME "Ookii.CommandLine for C++")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "doxygen_main.md")
    set(DOXYGEN_PREDEFINED "DOXYGEN=1")
    doxygen_add_docs(docs "../include/ookii" "doxygen_main.md")
endif()