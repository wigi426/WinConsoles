# Outlining of goals for this library

## **Primary:** to provide a Simple way to open multiple console windows and write and read from them.

### the interface for this library will allow a user to pass in a std::string or a char array. formatting for input is the users responsibility.

### the library will use std::istream to format console input. When the user asks for input they can use a get function which allows them to specifiy both a count of characters to receive and a deliminator.
