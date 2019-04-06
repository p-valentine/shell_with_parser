CURRENT CONDITION
-----------------------------
Currently the shell can execute commands (ex. firefox, echo "hello", ls)
as well as built in commands (cd, help, history)

history works

Currently implemented kill works where the user has to manually enter the pid 
    example - get pid from htop and then call kill with the pid

Can redirect stdin and stdout

-----------------------
TO-DO / Not Working
-----------------------
exit is not working currently as well as jobs
exit worked before I began the assignment, somehow I broke it 

I began implementation of jobs however could not finish it, I tried getting it so that 
when a new process was forked, it added it to my jobs array, but I was getting errors
and I was unable to get it working.


I didn't get !number working as indicated in #7 on the shell assignment page
