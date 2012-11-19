# - Try to find LibTooling
# Once done, this will define
#
#  Tooling_FOUND - system has Tooling
#  Tooling_INCLUDE_DIRS - the Tooling include directories
#  Tooling_LIBRARIES - link these to use Tooling
#  Tooling_DEFINES - definitions

include(LibFindMacros)

# Include dir
find_path(Tooling_INCLUDE_DIR
  NAMES clang/Tooling/Tooling.h
)

# Finally the library itself
find_library(Tooling_LIBRARY
  NAMES clang
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Tooling_PROCESS_INCLUDES Tooling_INCLUDE_DIR Magick_INCLUDE_DIRS)
set(Tooling_PROCESS_LIBS Tooling_LIBRARY Magick_LIBRARIES)
set(Tooling_DEFINES -D__STDC_LIMIT_MACROS=1 -D__STDC_CONSTANT_MACROS=1 -fno-rtti -DGTEST_HAS_RTTI=0 -DGTEST_HAS_TR1_TUPLE=0)
libfind_process(Tooling)
