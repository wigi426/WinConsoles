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
the handles should be layed out int the command line in this order: 1. **Command Pipe** 2. **Console Write Pipe** 3. **Console Read Pipe**.

On both ends (parent and console) these pipe handles should be used to open std::i/ostreams.

this can be done in the following way:

```
int fd = _open_osfhanlde(pipehandle)
FILE* filePtr = _fdopen(fd)
std::i/ofstream fstream(filePtr)
std::i/ostream stream(filePtr.rdbuf())
```

---

#### **Command Pipe**

Parent keeps the write end, console inherits the read end

3 different type of commands should be written to this pipe from the parent:

1. Exit: tells the console to close itself

2. Write + _method_: tells the console to read from the **Console Write Pipe** and output it to the console using specified _method_

3. Read + _method_: tells the console to read from the console using specified _method_ and write the data received to the **Console Read Pipe**

#### **Console Write Pipe**

Parent keeps the write end, console inherits the read end

This should be written to after sending a Write + _method_ command down the command pipe
_method_ should match the ostream function that you want to replicate

#### **Console Read Pipe**

Parent keeps the read end, console inherits the write end

Should be read from after sending a Read + _method_ command down the command pipe
_method_ should match the istream funciton that you want to replicate

#### **Command Pipe syntax**

| CMD | Description                                                                                                                                                                                              |
| --- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| e   | Exit command, tells the console to exit                                                                                                                                                                  |
| w0  | write command + method, tells the console to write the next **console Write Pipe** input to the console using the number as a reference to the method that was used to get data for the console          |
| r0  | read command + method, tells the console to read from the console and send the output to the **console Read Pipe** using the number as reference to the method that we want to replicate the behviour of |
