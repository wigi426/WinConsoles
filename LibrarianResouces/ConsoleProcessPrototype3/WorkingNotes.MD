# Working Notes, Console Prototype

-   parent.exe only needs to replicate the future behvaiour of the the WinConsole library and does not need to be written well.
-   console.cpp cannot rely on any non standard header files(not accessible by defualt by most compilers)
-   why can't the write to console thread in console.cpp just be in a constant loop of calling get on the inStream, check a boolean for exit before each read, and check the read for eof
-   main.cpp -o stdcstreamTest.exe is to test the behviour of the std streams and check that i am able to replicate identically
-   things the parent has to do:
    -   write the correct command for the desired replicant behaviour to the cmd stream
    -   flush that stream
    -   call the istream method after informing the console that reading is required
-   when trying to replicate behaviour of std::cin reaching eof simplest solution is just to close the console process, if std::cin reaches eof the console is unusable anyway

### 28/11/2022 end notes:

-   write has been simplified greatly, hopefully it can stay so simple
-   read will be more complicated due to the requirement to replicate the behaviour of std::cin:
    -   i will effectively need to pass along which method has been called and what it's parameters were
    -   this will require a more complicated command parsing system
    -   recommend start with once function such as get, and fulfil replicating the behaviour of all overloads

### 29/11/2022 end note:

-   console process is now able to allow for replicate behaviour of all 6 overloads of std::istream.get(), still needs to be fully tested but much closer to having an idea
-   added main.cpp -o stdcstreamTest.exe as to test behviour of the std console streams and check mine is an identical replicate.

### 04/12/2022 end notes:

-   have decided on a change of approach, every std::istream unformatted input function can be replicated with proper implementation on the parent side with two pieces of information:
    -   the delimiter character
    -   the number of characters to read
-   if the console process cin stream reaches eof then the console should close, while this isn't a perfect replication, it is pointless to keep
    a console proces open when it's read buffer is now unusable.

### 09/12/2022 end notes:

-   implemented "ru;count;delim;" command in console which should allow the parent to properly implement all unformatted input functions
-   next need to implement "rf" command to allow parent to replicate formmatted input functions
-   implementing the formmated input function replication behaviour should simply involved requiring the parent to inform that a fomatten input funciton has been called and then fetching all user input until the first whitespace after any preceeding whitespace
