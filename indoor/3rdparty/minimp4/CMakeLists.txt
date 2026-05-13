add_compile_options(-w)

# add_definitions(
#     -DHAVE_ASSERT_H
# )

include_directories(
    .
)

file(GLOB_RECURSE SOURCES ./*.c )

add_library(minimp4 STATIC ${SOURCES})