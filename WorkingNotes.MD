# WinConsoles Working notes

-   Define interface fully first
-   use one form of error handling, probably exceptions in places where they are absolutely needed
-   otherwise best to have a publicly accessible validity flag. which is set when the user attempts to use the interface incorreclty
-   ideally however the interface should be hard to use wrong.
-   using pImpl idiom to hide implementation details and have a clean interface file (WinConsoles.h)
-   do we need to use WinConsoles namespace in every implementation file
-   I need to clearly define the goals of this library otherwise i will constantly be changing my mind about how to design especially the interface. Is it supposed to replicate a std::stream to the most accurate degree, or are there other values which need to be considered for at a higher priority
-   the console process is in charge of positioning its console window dependent on arguments passed to it on execution

## thinkthrough of idea

### idea: perhaps use a privately contained std::ostream to simulate behaviour, feeding all input through that stream first then sending it to the console program

-   this might mean i could templatise all ostream replicating functions and then just call on the actual ostream
-   so contain a std::ostream use it as a passthrough so that we can replicate things like std::endl
-   this way the cstreams can fully replicate the std::streams in interface

## dependencies:

-   must be built with a c++20 compatible compiler
-   when included, c++ 11 required

### notes for when return to work as taking christmas off:

-   Still organising frontend interface, still not used to pImpl idiom usage, and implementing it with the inheritance adds complexities
-   If you have a uniuqe ptr as pImpl in the base and derived classes does that work? they would point to the same object, even if interpreted as a different object, pretty sure that would violate unique_ptr requirements
-   https://stackoverflow.com/questions/491304/pimpl-idiom-with-inheritance <- stack overflow on pImpl idiom with inheritance

### a realigning of goals

-   i have dropped the goal to replicate or provide a inherited std::iostream interface
-   instead i will provide very simple write() and read() functions at least to start, this also means that the interface no longer requires access to stream object which represent the unerlying streams
-   instread these read and write operations will be accessed directly through the console interface.
-   this heavilty simplifies the interface, making it one class "Console" which uses the pImpl idiom to hide implementation details.

### 16/03/2023 notes

-   src/Console.cpp needs the location which console.exe gets compiled to, right now this is achieved by having the top CMakeLists.txt write the binary output location of the console to a file called ConsoleLoc.h which is located in the src directory, Console.cpp includes this file and reads the location variable that was written to it by cmake.
-   WaitForInputIdle() only works on GUI applications, instead we can wait for confirmation that the process is ready by having the console send a character up the read pipe and having the parent wait for it to be written to.
-   GetHandleInformation() may give the proper santization in the destructor of Hndl
