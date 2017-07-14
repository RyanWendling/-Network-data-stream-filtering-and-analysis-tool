/*Ryan Wendling
 * assignment4 Piggy4, 
 * Meehan 367, Spring 2016
 * Piggy4 adds a command interface, an accept checker, use of the select statement, full use of everysocket and stdinput and error checking.
 */
#include <curses.h>
#include <locale.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#define DEFAULTPORT 36761           /* default protocol port number */
#define DEFAULTPORT2 46761           /* default protocol port number for lrport*/
#include <sys/socket.h>
#define QLEN 6                      /* size of request queue */
#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>             // contains constants and structures needed for internet domain addresses
#include <arpa/inet.h>              // for UNIX
#include <netdb.h>
#endif

#define maxwin 6
static  WINDOW *w[maxwin];
static ww[maxwin];
static wh[maxwin];
static wrpos[maxwin];
static wcpos[maxwin];
static chtype ls,rs,ts,bs,tl,tr,bl,br;

extern int optind;

void wAddstr(int i, char s[132])
{
  int j,l,y,x;
  getyx(w[i],y,x);      // find out where we are in the window
  y=y?y:!y;
  x=x?x:!x;  
  wrpos[i]=y;
  wcpos[i]=x;
  l=strlen(s);
  for (j=0;j<l;j++)
    {
      if (++wcpos[i]==ww[i]) 
	{
	  wcpos[i]=1;
	  if (++wrpos[i]==wh[i]) { wrpos[i]=1; }
	  
	}	
      mvwaddch(w[i],wrpos[i],wcpos[i],(chtype) s[j]);   
    }
  wrefresh(w[i]);
}

char hexDigit(unsigned n)
{
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}


void charToHex(char c, char hex[3])
{
    hex[0] = hexDigit(c / 0x10);
    hex[1] = hexDigit(c % 0x10);
    hex[2] = '\0';
}



void print_usage() {
    printw("usage, somethin went wrong? \n");
}

int outterSafe = 1;
char** holdargv;

int argcCopy;

int main(int argc, char *argv[]) {

int firstGo = 0;
  argcCopy = argc;  
  holdargv = malloc((sizeof(char* )*(argc+1)));  
  do {
  
  optind = 0;    
  if (firstGo == 0) {
     for (int c = 0; c < argc; c++) {
     holdargv[c] = strdup(argv[c]);//, sizeof(argv[c]));
      }
      holdargv[argc] = NULL;
  }
  if (firstGo == 1) {
     for (int c = 0; c < argcCopy; c++) {
     argv[c] = strdup(holdargv[c]);//, sizeof(argv[c]));
      }
      argv[argcCopy] = NULL;
      argc = argcCopy;
      return main(argc, argv);
  }
  
  outterSafe = 1;
  
  
  
  int k,j,a,b,c,d,nch; 
  chtype ch;
  char response[132];
  ch=(chtype) " ";
  ls=(chtype) 0;
  rs=(chtype) 0;
  ts=(chtype) 0;
  bs=(chtype) 0;
  tl=(chtype) 0;
  tr=(chtype) 0;
  bl=(chtype) 0;
  br=(chtype) 0;

  setlocale(LC_ALL,"");       // this has to do with the character set to use
  initscr();         // must always call this (or newterm) to initialize the
                     // library before any routines that deal with windows
                     // or the screen can be called
    
 cbreak();          // this allows use to get characters as they are typed
                     // without the need for the userpressing the enter key 
 noecho();          // this means the characters typed are not 
                     // shown on the screen unless we put them there
 nonl();            // this means don't translate a newline character
                     // to a carraige return linefeed sequence on output
 intrflush(stdscr, FALSE);  // 
 keypad(stdscr, TRUE);       // 


  if (!(LINES==43) || !(COLS==132) ) 
    { 
      if (resizeterm(43,132)==ERR)
	{
	  clear();
	  move(0,0);
	  addstr("Piggy3 requires a screen size of 132 columns and 43 rows");
	  move(1,0);
	  addstr("Set screen size to 132 by 43 and try again");
	  move(2,0);
	  addstr("Press enter to terminate program");
	  refresh();
	  getstr(response);            // Pause so we can see the screen 
	  endwin();
	  exit(EXIT_FAILURE);
	}
    }
  clear();             // make sure screen is clear before we start
  w[0]=newwin(0,0,0,0);
  touchwin(w[0]);
  wmove(w[0],0,0);
  wrefresh(w[0]);
  
  // create the 5 windows 
  a=18; b=66; c=0; d=0;
  w[1]=subwin(w[0],a,b,c,d);
  w[2]=subwin(w[0],a,b,c,b);
  w[3]=subwin(w[0],a,b,a,c);
  w[4]=subwin(w[0],a,b,a,b);
  w[5]=subwin(w[0],7,132,36,c);
  for (k=1;k<maxwin-1;k++) 
    { 
      ww[k]=b-1;
      wh[k]=a-1;
    }
  ww[5]=131;
  wh[5]=6;
  // draw a border on each window
  for (k=1;k<maxwin;k++)
    { wattron(w[k], A_STANDOUT);
      wborder(w[k],ls,rs,ts,bs,tl,tr,bl,br);
      wrefresh(w[k]);
      wattroff(w[k], A_STANDOUT);
      wrpos[k]=1;
      wcpos[k]=1;
    } 
  wmove(w[1], 8, 2);
  wAddstr(1,"Data entering from the left");
  wmove(w[2], 8, 2);
  wAddstr(2,"Data leaving out the right side");
  wmove(w[3], 8, 2);
  wAddstr(3,"Data leaving out the left side");
  wmove(w[4], 8, 2);
  wAddstr(4,"Data entering from the right"); 
  // Place cursor at top corner of window 5
  wmove(w[5],2,5);  
  //wprintw(w[5],"Hit enter to continue");
  //wgetstr(w[5],response);            // Pause so we can see the screen 
  wmove(w[5],1,1);
  wclrtoeol(w[5]);
  wrefresh(w[5]);
  
  
    // external filter flags
    int extlrON = 0;
    int extrlON = 0;
    //pipes stuff
    
    
    //script stuff
    char* scriptFileName;
    char scriptFileNamebuf[1000];
    int scriptFileNameDescriptor;
    char* scriptFileName2;
    char scriptFileNamebuf2[1000];
    int scriptFileNameDescriptor2;    
    int oldStdin;
    
    //used in piggy4 for filter logs
    int loglrpreON = 0;
    int logrlpreON = 0;
    int loglrpostON = 0;
    int logrlpostON = 0;
    FILE *loglrpreFile;
    FILE *logrlpreFile;
    FILE *loglrpostFile;
    FILE *logrlpostFile;

  
    int notprintabletoleft = 0;
    int notprintabletoright = 0;
    int notprintabletolefteol = 0;
    int notprintabletorighteol = 0;
    //two counters used to clear the output boxes when necessary.
    int dispCounter = 0;
    int dispCounter2 = 0;
    int first = 1;
    int first2 = 1;
  
    //for reset
    int innerSafe = 1;
    
    //for identifyng if there is a right or left connection/ which one is which
    int activeLeft = 0;
    int activeRight = 0;
    
    int opt= 0;
    char *RRADDR;
    char *LRADDR;
    //use these for lraddr/rraddr command checkers
    char compareRRADDR[100];
    char compareLRADDR[100];
    int rightCheckAddr = 0;
    int leftCheckAddr = 0;
    int lraddrSize = 0;
    int rraddrSize = 0;
    
    int RRPORT = DEFAULTPORT;
    int LRPORT = DEFAULTPORT;
    int LLPORT = DEFAULTPORT;
    
    //used for piggy3 when we have two passive connections.
    int RLPORT = DEFAULTPORT2;
    
    int NOLEFT = -1;
    int NORIGHT = -1;
    int arguementNum = -1;
    int DSPLR = 1;
    int DSPRL = -1;
    int twodirect = 0;
    int LOOPR = -1;
    int LOOPL = -1;
    int OUTPUTLR = 1;                 //makes it so data can only travel left to right, except for loop. 
    int OUTPUTRL = -1;
    int Leftlistensocket;           //socket descriptors
    
    // CHANGE VALUE TO -1 TO MAKE NON-VALID SOCKET?
    int waitingLeft = 0;
    int waitingRight = 0;
    int Rightlistensocket;          //need this in case we have two passive connections   
    int leftListenActive = 0;
    int rightListenActive = 0;
    
    int Rightsocket;
    int Leftsocket;
    int n;                          /* number of characters read */
    int alen;                       /* length of address */
    char buf[1000];                 /* buffer for string the server sends */
    char stripbuf[1000];            //used to delete from and write out with when filters are on.
    char stripeolbuf[1000];
    
    char transfer[1000];            // used to put together some input from buf to potentially use for commands
    int counter = 0;
    int READY = 0;
    
    struct hostent *ptrh;           /* pointer to a host table entry, holds host info */ 
    struct hostent *ptrh2;          /* pointer to a host table entry, holds host info, only for comparing LRADDR */ 
    struct protoent *ptrp;          /* pointer to a protocol table entry */
    struct sockaddr_in leftThisPad;     /* structure to hold THIS piggy's address */
    struct sockaddr_in rightThisPad;    // in case of two passive connections!!
    struct sockaddr_in rightPad;     /* structure to hold next/RIGHT piggy's address */
    struct sockaddr_in leftPad;    // for two active connections
    struct sockaddr_in leftPrevPad;     /* structure to hold incoming piggy's address */
    struct sockaddr_in rightPrevPad;     /* structure to hold incoming piggy's address */
    memset((char *)&leftThisPad,0,sizeof(leftThisPad)); /* clear sockaddr structure */
    memset((char *)&rightThisPad,0,sizeof(rightThisPad)); /* clear sockaddr structure */
    memset((char *)&rightPad,0,sizeof(rightPad));
    memset((char *)&leftPad,0,sizeof(leftPad));
    memset((char *)&rightPrevPad,0,sizeof(rightPrevPad));
    memset((char *)&leftPrevPad,0,sizeof(leftPrevPad));
    //close(Leftlistensocket);
    //close(Rightlistensocket);
    
    
    int i;
    fd_set read_fds;  // temp file descriptor list for select()
    fd_set master;    // master file descriptor list
    int fdmax;        // maximum file descriptor number
    FD_ZERO(&master);    // clear the master set
    FD_ZERO(&read_fds);
    

    int COMM = 0;
    //all this is used to compare LRADDR
    socklen_t leftlen;
    int LEFTADDRCHECK = 0;
    int HAVELRADDR = 0;
    int LEFTPORTCHECK = 0;
    int HAVELRPORT = 0;
    struct sockaddr_storage leftaddr;
    char leftipstr[INET6_ADDRSTRLEN];
    int leftport;
    
    //need second set of these for middle piggy
    socklen_t rightlen;
    struct sockaddr_storage rightaddr;
    char rightipstr[INET6_ADDRSTRLEN];
    int rightport;
    
    //Specifying the expected options
    static struct option long_options[] = {
        {"noleft",      no_argument,       0,  'e' },
        {"noright",     no_argument,       0,  'i' },
        {"rraddr",    required_argument, 0,  'a' },
        {"lraddr",    required_argument, 0,  'r' },        
        {"rrport",   optional_argument, 0,  'p' },
        {"lrport",   required_argument, 0,  'o' },
        {"llport",   optional_argument, 0,  'l' },
        {"dsplr",    no_argument,      0,   'd' },
        {"dsprl",    no_argument,      0,   't' },
        {"loopr",    no_argument,      0,   'm' },
        {"loopl",    no_argument,      0,   'z' },    
        {"s",        optional_argument, 0,   's' },
        {0,           0,                 0,  0   }
    };

    int long_index =0;
    
    while ((opt = getopt_long_only(argc, argv,"eiardsmzo:spl", 
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'e' :
                wAddstr(5,"noleft, take standard input ");
                NOLEFT = 1;
                arguementNum = 1;
                break;
                 
             case 'i' :
                wprintw(w[5],"noright, print out to screen only ");
                NORIGHT = 1;
                arguementNum = 1;
                 break;
                 
             case 'a' :
                 wAddstr(5,"The right direction ip address was given ");
                RRADDR = optarg;
                arguementNum = 1;
                wAddstr(5, RRADDR);
                 break;
                 
             case 'r' :
                wAddstr(5,"The left direction ip address was given ");                
                LRADDR = optarg;
                arguementNum = 1; 
                if (optarg[0] == '*') {
                    wAddstr(5,"any incoming left address will suffice ");
                    LEFTADDRCHECK = 1;
                }
                wAddstr(5, optarg);
                HAVELRADDR = 1;
                 break;                 
                 
             case 'p' : 
                if (optarg && atoi(optarg) != 0) {
                    wAddstr(5,"The right direction port was given ");
                    RRPORT = atoi(optarg);
                    wAddstr(5, optarg);
                    arguementNum = 1;
                    
                } else {
                    wAddstr(5,"the right direction port will use the default port ");
                    RRPORT = DEFAULTPORT;
                    arguementNum = 1;
                }
                
                 break;
                 
             case 'o' : 
                    wAddstr(5,"The left direction port was given ");
                    LRPORT = atoi(optarg);
                    arguementNum = 1;
                    if (optarg[0] == '*') {
                        wAddstr(5,"any incoming left port will suffice ");
                        LEFTPORTCHECK = 1;
                    }    
                   wAddstr(5, optarg);
                    HAVELRPORT = 1;
                 break;                 
                 
             case 'l' : 
                if (optarg && atoi(optarg) != 0) {
                    wAddstr(5,"The local port was given ");
                    LLPORT = atoi(optarg);
                    wAddstr(5, optarg);
                    arguementNum = 1;
                    
                } else {
                    wAddstr(5,"the local port will use the default port ");
                    LLPORT = DEFAULTPORT;
                    arguementNum = 1;
                }
                 break; 
                 
             case 'd' : 
                wAddstr(5,"the direction of output data will be left to right, normal ");
                DSPLR = 1;
                twodirect++;
                break;
                 
             case 't' :
                wAddstr(5,"the direction of output data will be right to left, reverse ");
                DSPLR = -1;
                DSPRL = 1;
                twodirect++;
                break;     
                
             case 'm' : 
                wAddstr(5,"data coming from the left is reflected back, this is for the tail ");
                LOOPR = 1;
                break;
                 
             case 'z' :
                wAddstr(5,"data coming from the right is reflected back, this is for the head ");
                LOOPL = 1;
                break;        
                
             case 's' :
                wAddstr(5,"reading script ");
                scriptFileName = optarg;
                oldStdin = dup(STDIN_FILENO);
                scriptFileNameDescriptor = open(scriptFileName, O_RDONLY);
                dup2(scriptFileNameDescriptor, 0);
                int scriptSize = read(scriptFileNameDescriptor, scriptFileNamebuf, 1000);
                wmove(w[5], 4, 2);
                wclrtoeol(w[5]);
                wAddstr(5, scriptFileNamebuf);
                strncpy(buf, scriptFileNamebuf, 1000);
                stripbuf[scriptSize] = '\0';
                dup2(oldStdin, 0);
                
                break;                  
                 
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }

    wmove(w[5],3,3);
    
    //error cases from input parameters.
    if (arguementNum == -1) {
        wAddstr(5,"NO INPUT PARAMETERS");
        exit(EXIT_FAILURE);
    }    
    
    if (RRADDR == NULL && NORIGHT == -1) {
        wAddstr(5,"NOWHERE TO CONNECT");
        exit(EXIT_FAILURE);
    } 
    
    if (twodirect == 2) {
        wAddstr(5,"CAN'T GO BOTH WAYS");
        exit(EXIT_FAILURE);
    }     
 
    
    /* Map TCP transport protocol name to protocol number */
    if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "cannot map \"tcp\" to protocol number");
        exit(EXIT_FAILURE);
    }
       
       
       
        
    //Enters the needed fields, we need to set up our own socket before we go connect with the RIGHT. SERVER
    rightThisPad.sin_port = htons((u_short)RLPORT);            //converts to TCP/IP network byte order, which is big-endian.
    rightThisPad.sin_family = AF_INET;                            /* set family to Internet */
    rightThisPad.sin_addr.s_addr = INADDR_ANY;                    /* set the local IP address, binds socket to all */    
        
    //make a listening socket
    Rightlistensocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Rightlistensocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(1);
    } 
    //Eliminate address already in use message.    
    int flag3 = 1;
    if (setsockopt (Rightlistensocket, SOL_SOCKET, SO_REUSEADDR, &flag3, sizeof(int)) == -1) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "setsockopt error eliminator failed");
         exit(1);
    }    
    //CONNECT TO LEFT SIDE aka bind a local address to socket
    if (bind (Rightlistensocket, (struct sockaddr *)&rightThisPad, sizeof(rightThisPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "right bind failed");
        exit(1);
    } 
    
    
    
    
    
    
    
    
    // Do client and server stuff, reads in data from buffer and writes it onward. The middle piggy
    if (NORIGHT == -1 && NOLEFT == -1) {
    
    activeLeft = 1;
    activeRight = 1;
    //Enters the needed fields, we need to set up our own socket before we go connect with the RIGHT. SERVER
    leftThisPad.sin_port = htons((u_short)LLPORT);            //converts to TCP/IP network byte order, which is big-endian.
    leftThisPad.sin_family = AF_INET;                            /* set family to Internet */
    leftThisPad.sin_addr.s_addr = INADDR_ANY;                    /* set the local IP address, binds socket to all */    
        
    //make a listening socket
    Leftlistensocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Leftlistensocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(1);
    } 
    //Eliminate address already in use message.    
    int flag = 1;
    if (setsockopt (Leftlistensocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "setsockopt error eliminator failed");
         exit(1);
    }  
    
    //CONNECT TO LEFT SIDE aka bind a local address to socket??
    if (bind (Leftlistensocket, (struct sockaddr *)&leftThisPad, sizeof(leftThisPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "left bind failed");
        exit(1);
    }
    
    leftListenActive = 1;
    
    // Specify size of request queue
    if (listen(Leftlistensocket, QLEN) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "listen failed");
        exit(1);
    }
        alen = sizeof(leftPrevPad);
    if ( (Leftsocket = accept(Leftlistensocket, (struct sockaddr *)&leftPrevPad, &alen)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "accept failed");
        exit(EXIT_FAILURE);
    }
    

    
        //Enters the needed fields, we need to connect this with a remote socket on the RIGHT, given an rrport and rraddr. CLIENT
    rightPad.sin_port = htons((u_short)RRPORT);            //converts to TCP/IP network byte order, which is big-endian.
    rightPad.sin_family = AF_INET;                         /* set family to Internet */
    
    /* Convert host name to equivalent IP address */
    ptrh = gethostbyname(RRADDR);
    if ( ((char *)ptrh) == NULL ) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "invalid host:");
        wAddstr(5, RRADDR);
        exit(EXIT_FAILURE);
    }
    memcpy(&rightPad.sin_addr, ptrh->h_addr, ptrh->h_length);
    
    
                           //CLIENT
    /* Create a connecting socket. */
    Rightsocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Rightsocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(EXIT_FAILURE);
    }   
    /* Connect the socket to the specified server. */
    if (connect(Rightsocket, (struct sockaddr *)&rightPad, sizeof(rightPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "connect failed");
        exit(EXIT_FAILURE);
    } 
    
                            //NEW
    // add the incoming socket to the master set
    FD_SET(Leftsocket, &master);
    FD_SET(Rightsocket, &master);
    FD_SET(0, &master);
    if (Leftsocket > Rightsocket) {
        fdmax = Leftsocket;
    } else {
        fdmax = Rightsocket;
    }

    
        // these two blocks get information about our two connected sockets.
        leftlen = sizeof leftaddr;
        getpeername(Leftsocket, (struct sockaddr*)&leftaddr, &leftlen); 
        
        // deal with both IPv4 and IPv6:
        if (leftaddr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&leftaddr;
            leftport = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, leftipstr, sizeof leftipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&leftaddr;
            leftport = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, leftipstr, sizeof leftipstr);
        }
        
        
        rightlen = sizeof rightaddr;
        getpeername(Rightsocket, (struct sockaddr*)&rightaddr, &rightlen); 
        
        // deal with both IPv4 and IPv6:
        if (leftaddr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&rightaddr;
            rightport = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, rightipstr, sizeof rightipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&rightaddr;
            rightport = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, rightipstr, sizeof rightipstr);
        }

        
        
        //do this comparison if we gave the lraddr arguement.
        if (LEFTADDRCHECK == 0 && HAVELRADDR == 1) {
            if (strncmp(leftipstr, LRADDR, leftlen)) {
                wmove(w[5], 2, 2);    
                wAddstr(5, "disconnect left socket, address incorrect");
                close(Leftsocket);
                exit(1);
            }
        }    
        //do this comparison if we gave the lrport arguement
        if (LEFTPORTCHECK == 0 && HAVELRPORT == 1) {
            if (!(leftport == LRPORT)) {
                wmove(w[5], 2, 2);    
                wAddstr(5, "disconnect left socket, port incorrect");
                close(Leftsocket);
                exit(1);
            }
        }
    }
    
    
    
    
    
    
    
    
    if (NORIGHT == -1 && NOLEFT == 1) {         //Client stuff only, stdin, HEAD
        
    activeRight = 1;    
    //Enters the needed fields, we need to connect this with a remote socket on the RIGHT, given an rrport and rraddr. CLIENT
    rightPad.sin_port = htons((u_short)RRPORT);            //converts to TCP/IP network byte order, which is big-endian.
    rightPad.sin_family = AF_INET;                         /* set family to Internet */
    
    /* Convert host name to equivalent IP address */
    ptrh = gethostbyname(RRADDR);
    if ( ((char *)ptrh) == NULL ) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "invalid host:");  
        wAddstr(5, RRADDR);
        exit(EXIT_FAILURE);
    }
    memcpy(&rightPad.sin_addr, ptrh->h_addr, ptrh->h_length);
    
    
                           //CLIENT
    /* Create a connecting socket. */
    Rightsocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Rightsocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(EXIT_FAILURE);
    }   
    /* Connect the socket to the specified server. */
    if (connect(Rightsocket, (struct sockaddr *)&rightPad, sizeof(rightPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "connect failed");
        exit(EXIT_FAILURE);
    } 
                                //NEW
                                
    rightlen = sizeof rightaddr;
    getpeername(Rightsocket, (struct sockaddr*)&rightaddr, &rightlen); 
        
        // deal with both IPv4 and IPv6:
    if (rightaddr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&rightaddr;
        rightport = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, rightipstr, sizeof rightipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&rightaddr;
        rightport = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, rightipstr, sizeof rightipstr);
    }
    
    
    // add the incoming socket to the master set
    FD_SET(Rightsocket, &master);
    FD_SET(0, &master);
    fdmax = Rightsocket;  
    
        
    //THIS IS FOR A POTENTIAL LISTEN ON THE LEFT AT SOME POINT, USING LLPORT
    leftThisPad.sin_port = htons((u_short)LLPORT);            //converts to TCP/IP network byte order, which is big-endian.
    leftThisPad.sin_family = AF_INET;                            /* set family to Internet */
    leftThisPad.sin_addr.s_addr = INADDR_ANY;                    /* set the local IP address, binds socket to all */    
    //make a listening socket
    Leftlistensocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Leftlistensocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(1);
    } 
    //Eliminate address already in use message.    
    int flag = 1;
    if (setsockopt (Leftlistensocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "setsockopt error eliminator failed");
         exit(1);
    }    
    //CONNECT TO LEFT SIDE aka bind a local address to socket
    if (bind (Leftlistensocket, (struct sockaddr *)&leftThisPad, sizeof(leftThisPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "bind failed");
        exit(1);
    }    
    }
    
    
    
       
      
      
      
      
      
    
    if (NORIGHT == 1 && NOLEFT == -1) {         //Server stuff only, stdout, TAIL

    activeLeft = 1;
    OUTPUTRL = 1;
    OUTPUTLR = -1;
                            //SERVER
    //Enters the needed fields, we need to set up our own socket before we go connect with the RIGHT. SERVER
    leftThisPad.sin_port = htons((u_short)LLPORT);            //converts to TCP/IP network byte order, which is big-endian.
    leftThisPad.sin_family = AF_INET;                            /* set family to Internet */
    leftThisPad.sin_addr.s_addr = INADDR_ANY;                    /* set the local IP address, binds socket to all */    
        
    //make a listening socket
    Leftlistensocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (Leftlistensocket < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "socket creation failed");
        exit(1);
    } 
    //Eliminate address already in use message.    
    int flag = 1;
    if (setsockopt (Leftlistensocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "setsockopt error eliminator failed");
         exit(1);
    }    
    //CONNECT TO LEFT SIDE aka bind a local address to socket
    if (bind (Leftlistensocket, (struct sockaddr *)&leftThisPad, sizeof(leftThisPad)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "bind failed");
        exit(1);
    }  
    
    leftListenActive = 1;
    
    // Specify size of request queue
    if (listen(Leftlistensocket, QLEN) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "listen failed");
        exit(1);
    }
        alen = sizeof(leftPrevPad);
    if ( (Leftsocket = accept(Leftlistensocket, (struct sockaddr *)&leftPrevPad, &alen)) < 0) {
        wmove(w[5], 2, 2);    
        wAddstr(5, "accept failed");
        exit(EXIT_FAILURE);
    }
                        //NEW
    
        leftlen = sizeof leftaddr;
        getpeername(Leftsocket, (struct sockaddr*)&leftaddr, &leftlen); 
        
        // deal with both IPv4 and IPv6:
        if (leftaddr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&leftaddr;
            leftport = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, leftipstr, sizeof leftipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&leftaddr;
            leftport = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, leftipstr, sizeof leftipstr);
        }

        //do this comparison if we gave the lraddr arguement.
        if (LEFTADDRCHECK == 0 && HAVELRADDR == 1) {
            if (strncmp(leftipstr, LRADDR, leftlen)) {//if (ipstr == LRADDR){
                wmove(w[5], 2, 2);    
                wAddstr(5, "disconnect left socket, address incorrect");
                close(Leftsocket);
                exit(1);
            }
        }    
        //do this comparison if we gave the lrport arguement
        if (LEFTPORTCHECK == 0 && HAVELRPORT == 1) {
            if (!(leftport == LRPORT)) {//if (ipstr == LRADDR){
                wmove(w[5], 2, 2);    
                wAddstr(5, "disconnect left socket, port incorrect");
                close(Leftsocket);
                exit(1);
            }
        } 

    // add the incoming socket to the master set
    FD_SET(Leftsocket, &master);
    FD_SET(0, &master);
        fdmax = Leftsocket;  
    
    }
    
    
    
    wmove(w[5], 2, 2);    
    wAddstr(5, "waiting, next buffer will be input");
    wmove(w[5], 5, 2);
    
    
    
    
    
    
            /* Main server loop - accept and handle requests */

    while (innerSafe == 1) {
        
        
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            wmove(w[5], 2, 2);    
            wAddstr(5, "nothing to select");
            wmove(w[5], 5, 2);
            wclrtoeol(w[5]);
            endwin();
            exit(4);
        }
        
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                

                if (i == Leftlistensocket) {// && waitingLeft == 1) {
                    wAddstr(5, "accepting left..");
                    alen = sizeof(leftPrevPad);
                    if ( (Leftsocket = accept(Leftlistensocket, (struct sockaddr *)&leftPrevPad, &alen)) < 0) {
                    } else {
                        //close(Leftlistensocket);
                        //FD_CLR(Leftlistensocket, &master);
                        //FD_CLR(Leftlistensocket, &read_fds);
                        wmove(w[5], 2, 2);
                        wclrtoeol(w[5]);
                        wAddstr(5, "left accept made");
                        FD_SET(Leftsocket, &master);
                        
                        if (Leftsocket > fdmax) {
                            fdmax = Leftsocket;
                        }
                        activeLeft = 1;
                        
                        //all this is used to compare LRADDR
                        socklen_t leftlen2;
                        struct sockaddr_storage leftaddr2;
                        char leftipstr2[INET6_ADDRSTRLEN];
                        int leftport2;

                        
                        leftlen2 = sizeof leftaddr2;
                        getpeername(Leftsocket, (struct sockaddr*)&leftaddr2, &leftlen2); 
        
                        // deal with both IPv4 and IPv6:
                        if (leftaddr.ss_family == AF_INET) {
                            struct sockaddr_in *s = (struct sockaddr_in *)&leftaddr2;
                            leftport2 = ntohs(s->sin_port);
                            inet_ntop(AF_INET, &s->sin_addr, leftipstr2, sizeof leftipstr2);
                        } else { // AF_INET6
                            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&leftaddr2;
                            leftport2 = ntohs(s->sin6_port);
                            inet_ntop(AF_INET6, &s->sin6_addr, leftipstr2, sizeof leftipstr2);
                        }

                        //do this comparison if we gave the lraddr arguement.
                        if (leftCheckAddr == 1) {
                            
                            if (strncmp(leftipstr2, compareLRADDR+1, lraddrSize - 2)) {//if (ipstr == LRADDR){
                                //printw("disconnect left socket, address incorrect\n");
                                wmove(w[5], 2, 2);    
                                wAddstr(5, "disconnect left socket, address incorrect");
                                close(Leftsocket);
                                FD_CLR(Leftsocket, &master);
                                activeLeft = 0;
                                leftCheckAddr = 0;
                            }
                        }
                    }    
                    waitingLeft = 0;
                    
                }
                if (i == Rightlistensocket) {  
                    alen = sizeof(rightPrevPad);
                    if ( (Rightsocket = accept(Rightlistensocket, (struct sockaddr *)&rightPrevPad, &alen)) < 0) {
                    }  else {
                        wmove(w[5], 2, 2);
                        wclrtoeol(w[5]);
                        wAddstr(5, "right accept made duid");
                        FD_SET(Rightsocket, &master);
                        
                        if (Rightsocket > fdmax) {
                            fdmax = Rightsocket;
                        }
                        activeRight = 1;
                        
                        //all this is used to compare LRADDR
                        socklen_t rightlen2;
                        struct sockaddr_storage rightaddr2;
                        char rightipstr2[INET6_ADDRSTRLEN];
                        int rightport2;

                        
                        rightlen2 = sizeof rightaddr2;
                        getpeername(Rightsocket, (struct sockaddr*)&rightaddr2, &rightlen2); 
        
                        // deal with both IPv4 and IPv6:
                        if (rightaddr.ss_family == AF_INET) {
                            struct sockaddr_in *s = (struct sockaddr_in *)&rightaddr2;
                            rightport2 = ntohs(s->sin_port);
                            inet_ntop(AF_INET, &s->sin_addr, rightipstr2, sizeof rightipstr2);
                        } else { // AF_INET6
                            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&rightaddr2;
                            rightport2 = ntohs(s->sin6_port);
                            inet_ntop(AF_INET6, &s->sin6_addr, rightipstr2, sizeof rightipstr2);
                        }

                        //do this comparison if we gave the lraddr arguement.
                        if (rightCheckAddr == 1) {
                            if (strncmp(rightipstr2, compareRRADDR+1, rraddrSize - 2)) {
                                wmove(w[5], 2, 2);    
                                wAddstr(5, "disconnect right socket, address incorrect");
                                close(Rightsocket);
                                FD_CLR(Rightsocket, &master);
                                activeRight = 0;
                                rightCheckAddr = 0;
                            }
                        }                        
                    } 
                    waitingRight = 0;
                }                
                
                
                
                memset(buf, '\0', sizeof(buf));
                memset(stripbuf, '\0', sizeof(stripbuf));
                memset(stripeolbuf, '\0', sizeof(stripeolbuf));
                int recievedSize = read(i, buf, 1000);
                if (recievedSize == 0 || recievedSize == -1) {
                    wmove(w[5], 4, 2);
                    wclrtoeol(w[5]);
                    wAddstr(5, "incorrect/closed socket");
                    FD_CLR(i, &master);
                    FD_CLR(i, &read_fds);
                    close(i);
                }
                
                // clears the rest of the buffer
                buf[recievedSize] = '\0';
                strncpy(stripbuf, buf, 1000);
                strncpy(stripeolbuf, buf, 1000);
                stripbuf[recievedSize] = '\0';
                stripeolbuf[recievedSize] = '\0';
                
                char bufsize[10];
                int len = strlen(buf);
                int recievedSize2 = recievedSize;
                int recievedSize3 = recievedSize;
               
                int notprintable = 0;
                int notprintableeol = 0;
                int notprintableNLleft = 0;
                int notprintableNLright = 0;

                
                    
                    for (int t = 0; t < recievedSize2; t++) {
                        int comp = stripeolbuf[t];
                        if ((comp < 32 || comp > 126) && (comp != 13 && comp != 10 && comp != 27)) {
                            char hexBuffer[3];
                            charToHex(stripeolbuf[t], hexBuffer);
                            wAddstr(5, hexBuffer);
                            memmove(&stripeolbuf[t], &stripeolbuf[t + 1], strlen(stripeolbuf) - t);
                            t--;
                            recievedSize2--;
                        }
                    }                     
                    
                    
                    for (int t = 0; t < recievedSize3 ; t++) {
                        int comp = stripbuf[t];
                        if ((comp < 32 || comp > 126) && (comp != 27)) {
                            memmove(&stripbuf[t], &stripbuf[t + 1], sizeof(stripbuf) - t);                     
                            t--;
                            recievedSize3--;                       
                        }

                    }
                
                    
                // internal buffer for commands, making sure its printable
                if (COMM == 1 && i == 0) {
                    if (notprintable == 0) {
                        wAddstr(5, stripbuf);
                        transfer[counter] = stripbuf[0];
                        counter++;
                        if (transfer[counter - 1] == '-') {
                            READY = 1;
                            transfer[counter - 1] = '\0';
                            counter = 0;
                        } 
                    }    
                }    
                
                
                
                if (activeRight == 0 && activeLeft == 1) {
                // prints stuff that is coming from other sockets
                 if ((i != 0 && buf[0] != '\e')) {
                    if (first == 1) {
                        wmove(w[1], 1, 1);
                        first = 0;
                    }  
                    dispCounter++;
                    if (dispCounter == 28) {
                        wmove(w[1], 1, 1);
                        wclrtoeol(w[1]);
                        dispCounter = 0;
                    }    
                    wAddstr(1, buf);
                    wmove(w[5], 5, 2);
                } 
                }
                
                if ((activeRight == 1 && activeLeft == 0)) { 
                // prints stuff that is coming from other sockets
					 if (i != 0 && buf[0] != '\e') {
						if (first == 1) {
							wmove(w[4], 1, 1);
							first = 0;
						}  
						dispCounter++;
						if (dispCounter == 28) {
							wmove(w[4], 1, 1);
							wclrtoeol(w[4]);
							dispCounter = 0;
						}    
						wAddstr(4, buf);
						wmove(w[5], 5, 2);
					} 
                }
                
                
                
                if (activeRight == 1 && activeLeft == 1) {
                if (i == Leftsocket) { 
                    if (first2 == 1) {
                        wmove(w[1], 1, 1);
                        first2 = 0;
                    }    
                    dispCounter2++;
                    if (dispCounter2 == 28) {
                        wmove(w[1], 1, 1);
                        wclrtoeol(w[1]);
                        dispCounter2 = 0;
                    }    
                    wAddstr(1, buf);
                    wmove(w[5], 5, 2);
                     
                } 
                        

                if ((i == Rightsocket)) { 
                    if (first == 1) {
                        wmove(w[4], 1, 1);
                        first = 0;
                    }  
                    dispCounter++;
                    if (dispCounter == 28) {
                        wmove(w[4], 1, 1);
                        wclrtoeol(w[4]);
                        dispCounter = 0;
                    }    
                    wAddstr(4, buf);
                    wmove(w[5], 5, 2);
                } 
                }
                
                
                
                
                mylabel:
                //enter command mode, if not in it already
                if (i == 0 && buf[0] == '\e' && COMM == 0) {
                    wmove(w[5], 3, 2);
                    wclrtoeol(w[5]);
                    COMM = 1;
                    wAddstr(5,"entering command mode:   press  -  to send the command");
                    wmove(w[5], 5, 2);
                }
                
                //does commands if ready and in command mode
                if (COMM == 1 && i == 0 && READY == 1) {
                    
                    wmove(w[5], 4, 2);
                    wclrtoeol(w[5]);
                    if (!strncmp("outputl", transfer, 7)) {
                        wAddstr(5,"set the output direction to the left");
                        OUTPUTRL = 1;
                        OUTPUTLR = -1;
                    }
                    else if (buf[0] == '\e') {
                        //ignore, do nothing.
                    }
                    else if (!strncmp("i", transfer, 1)) {
                        wmove(w[5], 3, 2);
                        wclrtoeol(w[5]);
                        wAddstr(5,"enter insert mode");
                        COMM = 0;
                    }
                    else if (!strncmp("outputr", transfer, 7)) {
                        wAddstr(5,"set the output direction to right");
                        OUTPUTRL = -1;
                        OUTPUTLR = 1;
                    }
                    else if (!strncmp("output", transfer, 6)) {
                        wAddstr(5,"the output direction is...");
                        if (OUTPUTRL == 1) {
                            wAddstr(5,"RL");
                        }
                        if (OUTPUTLR == 1) {
                            wAddstr(5,"LR");
                        }    
                    }
                    else if (!strncmp("dsplr", transfer, 5)) {
                        wAddstr(5,"display data left to right");
                        DSPLR = 1;
                        DSPRL = -1;
                    }
                    else if (!strncmp("dsprl", transfer, 5)) {
                        wAddstr(5,"display data right to left");
                        DSPLR = -1;
                        DSPRL = 1;
                    }
                    else if (!strncmp("display", transfer, 7)) {
                        wAddstr(5,"the display direction is...");
                        if (DSPLR == 1) {
                            wAddstr(5,"LR");
                        }
                        if (DSPRL == 1) {
                            wAddstr(5,"RL");
                        }  
                    }
                    else if (!strncmp("dropr", transfer, 5)) {
                        wAddstr(5,"drop the right side connection - none to close, if this is the tail.");
                        if (activeRight == 1) {
                            close(Rightsocket);
                            FD_CLR(Rightsocket, &master);
                            activeRight = 0;
                            Rightsocket = 0;
                        } else {
                            wAddstr(5,"no active right connection");
                        }    
                    }
                    else if (!strncmp("dropl", transfer, 5)) {
                        wAddstr(5,"drop the left side connection - none to close, if this is the head.");
                        if (activeLeft == 1) {
                            close(Leftsocket);
                            FD_CLR(Leftsocket, &master);
                            activeLeft = 0;
                            Leftsocket = 0;
                        } else {
                            wAddstr(5,"no active left connection");
                        }    
                    }
                    
                    
                    else if (!strncmp("right", transfer, 5)) {
                       char portbuf[10];
                        char myportbuf[10];
                        
                        
                        //all this is used to compare LRADDR
                        socklen_t rightlen2;
                        struct sockaddr_storage rightaddr2;
                        char rightipstr2[INET6_ADDRSTRLEN];
                        int rightport2;

                        
                        rightlen2 = sizeof rightaddr2;
                        getpeername(Rightsocket, (struct sockaddr*)&rightaddr2, &rightlen2); 
        
                        // deal with both IPv4 and IPv6:
                        if (leftaddr.ss_family == AF_INET) {
                            struct sockaddr_in *s = (struct sockaddr_in *)&rightaddr2;
                            rightport2 = ntohs(s->sin_port);
                            inet_ntop(AF_INET, &s->sin_addr, rightipstr2, sizeof rightipstr2);
                        } else { // AF_INET6
                            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&rightaddr2;
                            rightport2 = ntohs(s->sin6_port);
                            inet_ntop(AF_INET6, &s->sin6_addr, rightipstr2, sizeof rightipstr2);
                        }
                        
                        
   
                        
                        
                        struct ifaddrs * ifAddrStruct=NULL;
                        struct ifaddrs * ifa=NULL;
                        void * tmpAddrPtr=NULL;

                        getifaddrs(&ifAddrStruct);

                        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
                            if (!ifa->ifa_addr) {
                                continue;
                            }
                            if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
                                // is a valid IP4 Address
                                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                                char addressBuffer[INET_ADDRSTRLEN];
                                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                                if (!strncmp("140.160.137.", addressBuffer, 12)) { 
                                    
                                    wAddstr(5,addressBuffer);
                                    
                                }    
                            }
                        }   
                            

                        wAddstr(5,":");
                        snprintf(myportbuf, sizeof myportbuf, "%d", RLPORT);
                        wAddstr(5,myportbuf);
                        wAddstr(5,":");
                        snprintf(portbuf, sizeof portbuf, "%d", rightport2);
                        if (activeRight == 1) {
                            wAddstr(5,rightipstr2);
                            wAddstr(5,":");
                            wAddstr(5,portbuf);
                            wAddstr(5, "CONNECTED");
                        } else {
                            wAddstr(5,"*:*");
                        } 
                            
                    }
                    
                    
                    
                    else if (!strncmp("left", transfer, 4)) {
                        char portbuf[10];
                        char myportbuf[10];
                        
                        
                        //all this is used to compare LRADDR
                        socklen_t leftlen2;
                        struct sockaddr_storage leftaddr2;
                        char leftipstr2[INET6_ADDRSTRLEN];
                        int leftport2;

                        
                        leftlen2 = sizeof leftaddr2;
                        getpeername(Leftsocket, (struct sockaddr*)&leftaddr2, &leftlen2); 
        
                        // deal with both IPv4 and IPv6:
                        if (leftaddr.ss_family == AF_INET) {
                            struct sockaddr_in *s = (struct sockaddr_in *)&leftaddr2;
                            leftport2 = ntohs(s->sin_port);
                            inet_ntop(AF_INET, &s->sin_addr, leftipstr2, sizeof leftipstr2);
                        } else { // AF_INET6
                            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&leftaddr2;
                            leftport2 = ntohs(s->sin6_port);
                            inet_ntop(AF_INET6, &s->sin6_addr, leftipstr2, sizeof leftipstr2);
                        }
                        
                        
                        snprintf(portbuf, sizeof portbuf, "%d", leftport2);
                        if (activeLeft == 1) {
                            wAddstr(5,leftipstr2);
                            wAddstr(5,":");
                            wAddstr(5,portbuf);
                        } else {
                            wAddstr(5,"*:*");
                        }    
                        
                        
                        struct ifaddrs * ifAddrStruct=NULL;
                        struct ifaddrs * ifa=NULL;
                        void * tmpAddrPtr=NULL;

                        getifaddrs(&ifAddrStruct);

                        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
                            if (!ifa->ifa_addr) {
                                continue;
                            }
                            if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
                                // is a valid IP4 Address
                                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                                char addressBuffer[INET_ADDRSTRLEN];
                                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                                if (!strncmp("140.160.137.", addressBuffer, 12)) { 
                                    wAddstr(5,":");
                                    wAddstr(5,addressBuffer);
                                }    
                            }
                        }   
                            

                            wAddstr(5,":");
                            snprintf(myportbuf, sizeof myportbuf, "%d", LLPORT);
                            wAddstr(5,myportbuf);
                            if (activeRight == 1) {
                                wAddstr(5, "CONNECTED");
                            }    
                    }
                    
                    
                    
                    else if (!strncmp("loopr", transfer, 5)) {
                        wAddstr(5,"take left incoming data and send it back - no effect on the head.\n");
                        LOOPR = 1;
                        LOOPL = -1;
                    }
                    else if (!strncmp("loopl", transfer, 5)) {
                        wAddstr(5,"take right incoming data and send it back - no effect on the tail.\n");
                        LOOPR = -1;
                        LOOPL = 1;
                    }
                    else if (!strncmp("q", transfer, 1)) {
                        endwin();
                        exit(1);
                    }
                    
                    
                    else if (!strncmp("reset", transfer, 5)) {
                        wAddstr(5,"reset program\n");
                        close(Leftsocket);
                        close(Rightsocket);
                        close(Leftlistensocket);
                        close(Rightlistensocket);
                        FD_ZERO(&read_fds);
                        FD_ZERO(&master);
                        memset(buf, '\0', sizeof(buf));
                        memset(transfer, '\0', sizeof(transfer));
                        

                        endwin();
                        firstGo = 1;
                        innerSafe = 0;
                    }
                    
                    
                        else if (!strncmp("read", transfer, 4)) {
                        wAddstr(5,"attempting to read");
                        //get the string of the file, read it, and write it out in the current output direction
                        if (strchr(transfer, ' ')) {
                            
                            char *fileName = strstr(transfer, " ")+1;
                                fileName[sizeof(fileName)] = '\0';
                                FILE *file = fopen(fileName, "r");
                                char *code;
                                size_t n = 0;
                                int c;
                                char acode[10];

                                if (file == NULL) {
                                    wAddstr(5,"file was NULL"); //could not open file
                                }
                                fseek(file, 0, SEEK_END);
                                long f_size = ftell(file);
                                fseek(file, 0, SEEK_SET);
                                code = malloc(f_size);
                                while ((c = fgetc(file)) != EOF) {
                                    int notprintable2 = 0;
                                    int cval = (char) c;
                                    char stripbuf2[10];
                                    char stripeolbuf2[10];
                                    
                                    code[n++] = (char) c;
                                    acode[0] = (char) c;
                                    acode[1] = '\0';
                                
                                    strncpy(stripbuf2, acode, 10);
                                    strncpy(stripeolbuf2, acode, 10);
                                    stripbuf2[1] = '\0';
                                    stripeolbuf2[1] = '\0';

                                    
                                     if ((cval < 32 || cval > 126) && (cval != 13 && cval != 10 && cval != 27)) {
                                        char hexBuffer[3];
                                        charToHex(cval, hexBuffer);
                                        wAddstr(5, hexBuffer);
                                        int t = 0;
                                        memmove(&stripeolbuf2[t], &stripeolbuf2[t + 1], strlen(stripeolbuf2) - t);
                                    }
                                    
                                    if ((cval < 32 || cval > 126) && (cval != 27)) {
                                        int t = 0;
                                        memmove(&stripbuf2[t], &stripbuf2[t + 1], strlen(stripbuf2) - t);
                                    } 
                                    
                                    
									
                                        if (OUTPUTRL == 1) {
                                            if (logrlpreON == 1) {
                                                fputs(acode, logrlpreFile);
                                            } 
                                            if (notprintabletoleft == 1) {
                                                if (extrlON == 1) {
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                        
                                                    write(pipeEnds1[1], stripbuf2, sizeof(stripbuf2));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL);
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        strncpy(stripbuf2, returningbuf, 1000);
                                                        stripbuf2[newRecievedSize] = '\0';
                                                    }    
                                                }                                                
                                                if (logrlpostON == 1) {
                                                    fputs(stripbuf2, logrlpostFile);
                                                } 
												
                                                wmove(w[3], 3, 3);
                                                wclrtoeol(w[3]);
                                                wAddstr(3, stripbuf2);
                                                write(Leftsocket, stripbuf2, strlen(stripbuf2));
                                            } else if (notprintabletolefteol == 1) {
                                                if (extrlON == 1) {
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                                    write(pipeEnds1[1], stripeolbuf2, sizeof(stripeolbuf2));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL);
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        strncpy(stripeolbuf2, returningbuf, 1000);
                                                        stripeolbuf2[newRecievedSize] = '\0';
                                                    }    
                                                }                                                
                                                if (logrlpostON == 1) {
                                                    fputs(stripeolbuf2, logrlpostFile);
                                                } 
                                                wmove(w[3], 3, 3);
                                                wclrtoeol(w[3]);
                                                wAddstr(3, stripeolbuf2);
                                                write(Leftsocket, stripeolbuf2, strlen(stripeolbuf2));     
                                            } else  {  
                                                if (extrlON == 1) {
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                                    write(pipeEnds1[1], acode, sizeof(acode));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL); 
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        
                                                        strncpy(acode, returningbuf, 1000);
                                                        acode[newRecievedSize] = '\0';
                                                    }    
                                                }                                                
                                                if (logrlpostON == 1) {
                                                    fputs(acode, logrlpostFile);
                                                }    
                                                wmove(w[3], 3, 3);
                                                wclrtoeol(w[3]);
                                                wAddstr(3, acode);
                                                write(Leftsocket, acode, strlen(acode));                         
                                            }       
                                            
                                        }
                                        if (OUTPUTLR == 1) {
                                            if (loglrpreON == 1) {
                                                fputs(acode, loglrpreFile);
                                            }
                                            if (notprintabletoright == 1) {
                                                if (extlrON == 1) {
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                                    write(pipeEnds1[1], stripbuf2, sizeof(stripbuf2));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL); 
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        strncpy(stripbuf2, returningbuf, 1000);
                                                        stripbuf2[newRecievedSize] = '\0';
                                                    }    
                                                }
                                          
                                                if (loglrpostON == 1) {
                                                    fputs(stripbuf2, loglrpostFile);
                                                }
                                                wmove(w[2], 3, 3);
                                                wclrtoeol(w[2]);
                                                wAddstr(2, stripbuf2);
                                                write(Rightsocket, stripbuf2, strlen(stripbuf2));
                                            } else if (notprintabletorighteol == 1) {
                                                if (extlrON == 1) {
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                                    write(pipeEnds1[1], stripeolbuf2, sizeof(stripeolbuf2));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL);
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        strncpy(stripeolbuf2, returningbuf, 1000);
                                                        stripeolbuf2[newRecievedSize] = '\0';
                                                    }    
                                                }                                                
                                                if (loglrpostON == 1) {
                                                    fputs(stripeolbuf2, loglrpostFile);
                                                }
                                                wmove(w[2], 3, 3);
                                                wclrtoeol(w[2]);
                                                wAddstr(2, stripeolbuf2);
                                                write(Rightsocket, stripeolbuf2, strlen(stripeolbuf2));       
                                            } else {
                                                if (extlrON == 1) {
                                                    wAddstr(4, "in");
                                                    char returningbuf[1000];
                                                    char errorbuf[10];
                                                    int pipeEnds1[2];
                                                    int pipeEnds2[2];
                                                    pipe(pipeEnds1);
                                                    pipe(pipeEnds2);   
                                                    write(pipeEnds1[1], acode, sizeof(acode));
                                                    close(pipeEnds1[1]);
                                                    pid_t returnValue = fork();
                                                    if (returnValue == 0) {
                                                        //Child process
                                                        dup2(pipeEnds1[0], STDIN_FILENO);
                                                        dup2(pipeEnds2[1], STDOUT_FILENO);
                                                        int ret = execl("exfilt", "pipeEnds1", NULL); 
                                                    } else if (returnValue < 0) {
                                                        wAddstr(5, "failed to fork");
                                                    } else {  
                                                        //parent
                                                        close(pipeEnds1[0]);
                                                        close(pipeEnds2[1]);
                                                        wait(NULL); //wait for child process to join with this parent
                                                        int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                                        close(pipeEnds2[0]);
                                                        strncpy(acode, returningbuf, 1000);
                                                        acode[newRecievedSize] = '\0';
                                                    }    
                                                }                                                
                                                if (loglrpostON == 1) {
                                                    fputs(acode, loglrpostFile);
                                                }
                                                wmove(w[2], 3, 3);
                                                wclrtoeol(w[2]);
                                                wAddstr(2, acode);
                                                write(Rightsocket, acode, strlen(acode));
                                            }  
                                        } 
                                    //}    
                                }

                                // don't forget to terminate with the null character
                                code[n] = '\0';  
                                
                            
                        } else {
                           wAddstr(5,"no file given");
                        }   
                    }
                    
                    
                    // HAVE SOME RIGHT ASSOCIATION VALUE TO USE WITH OUR LEFT/RIGHT COMMANDS.
                    else if (!strncmp("connectr", transfer, 8)) {
                        
                        if (activeRight == 0) {
                        wAddstr(5,"attempting to connect to new right socket.");
                        activeRight = 1;
                        char *newRRADDR;
                        int newRRPORT;
                        int try = 0;
                        char portbuf[10];
                        int wehaveport = 0;
                        size_t len = sizeof(transfer);
                        if (strchr(transfer, ' ')) {
                            const char *p1 = strstr(transfer, " ")+1;
                            if (strstr(p1, " ")) {
                                const char *p2 = strstr(p1, " ");
                                len = p2 - p1;
                                wehaveport = 1;
                            }
                            
                            newRRADDR = (char*)malloc(sizeof(char)*(len+1));
                            strncpy(newRRADDR, p1, len);
                            newRRADDR[len] = '\0';
                            wAddstr(5, newRRADDR);
                            try = 1;
                        } else {
                            wAddstr(5,"error, need an ip to connect to");
                            activeRight = 0;
                        } 
                        
                        if (wehaveport == 1) {
                            newRRPORT = atoi(strrchr(transfer, ' '));
                            snprintf(portbuf, sizeof portbuf, "%d", newRRPORT);
                            wAddstr(5, portbuf);
                        } else {
                            wAddstr(5, "error, no rrport given, will use DEFAULTPORT");
                            newRRPORT = DEFAULTPORT;
                        }    
                        
                        
                        if (try == 1) {
                        //Enters the needed fields, we need to connect this with a remote socket on the RIGHT, given an rrport and rraddr. CLIENT
                        rightPad.sin_port = htons((u_short)newRRPORT);            //converts to TCP/IP network byte order, which is big-endian.
                        rightPad.sin_family = AF_INET;                         /* set family to Internet */
    
                        /* Convert host name to equivalent IP address */
                        ptrh = gethostbyname(newRRADDR);
                        if ( ((char *)ptrh) == NULL ) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "invalid host:");  
                            wAddstr(5, newRRADDR);
                            activeRight = 0;
                        }
                        memcpy(&rightPad.sin_addr, ptrh->h_addr, ptrh->h_length);
                        /* Create a connecting socket. */
                        Rightsocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
                        if (Rightsocket < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "socket creation failed");
                            activeRight = 0;
                        }   
                        /* Connect the socket to the specified server. */
                        if (connect(Rightsocket, (struct sockaddr *)&rightPad, sizeof(rightPad)) < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "connect failed");
                            activeRight = 0;
                        } 
                        }
                        // add the incoming socket to the master set
                        FD_SET(Rightsocket, &master);
                        if (Leftsocket > Rightsocket) {
                            fdmax = Leftsocket;
                        } else {
                            fdmax = Rightsocket;
                        }
                        
                        } else {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "you already have a right");
                        }    
                    } 

                    // HAVE SOME RIGHT ASSOCIATION VALUE TO USE WITH OUR LEFT/RIGHT COMMANDS.
                    else if (!strncmp("connectl", transfer, 8)) {
                        
                        if (activeLeft == 0) {
                        wAddstr(5,"attempting to connect to new left socket.");
                        activeLeft = 1;
                        char *newLRADDR;
                        int newLRPORT;
                        int try = 0;
                        char portbuf[10];
                        int wehaveport = 0;
                        size_t len = sizeof(transfer);
                        if (strchr(transfer, ' ')) {
                            const char *p1 = strstr(transfer, " ")+1;
                            if (strstr(p1, " ")) {
                                const char *p2 = strstr(p1, " ");
                                len = p2 - p1;
                                wehaveport = 1;
                            }
                            
                            newLRADDR = (char*)malloc(sizeof(char)*(len+1));
                            strncpy(newLRADDR, p1, len);
                            newLRADDR[len] = '\0';
                            wAddstr(5, newLRADDR);
                            try = 1;
                        } else {
                            wAddstr(5,"error, need an ip to connect to");
                            activeLeft = 0;
                        } 
                        
                        if (wehaveport == 1) {
                            newLRPORT = atoi(strrchr(transfer, ' '));
                            snprintf(portbuf, sizeof portbuf, "%d", newLRPORT);
                            wAddstr(5, portbuf);
                        } else {
                            wAddstr(5, "error, no rrport given, will use DEFAULTPORT");
                            newLRPORT = DEFAULTPORT; 
                        }    
                        
                        
                        if (try == 1) {
                        //Enters the needed fields, we need to connect this with a remote socket on the RIGHT, given an rrport and rraddr. CLIENT
                        leftPad.sin_port = htons((u_short)newLRPORT);            //converts to TCP/IP network byte order, which is big-endian.
                        leftPad.sin_family = AF_INET;                         /* set family to Internet */
    
                        /* Convert host name to equivalent IP address */
                        ptrh = gethostbyname(newLRADDR);
                        if ( ((char *)ptrh) == NULL ) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "invalid host:");  
                            wAddstr(5, newLRADDR);
                            activeLeft = 0;
                        }
                        memcpy(&leftPad.sin_addr, ptrh->h_addr, ptrh->h_length);
                        /* Create a connecting socket. */
                        Leftsocket = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
                        if (Leftsocket < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "socket creation failed");
                            activeLeft = 0;
                        }   
                        /* Connect the socket to the specified server. */
                        if (connect(Leftsocket, (struct sockaddr *)&leftPad, sizeof(leftPad)) < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "connect failed");
                            activeLeft = 0;
                        } 
                        }
                        // add the incoming socket to the master set
                        FD_SET(Leftsocket, &master);
                        if (Leftsocket > Rightsocket) {
                            fdmax = Leftsocket;
                        } else {
                            fdmax = Rightsocket;
                        }
                        
                        } else {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "you already have a left\n");
                        }   
                    }

                    else if (!strncmp("llport", transfer, 6)) {
                        wAddstr(5,"changing the llport");
                        if (activeLeft == 0) {
                            int newLLPORT;
                            char llportbuf[10];
                            size_t len = sizeof(transfer);
                            
                            if (strchr(transfer, ' ')) {
                                newLLPORT = atoi(strchr(transfer, ' '));
                                LLPORT = atoi(strchr(transfer, ' '));
                                snprintf(llportbuf, sizeof llportbuf, "%d", newLLPORT);
                                wAddstr(5, llportbuf);
                            } else {
                                wAddstr(5,"error, didn't give value");
                            }
                        } else {
                            wAddstr(5,"remove the left connection first");
                        }
                    }  
                    
                    else if (!strncmp("rlport", transfer, 6)) {
                        wAddstr(5,"changing the rlport");
                        if (activeRight == 0) {
                            int newRLPORT;
                            char rlportbuf[10];
                            size_t len = sizeof(transfer);
                            
                            if (strchr(transfer, ' ')) {
                                newRLPORT = atoi(strchr(transfer, ' '));
                                snprintf(rlportbuf, sizeof rlportbuf, "%d", newRLPORT);
                                wAddstr(5, rlportbuf);
                            } else {
                                wAddstr(5,"error, didn't give value");
                            }
                        } else {
                            wAddstr(5,"remove the right connection first");
                        }
                    } 
                    
                    else if (!strncmp("lraddr", transfer, 6)) {
                        wAddstr(5,"adding restrictions on left socket, for listens");
                        size_t len = sizeof(transfer);
                            
                        if (strchr(transfer, ' ')) {
                            char* temp = strchr(transfer, ' ');
                            lraddrSize = (strlen(temp)) + 1;
                            leftCheckAddr = 1;
                            snprintf(compareLRADDR,lraddrSize, "%s", temp);
                            wAddstr(5,compareLRADDR);  
                        } else {
                            wAddstr(5,"error, didn't give value");  
                        }    

                    }
                    else if (!strncmp("rraddr", transfer, 6)) {
                        wAddstr(5,"adding restrictions on right socket, for listens");
                        size_t len = sizeof(transfer);
                        
                        if (strchr(transfer, ' ')) {
                            char* temp = strchr(transfer, ' ');
                            rraddrSize = (strlen(temp)) + 1;
                            rightCheckAddr = 1;
                            snprintf(compareRRADDR,rraddrSize, "%s", temp);
                            wAddstr(5, compareRRADDR);
                        } else {
                            wAddstr(5,"error, didn't give value");  
                        }    
                    }                    
                    
                    else if(!strncmp("listenl", transfer, 8)) {
                        
                        if (activeLeft == 0) {
                           
                            
                        int newLLPORT;
                        char llportbuf[10];
                        size_t len = sizeof(transfer);
                            
                        if (strchr(transfer, ' ')) {
                            newLLPORT = atoi(strchr(transfer, ' '));
                            snprintf(llportbuf, sizeof llportbuf, "%d", newLLPORT);
                            wAddstr(5, llportbuf);
                            //THIS IS WHERE WE'D REBIND OUR LEFT LISTENING SOCKET
                        } else {
                            wAddstr(5,"no port given, using default 36761");
                        }
                        
                        
                        leftListenActive = 0; 
                        
                        // Specify size of request queue
                        if (listen(Leftlistensocket, QLEN) < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "listen failed");
                            //exit(1);
                        }
                       // add the listener to the master set
                         FD_SET(Leftlistensocket, &master);
                        
                        if (Leftlistensocket > fdmax) {
                            fdmax = Leftlistensocket;
                        }                        
                        
                        } else {
                            wAddstr(5, "we already have a left connection");
                        }    
                    }
                    
                    
                    else if(!strncmp("listenr", transfer, 8)) {
                        
                        if (activeRight == 0) {
                            
                        int newRLPORT;
                        char rlportbuf[10];
                        size_t len = sizeof(transfer);
                            
                        if (strchr(transfer, ' ')) {
                            newRLPORT = atoi(strchr(transfer, ' '));
                            snprintf(rlportbuf, sizeof rlportbuf, "%d", newRLPORT);
                            wAddstr(5, rlportbuf);
                            //THIS IS WHERE WE'D REBIND OUR RIGHT LISTENING SOCKET
                        } else {
                            wAddstr(5,"no port given, using default 46761");
                        }
                        
                        
                        // Specify size of request queue
                        if (listen(Rightlistensocket, QLEN) < 0) {
                            wmove(w[5], 2, 2);    
                            wAddstr(5, "listen failed");
                            //exit(1);
                        }
                                             
                        FD_SET(Rightlistensocket, &master);
                        
                        if (Rightlistensocket > fdmax) {
                            fdmax = Rightlistensocket;
                        } 
                        
                        
                        } else {
                            wAddstr(5, "we already have a right connection");
                        }    
                    }
                    
                    else if (!strncmp("stlrnp", transfer, 6)) {
                        wAddstr(5,"attempting to set internal filter left to right");
                        if (strchr(transfer, ' ')) {
                            char *direction = strchr(transfer, ' ');
                            if (!strncmp("on", direction+1, 2)) {
                                notprintabletoright = 1;
                                notprintabletorighteol = 0;
                                wAddstr(5, "on");

                            } else if (!strncmp("off", direction+1, 3)) {
                                wAddstr(5, "off");
                                notprintabletoright = 0;
                            } else {
                               wAddstr(5,"wrong direction given");
                            }   
                                
                        } else {
                            wAddstr(5,"no direction given");
                        }                    
                    }
                    
                    else if (!strncmp("strlnp", transfer, 6)) {
                        wAddstr(5,"attempting to set internal filter right to left");
                        if (strchr(transfer, ' ')) {
                            char *direction = strchr(transfer, ' ');
                            if (!strncmp("on", direction+1, 2)) {
                                notprintabletoleft = 1;
                                notprintabletolefteol = 0;
                                wAddstr(5, "on");

                            } else if (!strncmp("off", direction+1, 3)) {
                                wAddstr(5, "off");
                                notprintabletoleft = 0;
                            } else {
                               wAddstr(5,"wrong direction given");
                            }   
  
                        } else {
                            wAddstr(5,"no direction given");
                        }                    
                    }
                    
                    else if (!strncmp("stlrnpxeol", transfer, 10)) {
                        wAddstr(5,"attempting to set internal filter left to right keep eol");
                        if (strchr(transfer, ' ')) {
                            char *direction = strchr(transfer, ' ');
                            if (!strncmp("on", direction+1, 2)) {
                                notprintabletorighteol = 1;
                                notprintabletoright = 0;
                                wAddstr(5, "on");

                            } else if (!strncmp("off", direction+1, 3)) {
                                wAddstr(5, "off");
                                notprintabletorighteol = 0;
                            } else {
                               wAddstr(5,"wrong direction given");
                            }   
                                
                        } else {
                            wAddstr(5,"no direction given");
                        }                    
                    }
                    
                    else if (!strncmp("strlnpxeol", transfer, 10)) {
                        wAddstr(5,"attempting to set internal filter right to left keep eol");
                        if (strchr(transfer, ' ')) {
                            char *direction = strchr(transfer, ' ');
                            if (!strncmp("on", direction+1, 2)) {
                                notprintabletoleft = 0;
                                notprintabletolefteol = 1;
                                wAddstr(5, "on");

                            } else if (!strncmp("off", direction+1, 3)) {
                                wAddstr(5, "off");
                                notprintabletolefteol = 0;
                            } else {
                               wAddstr(5,"wrong direction given");
                            }   
   
                        } else {
                            wAddstr(5,"no direction given");
                        }                    
                    }                    
                    
                    else if (!strncmp("loglrpre ", transfer, 9)) {
                        wAddstr(5,"attempting to log left to right before filter");
                        if (strchr(transfer, ' ')) {

                            char *filename = strchr(transfer, ' ') + 1;
                            loglrpreON = 1;

                            if (loglrpreFile != NULL) {
                                fclose(loglrpreFile);
                            }    
                            filename[sizeof(filename)] = '\0';
                            loglrpreFile = fopen(filename, "a+");
                        } else {
                            wAddstr(5,"no filename given");
                        }                    
                    }   
                    
                    else if (!strncmp("loglrpreoff", transfer, 11)) {
                        wAddstr(5,"turing off log left to right before filter");
                            loglrpreON = 0;                   
                    }  
                    
                    else if (!strncmp("logrlpre ", transfer, 9)) {
                        wAddstr(5,"attempting to log right to left before filter");
                        if (strchr(transfer, ' ')) {
                            char *filename = strchr(transfer, ' ') + 1;
                            logrlpreON = 1;
 
                            if (logrlpreFile != NULL) {
                                fclose(logrlpreFile);
                            }    
                            filename[sizeof(filename)] = '\0';
                            logrlpreFile = fopen(filename, "a+");                            
                        } else {
                            wAddstr(5,"no filename given");
                        }                    
                    }   
                    
                    else if (!strncmp("logrlpreoff", transfer, 11)) {
                        wAddstr(5,"turing off log right to left before filter");
                            loglrpreON = 0;                   
                    } 
                   
                    else if (!strncmp("logrlpost ", transfer, 10)) {
                        wAddstr(5,"attempting to log right to left after filter");
                        if (strchr(transfer, ' ')) {
                            char *filename = strchr(transfer, ' ') + 1;

                            logrlpostON = 1;
                            if (logrlpostFile != NULL) {
                                fclose(logrlpostFile);
                            }    
                            filename[sizeof(filename)] = '\0';
                            logrlpostFile = fopen(filename, "a+");   
                        } else {
                            wAddstr(5,"no filename given");
                        }                    
                    }   
                    
                    else if (!strncmp("logrlpostoff", transfer, 12)) {
                        wAddstr(5,"turing off log right to left after filter");
                            logrlpostON = 0;                   
                    }    
                    
                    else if (!strncmp("loglrpost ", transfer, 10)) {
                        wAddstr(5,"attempting to log left to right after filter");
                        if (strchr(transfer, ' ')) {
                            char *filename = strchr(transfer, ' ') + 1;
                            loglrpostON = 1;   
                            if (loglrpostFile != NULL) {
                                fclose(loglrpostFile);
                            }    
                            filename[sizeof(filename)] = '\0';
                            loglrpostFile = fopen(filename, "a+");
                        } else {
                            wAddstr(5,"no filename given");
                        }                    
                    }   
                    
                    else if (!strncmp("loglrpostoff", transfer, 12)) {
                        wAddstr(5,"turing off log left to right after filter");
                            loglrpostON = 0;                   
                    } 
                    
                    else if (!strncmp("extlr ", transfer, 6)) {
                        wAddstr(5,"executing external filter program, used for left to right");
                        if (strchr(transfer, ' ')) {
                            char *command = strchr(transfer, ' ') + 1;
                            extlrON = 1;    
                            command[sizeof(command)] = '\0';
                        } else {
                            wAddstr(5,"no extra commands given");
                        }                    
                    }   
                    
                    else if (!strncmp("extlroff", transfer, 8)) {
                        wAddstr(5,"turing off external filter, used for left to right");
                            extlrON = 0;                   
                    }   
                    
                    else if (!strncmp("extrl ", transfer, 6)) {
                        wAddstr(5,"executing external filter program, used for right to left");
                        if (strchr(transfer, ' ')) {
                            char *command = strchr(transfer, ' ') + 1;
                            extrlON = 1;    
                            command[sizeof(command)] = '\0';
                        } else {
                            wAddstr(5,"no extra commands given");
                        }                    
                    }   
                    
                    else if (!strncmp("extrloff", transfer, 8)) {
                        wAddstr(5,"turing off external filter, used for right to left");
                            extrlON = 0;                   
                    }                    
                    
                    else if (!strncmp("source", transfer, 6)) {
                        wAddstr(5,"attempting to read script");
                        if (strchr(transfer, ' ')) {
                            scriptFileName2 = strchr(transfer, ' ');
                            scriptFileNameDescriptor2 = open(scriptFileName2 + 1, O_RDONLY);
                            read(scriptFileNameDescriptor2, scriptFileNamebuf2, 1000);
                            memcpy(transfer, scriptFileNamebuf2, 1000);
                            memset(scriptFileName2, '\0', sizeof(scriptFileName2));
                            goto mylabel;

                            
                        } else {
                            wAddstr(5,"use default script name of scriptin");
                            char *filename = "scriptin";
                        }                    
                    }                     
                    
                    else {

                        wAddstr(5,"none of the commands matched up");
                    }

                    wmove(w[5], 5, 2);
                    wclrtoeol(w[5]);
                    READY = 0;
                    memset(transfer, '\0', sizeof(transfer));
                }
                
                
                
                
                
                if (activeRight == 0 && activeLeft == 1) {
                //ONLY DO WRITE STUFF IF COMM == 0    
                //handles the loop back case and giving standard in and passing it on if we can.
                if (i == Leftsocket && LOOPR == 1) {
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    }    
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletolefteol == 1) { 
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));     
                    } else  {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));                         
                    }
                } 
                // the tail should always be writing in this direction, by default.
                if ( i == 0 && OUTPUTRL == 1 && COMM == 0) {
                    
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    } 
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                        
                    } else if (notprintabletolefteol == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }                         
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));
                        
                    } else {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }  
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        }                        
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));  
                    }
                }
                }
                
                
                
                
               if (activeRight == 1 && activeLeft == 1) {
                //handles shortcircuit loop/incoming from right, to left.
                if (i == Rightsocket && LOOPL == 1 && notprintable == 0) {
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }                     
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    } 
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));                        
                   }else {
                       if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                   }   
                    
                    
                    
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletolefteol == 1) {  
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }                        
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));     
                    } else  {  
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));                         
                    }
                    wmove(w[5], 5, 2);
                }
                
                //handles transfer loop/incoming from right, to left.
                else if (i == Rightsocket && LOOPL == -1) {
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    } 
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletolefteol == 1) {  
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));     
                    } else  {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2); 
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }                         
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));                         
                    }
                }  
                //if output is l to r
                else if (i == 0 && OUTPUTLR == 1 && COMM == 0) {
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(stripbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));       
                    } else {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                    }
                    wmove(w[5], 5, 2);   
                }
                //if output is r to l
                else if (i == 0 && OUTPUTRL == 1 && COMM == 0) { 
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    } 
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletolefteol == 1) { 
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));     
                    } else  {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));                         
                    }
                    wmove(w[5], 5, 2);
                }
                
                //handles shortcircuit loop/incoming from left, to right.
                else if (i == Leftsocket && LOOPR == 1) {
                    if (logrlpreON == 1) {
                        fputs(buf, logrlpreFile);
                    }
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }                     
                    if (notprintabletoleft == 1) {
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (logrlpostON == 1) {
                            fputs(stripbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripbuf);
                        write(Leftsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletolefteol == 1) { 
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(stripeolbuf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, stripeolbuf);
                        write(Leftsocket, stripeolbuf, strlen(stripeolbuf));     
                    } else  {  
                        if (extrlON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (logrlpostON == 1) {
                            fputs(buf, logrlpostFile);
                        } 
                        wmove(w[3], 3, 3);
                        wclrtoeol(w[3]);
                        wAddstr(3, buf);
                        write(Leftsocket, buf, strlen(buf));                         
                    }
                    
                    
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(stripbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));       
                    } else {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                    }
                    wmove(w[5], 5, 2);  
                }
                //handles transfer loop/incoming from left, to right.
                else if (i == Leftsocket && LOOPR == -1) {
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(stripbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));       
                    } else {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                    }
                    wmove(w[5], 5, 2);  
                }                
                }

                

                
                    
                if (activeRight == 1 && activeLeft == 0) {
                //handles the loop back case and giving standard in and passing it on if we can.
                if (i == Rightsocket && LOOPL == 1) {
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(stripbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));       
                    } else {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                    }
                    //wmove(w[5], 5, 2);  
                } 
                if ( i == 0 && OUTPUTLR == 1 && COMM == 0) {
                    if (loglrpreON == 1) {
                        fputs(buf, loglrpreFile);
                    }
                    if (notprintabletoright == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripbuf, sizeof(stripbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripbuf, returningbuf, 1000);
                                stripbuf[newRecievedSize] = '\0';
                            }    
                        } 
                        if (loglrpostON == 1) {
                            fputs(stripbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripbuf);
                        write(Rightsocket, stripbuf, strlen(stripbuf));
                    } else if (notprintabletorighteol == 1) {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], stripeolbuf, sizeof(stripeolbuf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL);
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(stripeolbuf, returningbuf, 1000);
                                stripeolbuf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(stripeolbuf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, stripeolbuf);
                        write(Rightsocket, stripeolbuf, strlen(stripeolbuf));       
                    } else {
                        if (extlrON == 1) {
                            char returningbuf[1000];
                            char errorbuf[10];
                            int pipeEnds1[2];
                            int pipeEnds2[2];
                            pipe(pipeEnds1);
                            pipe(pipeEnds2);   
                            write(pipeEnds1[1], buf, sizeof(buf));
                            close(pipeEnds1[1]);
                            pid_t returnValue = fork();
                            if (returnValue == 0) {
                                //Child process
                                dup2(pipeEnds1[0], STDIN_FILENO);
                                dup2(pipeEnds2[1], STDOUT_FILENO);
                                int ret = execl("exfilt", "pipeEnds1", NULL); 
                            } else if (returnValue < 0) {
                                wAddstr(5, "failed to fork");
                            } else {  
                                //parent
                                close(pipeEnds1[0]);
                                close(pipeEnds2[1]);
                                wait(NULL); //wait for child process to join with this parent
                                int newRecievedSize = read(pipeEnds2[0], returningbuf, sizeof(returningbuf));
                                close(pipeEnds2[0]);
                                strncpy(buf, returningbuf, 1000);
                                buf[newRecievedSize] = '\0';
                            }    
                        }
                        if (loglrpostON == 1) {
                            fputs(buf, loglrpostFile);
                        }
                        wmove(w[2], 3, 3);
                        wclrtoeol(w[2]);
                        wAddstr(2, buf);
                        write(Rightsocket, buf, strlen(buf));
                    }
                }                
                }               
            }
        }
    }   
        
} while (outterSafe == 1);
}  