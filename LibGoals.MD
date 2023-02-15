# Outlining of goals for this library

-   ## **Primary:** to provide a Simple way to open multiple console windows and write and read from them.

-   ## **Secondary:** Provide a std::stream like interface and replicant behaviour as to make the usage of the library created consoles similar to use the of std::cout and std::cin
    -   ### privately contain a std::stream to pass the character values through, in order to replicate behaviour
    -   ### use effectively the same interface without inheriting
