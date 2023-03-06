# Working Notes, Console Prototype

-   parent.exe only needs to replicate the future behvaiour of the the WinConsole library and does not need to be written well.
-   console.cpp cannot rely on any non standard header files(not accessible by defualt by most compilers)
-   why can't the write to console thread in console.cpp just be in a constant loop of calling get on the inStream, check a boolean for exit before each read, and check the read for eof
-   main.cpp -o stdcstreamTest.exe is to test the behviour of the std streams and check that i am able to replicate identically
-   things the parent has to do:
    -   write the correct command for the desired replicant behaviour to the cmd stream
    -   flush that stream
    -   call the istream method after informing the console that reading is required
    -   extra steps to make sure behviour is replicant
-   when trying to replicate behaviour of std::cin reaching eof simplest solution is just to close the console process, if std::cin reaches eof the console is unusable anyway
-   need to work out if there are any cases where after a read command in console process: `std::cin.rdstate() != std::cin.goodbit` that we shoudn't close the console process?
-   referencing above note: there definitely are, failbit is set if you don't actually extract anything, but a delimiter is reached on get funcitons which don't extract delimiter. but eofbit and badbit are valid reasons to close seemingly

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

### 10/12/2022 end notes

-   implemented rf command
-   now working on getting ignore to replicate correctly. i think i need to add an extra command to the read command which tells the console whether to extract the delimiter, since getline functions this way and ignore effectively functions this way.

### 12/12/2022 end notes

-   ignore is now replicable properly
    -   did this by adding an extra parameter for read commands sent down the cmd pipe by the parent
    -   the new parameter is a `bool extract` it indicates whether the console should extract the delimiter character from the std::cin stream
-   refactored console to clean up code
-   there is no longer a need for an rf command, that has been removed, rf is seemingly fully replicable with the functionality already provided by the ru command

### PreRefactor Analysis #1 (blockquote if refactor implemented)

-   > need one type of error handling in the console, ideally the same type of error handling used for the entire:
    -   > perhaps throw but catch in the console and have a system to report to the host process, redirrect std::err?
-   > encapuslate reading and writing or the ReadThreadCmdQueue and all the locking/condition variable management that comes along with that so that, currently we are repeating code to do so.
-   > move exit operations to after the loop, they need to be done each time and they're not dependent on the loop scope

### things to test:

-   what happens when the parent uses /0 as a delim, does it mess up the read command messaging:
    -   yes, fixed, parent uses //0 and we parse that out before calling any extraction function

### Prototype 4

-   we are no longer replicating std::iostreams and therefore will be using win32 annonymous pipe funcitons to receive and send data to and from the parent process
-   we need to add command line args that the console process takes in which allow the console of the console process to be positioned at specified coordinates and sized by specified parameters
-   since so much of the current code is intertwined with the usage of std::streams it would probably be best to write a new console.cpp while following the general flow and trend of the code, rather than trying to go line by line replacing lines with what makes sense for using win32 readFile and writeFile funcitons
