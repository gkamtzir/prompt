This is a very basic command line interpreter for Linux Operating Systems. It supports both interactive mode,
where the user can execute all the commands that can be executed via the execvp function, and batch mode, where
the user provides a batch file to be executed.

This tool was developed for the semester's project of Operating Systems class of Electrical Engineering and Computer Engineering
department at Aristotle University of Thessaloniki.

The code is in the shell.c file. Use the terminal to build, execute or delete the executable:

    $ make
    $ ./shell
    $ ./shell <batch file>
    $ ./shell_strict
    $ ./shell_strict <batch file>
	$ make clean

Georgios Kamtziridis, Winter Semester 2017-2018
