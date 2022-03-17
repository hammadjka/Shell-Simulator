# Shell-Simulator
Simulates a basic shell. Executes shell commands as well as custom built commands using multiple processes and pipes.

Shell-Simulator recognizes the following lines  
    It recognizes lines of the form $<VAR>=<value>  
    It recognizes lines of the form <command> <arg0> <arg1> … <argN>, where <command> is a name of built-in command.  

Shell-Simulator supports the following built-in commands  
    exit, the shell terminates on this Command   
    log, the shell prints history of executed commands with time and return code  
    print, the shell prints argument given to this Command  
    theme, the shell changes the color of the output (red, green or blue).  

Shell-Simulator supports two modes.  
    Interactive  
      Prints out a prompt  
      Reads line  
      If the line is non-valid, prints an error message, else executes the command.  
  
    Script mode  
      As the name suggests, takes a script as a txt file of commands.  
      Executes the commands in order.  
 
