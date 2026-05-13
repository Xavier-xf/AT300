add_compile_options(-w)

# add_definitions(
#     -DHAVE_ASSERT_H
# )

include_directories(
    .
    ./include/
)

file(GLOB_RECURSE SOURCES 
    threads.c
    tree.c
    xmlstring.c
    globals.c
    dict.c
    error.c
    parserInternals.c
    parser.c
    entities.c
    hash.c
    xmlIO.c
    xmlsave.c
    buf.c
    HTMLtree.c
    encoding.c
    HTMLparser.c
    SAX2.c
    valid.c
    uri.c
    list.c
    xmlregexp.c
    chvalid.c
    catalog.c
    xpath.c
    debugXML.c
    xmlmemory.c
    relaxng.c
    xmlschemastypes.c
    xmlschemas.c
    pattern.c
    xmlreader.c
    xinclude.c
    xpointer.c
)

add_library(xml2 STATIC ${SOURCES})