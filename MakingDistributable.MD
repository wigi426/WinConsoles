considerations:

    1) cannot compile logger on runtime, unless you construct a cmake file then run then run the cmake the run the cmake build
        ^^^ i think thats the best option tbh, use 'lazy' compilation at runtime using a cmake file written as a literal into code
    2) Use cross platform solutions only:
        win32 functions which need to be replaced:
            CreateProcess()
            CreateFile()
            CreatePipe()
            SetHanldeInformation()
            MessageBox()
    3) build with CMAKE for cross compatability
