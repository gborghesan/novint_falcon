
#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#

find_package(NiFalconAPI REQUIRED)

message("Found Falcon lib: ${NIFALCONAPI_FOUND}")
include_directories(${NIFALCONAPI_INCLUDE_DIR})

orocos_component(novint_falcon novint_falcon-component.hpp novint_falcon-component.cpp) #
target_link_libraries(novint_falcon ${NIFALCONAPI_LIBRARIES})

orocos_component(controller controller-component.hpp controller-component.cpp) #


orocos_install_headers(novint_falcon-component.hpp controller-component.hpp) # ...you may add multiple header files
#
# You may add multiple orocos_install_headers statements.

#
