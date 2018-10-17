# UNIX-Shell
A comprehensive UNIX-styled shell
___

**Background Information**

1. I/O Redirection

    * I/O redirection refers to changing a program's default methods of attaining input and/or displaying output.
    
    * All programs are initialized with 3 variables at runtime: stdin, stdout, & stderr. These variables are used as references by the program for their standard input, output, and errors, respectively.
    
    * stdin almost always refers to a user's keyboard whereas stdout is a user's screen.
    
    * With I/O redirection, we can hijack these destinations and have the program reference different locations.
    
    * For example, we can disable the stdout (screen) for a program and have it point to a text file instead. This, in effect, runs the program exactly as it normally would but its output would be in a user-specified file. The same logic holds for stdin, as well.
    
2. Piping

    * Piping is very similar to output/input redirection. As a matter of fact, it is I/O redirection, just with a more nuanced purpose.
    
    * Piping refers to taking the output of one program and using it as the input of another.
    
    * This is accomplished by creating a 'pipe' that two processes share. Their stdins/stdouts are then redirected to point this pipe, at either the write end or read end, depending which function is dependent on the other.
    
    * Essentially, we assign the stdout of program1 to the input end of the pipe. That program then exits and program 2 is launched, whose stdin is redirected to the same end of the pipe where program1 wrote its data. It then reads the pipe, uses its input as argument parameters and finally executes.

3. Backgrounding

    * Backgrounding is achieved by appending any command with a '&'.
    
    * When backgrounding a command, the program disables its stdout and appears as if it never ran the program at all (hence background).
    
    * Additionally, the shell typically waits for all commands to fully complete before prompting the user for another command. When backgrounded, however, the shell disregards this rule and immediately prompts the user for another command.

4. Program Environment

    * A program's environment is all the directories that are utilized by it. 
    
    * Typically, the UNIX shell stores its programs in /bin.
    
    * In this program, when a non-internal command is called, the shell will search for that executable in the current directory, then the /bin directory, and finally the shell's directory.
    
    * All of these paths are considered environment variables. They are naturally included in all shells and are crucial for proper program execution.


**Using the Shell**

Upon launch, the shell will be waiting for a command. Internal commands (built into program and guaranteed to execute on any system) include:
  
        * echo: Print the typed in command
        * cd: Print the current directory you are working in 
        (optionally can append cd with a path (i.e. ..) to change directories
        * dir: list all files in the current directory 
        (as above, can list a different directory if path is typed)
        * environ: list all the environment variables pertaining to the shell
        * clear: clear the terminal output log
        * pause: put the terminal on hold until a key is pressed
        * help: manual to help you navigate the shell
        * quit: qui the shell
        
In addition to these built-in functions, the shell supports execution of native UNIX programs, as well. If you are unfamiliar with these programs, you can read about them [here](https://maker.pro/linux/tutorial/basic-linux-commands-for-beginners).

At its core, this shell essentially serves as a non-graphical computer in the sense that it is capable of opening folders, creating files, running files, etc. all through command line input.

To execute a file, you may type out its file name (including extension!) and the shell will recognize it as a standalone executable. Otherwise, you may specify an absolute path to the file (i.e. /Users/[you]/Documents/hello.exe). If no directory is given, the shell will search for the file according to the logic outlined earlier in "4. Program Environment."

The shell's capability, however, extends beyond that of just running executables. As noted before, the terminal is capable of I/O redirection.

The exact syntax for executing an I/O redirection is detailed in the help manual when running the shell. 

If an I/O redirection is detected, the shell will either create an output file or search for an input file. As such, you must ensure the file you are using as an input redirection is either in the directory you currently reside in (use dir to check) or that you specify an absolute path to the said file. All output files are written to the current working directory (use cd to see where you are).

The final component to this shell involves batch command processing. With this feature, the shell has the capability to read a file containing a list of commands and consecutively execute them. This is achieved by redirecting the input of the main shell loop to point to that file. This feature can be accessed by first creating a .txt file containing commands with correct syntax on separate lines. The shell must then be launched (or any shell) and you can execute the shell itself, again, as if it were a built-in command with the name of the batch file (or absolute path) as an argument to that command! Another instance of the shell is then launched and automatically parses the file, executing all of its commands.

**Unfinished components**
1. pause()
    * huge bug where '\n' does not get cleared from input buffer despite using fflush(stdin)
    * As such, entering a character exits out of the pause() function but the \n still in the input stream completely breaks the program once it returns out of the function because the parser attempts to reference a bad address

2. clear()
    * printf("\033[H\033[2J") from guidelines did not work.
    * Tried innumerable variations of it from the internet, none worked.

3. input buffer bug
    * this bug is arguably the reason none of these other components work
    * all seem to fail on some exception error due to the while loop skipping over the next fgets() and still referencing the leftover '\n' from the last command
    * tried to fix this problem by setting \n to \0 manually, didn't have an effect besides on the actual buffer itself that holds the line, not the input stream
    * tried scanf, getchar, while(getchar != NULL), while(scanf != NULL), etc., spent hours trying to figure this out