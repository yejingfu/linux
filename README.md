# Linux programming exercise
## network: the codes about network(TCP/IP)

## debug by GDB
* Resource:

** [Command Cheat Sheet](http://www.yolinux.com/TUTORIALS/GDB-Commands.html)

** [Example](http://www.ibm.com/developerworks/linux/library/l-gdb/)

    build the hello/eg1.c by gcc WITH -g option:
    $gcc -g eg1.c -o eg1.out
    $gdb eg1.out
    (gdb)break main # set breakpoint at the firt exe line within the main function
    (gdb)run      # run the application until encouter a breakpoint
    (gdb)list     # print the 10 lines around the breakpoint
    (gdb)<Enter>  # repeat last command
    (gdb)print var_name  # show the value of this variable
    (gdb)info locals     # show all local variables
    (gdb)info break
    (gdb)continue # continue to run the application to next breakpoint
    (gdb)break line_num # set a break point at specific line
    (gdb)step   # step into 
    (gdb)next   # next exec line

