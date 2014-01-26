/**
 * CS3600, Spring 2013
 * Project 1 Starter Code
 * (c) 2013 Alan Mislove
 *
 * You should use this (very simple) starter code as a basis for 
 * building your shell.  Please see the project handout for more
 * details.
 */

#include "3600sh.h"

#define USE(x) (x) = (x)

int main(int argc, char*argv[]) {
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
  USE(argc);
  USE(argv);
  setvbuf(stdout, NULL, _IONBF, 0); 
  
  // Main loop that reads a command and executes it
  while (1) {         
    // You should issue the prompt here
      
    // You should read in the command and execute it here
    printf("%% "); 
        fflush(stdout); /* flush from output buffer to terminal itself */
    getargs(cmd, &childargc, childargv); /* childargc and childargv are
            output args; on input they have garbage, but getargs sets them. */
        /* Check first for built-in commands. */
    if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 ) {
            exit(0);
        }
    if ( childargc > 0 && strcmp(childargv[0], "logout") == 0 ) {
            exit(0);
         }
    if (childargc > 1 && strcmp(childargv[1], "|") == 0 ) {
             myPipe(childargv[0], childargv[2]);
        }
    if (childargc > 1 && strcmp(childargv[1], ">") == 0 ) {
             myOut(childargv[0], childargv[2]);
        }
    else {
        execute(childargc, childargv);
    }
    // You should probably remove this; right now, it
    // just exits
    do_exit();
  }

  return 0;
}

// Function which exits, printing the necessary message
//
void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}
