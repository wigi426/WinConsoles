

add_library(WinConsolesLib STATIC
    WinConsoles.h
    WinConsoles.cpp
    Console.h
    Console.cpp
    Cistream.h
    Cistream.cpp
    Costream.h
    Costream.cpp
    Win32Helpers.h
    Win32Helpers.cpp
)

if(MSVC)
    target_compile_options(WinConsolesLib PRIVATE /W4 /WX)
else()
    target_compile_options(WinConsolesLib PRIVATE -Wall -Wpedantic -Wextra -Werror)
endif()

# copy WinConsoles.h to build/include/
configure_file(WinConsoles.h ${CMAKE_BINARY_DIR}/include/WinConsoles.h COPYONLY)