# Working Notes, Console Prototype

-   parent.exe only needs to replicate the future behvaiour of the the WinConsole library and does not need to be written well.
-   console.cpp cannot rely on any non standard header files(not accessible by defualt by most compilers)
-   why can't the write to console thread in console.cpp just be in a constant loop of calling get on the inStream, check a boolean for exit before each read, and check the read for eof

### 28/11/2022 end note:

-   write has been simplified greatly, hopefully it can stay so simple
-   read will be more complicated due to the requirement to replicate the behaviour of std::cin:
    -   i will effectively need to pass along which method has been called and what it's parameters were
    -   this will require a more complicated command parsing system
    -   recommend start with once function such as get, and fulfil replicating the behaviour of all overloads
