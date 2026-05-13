add_compile_options(-w)

# add_definitions(
#     -DHAVE_ASSERT_H
# )

include_directories(
    .
)

file(GLOB_RECURSE SOURCES 
    thpool.c
)

add_library(thpool STATIC ${SOURCES})