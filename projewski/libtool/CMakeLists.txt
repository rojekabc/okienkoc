cmake_minimum_required(VERSION 3.17)
project(projewski-tool C)

set(CMAKE_C_STANDARD 11)

add_library(projewski-tool SHARED
        src/arguments.c src/arguments.h
        src/array.c src/array.h
        src/bits.c
        src/conflog.h
#        src/easymap.c src/easymap.h
        src/fileistream.c src/fileistream.h
        src/fileostream.c src/fileostream.h
        src/font.c
        src/global-inc.h
        src/istream.c src/istream.h
        src/iterator.c src/iterator.h
        src/library.c src/library.h
        src/log.h
        src/malloc.h
        src/memistream.c src/memistream.h
        src/memostream.c src/memostream.h
        src/mempipestream.c src/mempipestream.h
        src/mystr.c src/mystr.h
        src/nbitpola.c src/nbitpola.h
        src/ostream.c src/ostream.h
        src/random.h
        src/screen.c src/screen.h
        src/tableiterator.c src/tableiterator.h
        src/tablica.c src/tablica.h
        src/term.c src/term.h
        src/typetable.h
#        src/vtree.c src/vtree.h
        )

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(testsuite
        test/testsuite.c
        test/stringsuite.c test/stringsuite.h)
target_link_libraries(testsuite projewski-tool check)

add_executable(test001 test/test001.c)
target_link_libraries(test001 projewski-tool)

add_executable(test002 test/test002.c)
target_link_libraries(test002 projewski-tool)

add_executable(test003 test/test003.c)
target_link_libraries(test003 projewski-tool)

