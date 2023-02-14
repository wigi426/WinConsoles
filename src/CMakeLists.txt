

add_library(WinConsolesLib STATIC
    WinConsoles.h
    WinConsoles.cpp
)

if(MSVC)
    target_compile_options(WinConsolesLib PUBLIC /W4 /WX)
else()
    target_compile_options(WinConsolesLib PUBLIC /Wall /Wpedantic /Wextra /Werror)
endif()

# copy WinConsoles.h to build/include/
configure_file(WinConsoles.h ${CMAKE_BINARY_DIR}/include/WinConsoles.h COPYONLY)