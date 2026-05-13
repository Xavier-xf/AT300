add_compile_options(-w)

add_definitions(
    -DHAVE_ASSERT_H
)

if(PLATFORM_TYPE STREQUAL  "x86_64")
add_definitions(
	-DFPM_64BIT
)
else()
add_definitions(
	-DFPM_ARM
)
endif()

include_directories(
    .
)
    
file(GLOB_RECURSE SOURCES ./*.c )

add_library(mad STATIC ${SOURCES})