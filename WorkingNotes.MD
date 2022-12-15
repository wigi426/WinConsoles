# WinConsoles Working notes

-   Define interface fully first
-   use one form of error handling, probably exceptions in places where they are absolutely needed
-   otherwise best to have a publicly accessibly validity flag. which is set when the user attempts to use the interface incorreclty
-   ideally however the interface should be hard to use wrong.
-   using pImpl idiom to hide implementation details and have a clean interface file (WinConsoles.h)
-   do we need to use WinConsoles namespace in every implementation file

## dependencies:

-   must be built with a c++20 compatible compiler
-   when included, c++ 11 required
