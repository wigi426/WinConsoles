

add_library(WinConsoles STATIC
    WinConsoles.h
    WinConsoles.cpp
    WinConsolesImpl.h
    WinConsolesImpl.cpp
)

if(MSVC)
    target_compile_options(WinConsoles PUBLIC /W4 /WX)
else()
    target_compile_options(WinConsoles PUBLIC /Wall /Wpedantic /Wextra /Werror)
endif()