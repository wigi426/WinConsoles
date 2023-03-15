# Console Interface

## Console Creation

---

### Pipes

#### The Console process requires 3 Win32 Annonymous pipes in order to funciton:

1.  **Command Pipe**: \
    A pipe used to tell the console when to write to the conosle and when to read from the console. Also used to tell the console to close itself.

2.  **Console Write Pipe**:\
    A pipe used to send string data to be written to the Console.

3.  **Console Read Pipe**:\
    A pipe used to receive data which has been read from the Console.

These pipes should be created using Win32 funciton CreatePipe(), which creates two handles one which references the read end of the pipe and one which references the write end of the pipe. The console process should be provided with one Win32 HANDLE to each pipe, the read end for the **Command Pipe** and the **Console Write Pipe** and the write end for the **Console Read Pipe**

The ends of the pipe meant for the console are inherited by passing the HANDLE values reinterpret casted to intptr_t values then converted into a string into the cmd line when CreateProcess() is called for the console (make sure inherit handles is set to true)
the handles should be layed out in the command line in this order: 1. **Command Pipe**(read end) 2. **Console Write Pipe**(read end) 3. **Console Read Pipe**(write end).

once the console process has started the handles which were inherited by the console can be closed with CloseHandle()

#### 4 other arguments are required on the cmd line:

4. **SIZE X**: \
   the horizontal size of the console window
5. **SIZE Y**: \
   the vertical size of the console window
6. **POS X**: \
   the horizontal position of the console window
7. **POS Y**: \
   the vertical position of the console window

if any of these values are negative they will to be converted to their abosolute value(positive) and the argument in question needs an 'n' appended before the number to inform the console this is a negative number.

You might want a negative position because if you have multiple monitors then position is calculated from the top left of the primary monitor which might not be the leftmost, to move the console left of or above the primary monitor you will require negative position values.

---

#### **Command Pipe**

Parent keeps the write end, console inherits the read end

2 different type of commands should be written to this pipe from the parent:

1. Exit: tells the console to close itself

2. Read + params(formatting below): tells the console to read from the console using specified params and write the data received to the **Console Read Pipe**

#### **Console Write Pipe**

Parent keeps the write end, console inherits the read end

This should be written to as if writing to std::cout(either put(), write() or operator<< )

#### **Console Read Pipe**

Parent keeps the read end, console inherits the write end

Should be read from after sending a Read + param command down the command pipe, the params should be properly used in order to replicate your desired istream function

#### **Command Pipe syntax**

> **each command pipe command should be appended with an '\n' character**

> **make sure to flush the command pipe after writing**

| CMD                    | Description                                                                                                                                                                                                                                                                                                                                                  |
| ---------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| e                      | Exit command, tells the console to exit                                                                                                                                                                                                                                                                                                                      |
| r;count;delim;extract; | read command + args, tells the console process to read from the console and send the output to the **console Read Pipe** using the args to perform the correct operatorion on the console in stream to make sure we replicate the correct behaviour, it is the responsibility of the parent to use these params correctly to replicate the desired behaviour |

##### read command args

| arg     | explaination                                                                                                                                                                                                             |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| count   | Indicates the number of characters to extract if the delim is not reached first, keep in mind that this number includes the null terminator '\0' so replication of void get(char c)/int get() will require a count of 2. |
| delim   | The delimiter character to be used in the extraction operation, if either '\0' or '\n' are desired the '\' needs to be escaped, so a command that desires to use '\n' as a delimiter might look like: `"r;10;\\n;0;"`    |
| extract | either a 1 or a 0 to indicate to the console process whether to extract the delimiter character from the stream or not, getline() for example does extract the delim, but get() does not.                                |

#### **Error Handling**

internally the console uses exceptions only for error handling, if an exception is encountered, the console will close after outputting the exception message to std::cerr, which the parent may want to redirect when calling CreateProcess so that these messages can be read.
