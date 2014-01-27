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
  //print out the prompt         
    prompt();

    //read input into a string
    char * input = readinput();

    //parse input. also executes
    parseinput(input);
  }

  return 0;
}

//Prints out the required prompt:
//[user]@[host]:[dir]>[space]
void prompt()
 {
  //get pwd
  char *pwd = getcwd(NULL, 0);

  //Hostname
  char *host;
  size_t host_max = sysconf(_SC_HOST_NAME_MAX); //get the max length of the hostname
  hostname = calloc(hostname_max, sizeof(char)) //allocate memory
  gethostname(hostname, hostname_max);


  //User name
  char *user;
  size_t user_max = sysconf(_SC_LOGIN_NAME_MAX); //max length of the user
  login = calloc(login_max, sizeof(char)) //allocate memory
  struct passwd *pw = getpwuid(getuid());
  login = pw->pw_name;

  // Print the prompt.
  printf("%s@%s:%s> ", user, host, pwd);
 }

char * readinput() {
  char curr;
  int eof = 0;
  while(1) {
    curr = getc(stdin);
    if (curr == '\n') || (curr != EOF) {
      eof =1;
      break;
    }

  }
}

// Function which exits, printing the necessary message
void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}