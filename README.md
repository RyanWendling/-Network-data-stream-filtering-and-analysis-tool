# -Network-data-stream-filtering-and-analysis-tool
Designed to sit in-between two programs (like a server and client), and display and manipulate the data streams flowing between them in each direction. Engineered using TCP/IP socket connections, scripting, forking, and the ncurses GUI. Programmed in C.

How to use: 
After logging on to multiple computers or SSHing through one computer. Start running the programs in the shell by giving arguements such as -rrport (right remote port address we want to connect to), -rraddr (right remote address, or the IP address we want to connect to on the right side), -llport (left local port, gives a custom local port you want to listen through), -lraddr (specifies an IP address we want to accept from the left side), -noleft (indicates head of the program chain and that we wont be listening for an incoming left connection), -noright (indicates tail of the program and that we wont be trying to connect to another program iteration on the right).

Example invocation: piggy4 -lraddr 140.160.140.5 -rraddr 140.160.140.5 -dsprl




Other features:


Typing "i" will put the program into command mode, this allows us to type in some of the following commands:

-dsplr:  display the data flowing from left to right on the screen. This is the default if neither of the
display options is specified.  
-dsprl:  display the data flowing from right to left on the screen   
-loopr:  This parameter causes the program to to take any data that arrives from the left and would possibly be
written out to the Right side and inject it into the data stream leaving the left side.   
-loopl:   This parameter causes piggy2 to take the data comes in from the right that possibly would be written
out to the Left side and inject it into the data stream leaving the right side.  
-i:   enter insert mode   
-esc:   exit insert mode and return to command mode.   
An esc character entered while in command mode is ignored.   
-outputl:   set the output direction to left   
-outputr:   set the output direction to right  
-output:   show what direction the output is set to   
-display:   show what direction the display is set to. Write display= either lr or rl   
-dropr:   drop the right side connection   
-dropl:   drop the left side connection   
-right:   show information about the right side connection*   
-left:   show information about the left side connection*   
-q:   terminate the program.   
-connectr IP [port]:    create a connection to computer with “IP” on their tcp port
“port” for your “right side” If a port is not specified the current
value of port for the remote port on the right is used. This may
have been specified on the command line or may have been
established via an interactive command. If it has never been set
than use the default port.   
-connectl IP [port]:    create a connection to computer with “IP” on their tcp port
“port” for your “left side.” See remarks above above the
optional port.   
-listenl [port]:    listen for a connection on your local port port. If port is not
given then use your default port number. Use this for your left
side.   
-listenr [port]:    listen for a connection on your local port port. If port is not
given then use your default port number. Use this for your right
side.   
-read filename:    read the contents of file “filename ” and write it to the current
output direction   
-llport [port]:    bind to local port “port” for a left side connection  
-rlport [port]:    bind to local port “port” for a right side connection  
-lrport [port]:   accept a connection on the left side only if the remote computer
attempting to connect has source port “port”  
-lraddr  IP:   When the left is put into passive mode (via a listenl command)
accept a connection on the left side only if the remote computer
attempting to connect has IP address “IP” If the left is placed in
active mode (trying to connect) use this as the address to connect to   
-rraddr  IP:   If the right is set to passive mode to accept a connection on the
right side, allow it only if the remote computer attempting to
connect has IP address “IP”. If the right is placed in active mode
(trying to make a connection) use this as the address to connect to.   
-reset:   Act as if the program is starting up from the beginning again and
do whatever was requested by the command line parameters. Any
active connections or passive opens are dropped and reset. The
screen should be cleared and redrawn  
-Internal filter commands:   
-stlrnp on:   strip non-printable characters moving from left to right    
-stlrnp off:   turn off stlrnp  
-strlnp on:   strip non-printable characters moving from right to left  
-strlnp off:   turn off strlnp  
-stlrnpxeol on:   strip non-printable characters moving from left to right except CR and LF   
-stlrnpxeol off:   turn off stlrnpxeol   
-strlnpxeol on:   strip non-printable characters moving from right to left except CR and LF   
-strlnpxeol off:   turn off strlnpxeol   
-Log commands:  
-loglrpre fn:   log left to right traffic before any filtering to file with name fn
-loglrpreoff:   turn off loglrpre
-logrlpre fn:   log right to left traffic before any filtering to file with name fn
-logrlpreoff:   turn off logrlpre
-loglrpost fn:   log left to right traffic after any filtering to file with name fn
-loglrpostoff:   turn off loglrpost
-logrlpost fn:   log right to left traffic after any filtering to file with name fn
-logrlpostoff:   turn off logrlpost
-source [scriptin]:   The script file scriptin is read. The characters in the file are interpreted as if you had typed them. If the end of the file is reached before the program is told to exit (with a quit command) , further characters are read from the keyboard. If the file name is not specified the program attempts to open a file with the name “scriptin.”    
-External filter:
-extlr command
-extrl command
-extlroff
-extrloff


-We use the "select" function for our non-blocking i/o. It will look at our three file descriptors and safely read from them, knowing we will recieve more data and not be blocked waiting for input to arrive.

-The ncurses GUI seperates our terminal window into 5 boxes with which to display the transfer of information. The upper left box shows data entering from the left side. The upper right box shows data leaving out the right side, the lower left box shows data leaving out the left side. The lower right box shows data entering from the right side. The final bottom box is where the user enters commands, data entry, and shows error reporting.

-Other tech used was the cbreak() mode, echo /noecho mode,
