/* Piper2 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
void sys_err(char *s){
printf("%s\n",s);
exit(1);
}
int main(int argc, char *argv[]) {
char incomingbuf[1000];
int incomingDescriptorVal;

//while (1) {
int recievedSize = read(STDIN_FILENO, incomingbuf, sizeof(incomingbuf)-1);
incomingbuf[recievedSize] = '\0';
int len = strlen(incomingbuf);

close(STDIN_FILENO);
//printf ("buf recived: %s",incomingbuf);
//}
                    /*for (int t = 0; t <= strlen(incomingbuf); t++) {
                        int comp = incomingbuf[t];
                        if ((comp < 32 || comp > 125) && (comp != 13 && comp != 10)) {
                            memmove(&incomingbuf[t], &incomingbuf[t + 1], strlen(incomingbuf) - t);                     
                        }
                    }*/
                    
                    for (int t = 0; t < recievedSize ; t++) {
                        int comp = incomingbuf[t];
                        if ((comp < 32 || comp > 125) && (comp != 13 && comp != 10)) {
                            memmove(&incomingbuf[t], &incomingbuf[t + 1], sizeof(incomingbuf) - t);                     
                            t--;
                            recievedSize--;
                            
                        }

                    }
                    /*int x = 0;
                    int y = 0;
                    while( i < 1000) {
                        incomingbuf
                    */    
//write(STDOUT_FILENO, incomingbuf, strlen(incomingbuf)+1);
write(STDOUT_FILENO, incomingbuf, sizeof(incomingbuf));
                
                close(STDOUT_FILENO);
return 0;
}
