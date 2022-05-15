#---------------------------------------------------------------------------
PROJECT_NAME           = "p2c"
#---------------------------------------------------------------------------
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
#---------------------------------------------------------------------------
INPUT                  = @CMAKE_CURRENT_SOURCE_DIR@/../include \
                         @CMAKE_CURRENT_SOURCE_DIR@/../src \
                         @CMAKE_CURRENT_SOURCE_DIR@/../src/parser
#---------------------------------------------------------------------------
SOURCE_BROWSER         = YES
INLINE_SOURCES         = YES
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
#---------------------------------------------------------------------------
GENERATE_LATEX         = NO
GENERATE_RTF           = NO
GENERATE_MAN           = NO
GENERATE_XML           = NO
GENERATE_DOCBOOK       = NO
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = NO
#---------------------------------------------------------------------------
HAVE_DOT               = YES
CLASS_DIAGRAMS         = YES
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = NO
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
CALLER_GRAPH           = YES
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
# DoLEANUP            = YES
