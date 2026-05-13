add_compile_options(-w)

# add_definitions(
#     -DHAVE_ASSERT_H
# )

include_directories(
    .
    libonvif/include
    ../libxml2-2.15.1/include
)

file(GLOB_RECURSE SOURCES 
    libonvif/src/cencode.c
    libonvif/src/onvif.c
    libonvif/src/sha1.c
)

add_library(onvif STATIC ${SOURCES})