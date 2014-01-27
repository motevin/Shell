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
#define MAXLINE 200 
#define MAXARGS 20
void prompt();

 /* In C, "static" means not visible outside of file.  This is different
 * from the usage of "static" in Java.
 * Note that end_ptr is an output parameter.
 */
static char * getword(char * begin, char **end_ptr) {
    char * end = begin;

    while ( *begin == ' ' )
        begin++;  /* Get rid of leading spaces. */
    end = begin;
    while ( *end != '\0' && *end != '\n' && *end != ' ' )
        end++;  /* Keep going. */
    if ( end == begin )
        return NULL;  /* if no more words, return NULL */
    *end = '\0';  /* else put string terminator at end of this word. */
    *end_ptr = end;
    if (begin[0] == '$') { /* if this is a variable to be expanded */
        begin = getenv(begin+1); /* begin+1, to skip past '$' */
    if (begin == NULL) {
        perror("getenv");
        begin = "UNDEFINED";
        }
    }
    return begin; /* This word is now a null-terminated string.  return it. */
}

static void execute(char *argv[])
{
    pid_t childpid; /* child process ID */

    childpid = fork();
    if (childpid == -1) { /* in parent (returned error) */
        perror("fork"); /* perror => print error string of last system call */
        printf("  (failed to execute command)\n");
    }
    if (childpid == 0) { /* child:  in child, childpid was set to 0 */
        /* Executes command in argv[0];  It searches for that file in
     *  the directories specified by the ,.environment variable PATH.
         */
         if (-1 == execvp(argv[0], argv)) {
          printf("Error: Command not found.\n");
        }
    /* NOT REACHED unless error occurred */
        exit(1);
    } else /* parent:  in parent, childpid was set to pid of child process */
        waitpid(childpid, NULL, 0);  /* wait until child process finishes */
    return;
}

/* In C, "int" is used instead of "bool", and "0" means false, any
 * non-zero number (traditionally "1") means true.
 */
/* argc is _count_ of args (*argcp == argc); argv is array of arg _values_*/
static void getargs(char cmd[], int *argcp, char *argv[])
{
    char *cmdp = cmd;
    char *end;
    int i = 0;

    /* fgets creates null-terminated string. stdin is pre-defined C constant
     *   for standard intput.  feof(stdin) tests for file:end-of-file.
     */
    if (fgets(cmd, MAXLINE, stdin) == NULL && feof(stdin)) {
        do_exit();
    }
    while ( (cmdp = getword(cmdp, &end)) != NULL ) { /* end is output param */
        /* getword converts word into null-terminated string */
        if (*cmdp == '#') {
            break;
  }
  argv[i++] = cmdp;
  cmdp = end + 1;
       if (strcmp(cmdp, "<") == 0) {
    cmdp = end + 1;
    cmdp = getword(cmdp, &end);
    freopen(cmdp, "r", stdin);
    }
  }
    argv[i] = NULL; /* Create additional null word at end for safety. */
    *argcp = i;
}


void interrupt_handler(int signum) {
  printf("<Control-C>\n");
  signum = 0;
  if (signum == 0) {
    return;
  }
}

void myOut(char* arg1, char* arg2) {
  fpos_t pos;
  int fd;
  fflush(stdout);
       fgetpos(stdout, &pos);
       fd = dup(fileno(stdout));
       freopen(arg2, "w", stdout);
  char* argv[] = {arg1, NULL};
      
    pid_t childpid; /* child process ID */

    childpid = fork();
    if (childpid == -1) { /* in parent (returned error) */
        perror("fork"); /* perror => print error string of last system call */
        printf("  (failed to execute command)\n");
    }
    if (childpid == 0) { /* child:  in child, childpid was set to 0 */
        /* Executes command in argv[0];  It searches for that file in
     *  the directories specified by the environment variable PATH.
         */
         if (-1 == execvp(argv[0], argv)) {
          perror("execvp");
          printf("  (couldn't find command)\n");
        }
    /* NOT REACHED unless error occurred */
        exit(1);
    } else /* parent:  in parent, childpid was set to pid of child process */
        waitpid(childpid, NULL, 0);  /* wait until child process finishes */
    fflush(stdout);
    dup2(fd, fileno(stdout));
    close(fd);
    clearerr(stdout);
    fsetpos(stdout, &pos);
  
    return;
}

int main(int argc, char *argv[]) {
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
  USE(argc);
  USE(argv);
  setvbuf(stdout, NULL, _IONBF, 0); 
  char cmd[MAXLINE];
  char *childargv[MAXARGS];
  int childargc;
  signal(SIGINT, interrupt_handler); 
  if (argc > 1)
  freopen(argv[1], "r", stdin);
    while (1) {
        if (feof(stdin)) {
          do_exit();
        }
        prompt();
         
        fflush(stdout); /* flush from output buffer to terminal itself */
    getargs(cmd, &childargc, childargv); /* childargc and childargv are
            output args; on input they have garbage, but getargs sets them. */
        /* Check first for built-in commands. */
    if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 ) {
            do_exit();
        }
    if (childargc > 1 && strcmp(childargv[1], "|") == 0 ) {
             //myPipe(childargv[0], childargv[2]);
        }
    if (childargc > 1 && strcmp(childargv[1], ">") == 0 ) {
             myOut(childargv[0], childargv[2]);
        }
    else {
        execute(childargv);
    }
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
  host = calloc(host_max, sizeof(char)); //allocate memory
  gethostname(host, host_max);


  //User name
  char *user;
  size_t user_max = sysconf(_SC_LOGIN_NAME_MAX); //max length of the user
  user = calloc(user_max, sizeof(char)); //allocate memory
  struct passwd *pw = getpwuid(getuid());
  user = pw->pw_name;

  // Print the prompt.
  printf("%s@%s:%s> ", user, host, pwd);
 }

// Function which exits, printing the necessary message
//
void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}
