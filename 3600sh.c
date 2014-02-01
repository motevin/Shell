  /**
  * CS3600, Spring 2014
  * Project 1 Starter Code
  * (c) 2013 Alan Mislove
  *
  * You should use this (very simple) starter code as a basis for 
  * building your shell.  Please see the project handout for more
  * details.
  */

  #include "3600sh.h"

  #define USE(x) (x) = (x)
  typedef char bool;
  #define TRUE 1
  #define FALSE 0

  typedef int fd;
  //set integers for the three file descriptors
  #define STDIN 0
  #define STDOUT 1
  #define STDERR 2

  //defining all of these to different numbers
  //because we have to check equality of them later
  #define INVALID_SYNTAX 1
  #define INVALID_ESCAPE 2
  #define BACKGROUND 4
  #define EOF_FOUND 8
  #define REDIR_STDOUT 16
  #define REDIR_STDIN 32
  #define REDIR_STDERR 64
  #define CMD_WORD_CHUNK 10 /* number by which we increment space allocated for 'word' of input */

  //define max string lengths using same convention as strlen
  //e.g. memory allocated = length + 1
  #define MAX_DIR_LENGTH 100
  #define MAX_CMD_LENGTH 100
  #define MAX_HOSTNAME_LENGTH 100
  #define MAX_INPUT_LENGTH 100
  void prompt();
  void interrupt_handler(int signum);
  void deleteArgs(char **args);
  void background_handler(char* error,char* next);
  char** readArgs(char* error, char** file);
  void memoryError();
  fd addWord(char* word, char*** arguments, int* argCount, int* argSteps, char* error, fd redirect, char** file);




void getargs() {
/* allocate files used for redirection of stderr, stdin, stdout */
char **file = calloc(3, sizeof(char*));
   if (!file) {
   memoryError();
   }

for (int i = 0; i < 3; i++)
{
file[i] = calloc(MAX_INPUT_LENGTH + 1, sizeof(char));
if (!file[i]) {
memoryError();
}
}

/* initialize status to designate different modes as identified in #defines */
char parseStatus = 0;

char** args = readArgs(&parseStatus, file);

/* determine whether shell should exit after executing command */
bool quit = EOF_FOUND & parseStatus;
//Bit masking is used to use a single value for status.
switch(parseStatus & (INVALID_SYNTAX | INVALID_ESCAPE)){
case INVALID_SYNTAX:
printf("Error: Invalid syntax.\n");
if (quit)
do_exit();  
return;
case INVALID_ESCAPE:
printf("Error: Unrecognized escape sequence.\n");
if (quit)
do_exit();  
return;
}

/* if "exit" is inputted, quit the shell */
if (args[0]) {
if (args[1] == NULL && strncmp(args[0], "exit", strlen("exit")) == 0) {
do_exit();
}
}

int restore_stdout = 0;
int restore_stdin = 0;
int restore_stderr = 0;
  int f = -1;

/* check if any redirection was requested and set up proper files */
if (parseStatus & REDIR_STDOUT)
{
/*open afile with read and write permissions for the user. */
/*Create it if needed, and truncate it if opened*/
f = open(file[STDOUT],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
if (f == -1) //-1 indicates that there was a problem with the file
{
printf("Error: Invalid syntax.\n");
if (quit) {//if it is EOF, quit
do_exit();
}
return; //otherwise, print another prompt
}
/*Open a file with the path "file" and the intention to read and write from it. */
/*If it does not exist create it and give read and write permissions to the user*/
         restore_stdout = dup(STDOUT); //keep a copy of STDOUT
         close(STDOUT); //Close STDOUT
dup(f); //Copy the same file descriptor but set it to the newly closed STDOUT
close(f); //Close original file
}
if (parseStatus & REDIR_STDERR)
{
f = open(file[STDERR],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
if (f == -1)
{
printf("Error: Invalid syntax.\n");
if (quit) {
do_exit();
}
return;
}
         restore_stderr = dup(STDERR); //keep a copy of STDOUT
         close(STDERR); //Close STDOUT
dup(f); //Copy the same file descriptor but set it to the newly closed STDOUT
close(f); //Close original file
}
if (parseStatus & REDIR_STDIN)//type==(2<<STDIN))
{
/*Do not create a file, as it would be empty and useless to STDIN*/
/*Open as read only because it is an input source*/
f = open(file[STDIN],O_RDONLY);
if (f == -1)
{
printf("Error: Unable to open redirection file.\n");
if (quit) {
do_exit();
}
return;
}
         restore_stdin = dup(STDIN);
         close(STDIN);
dup(f);
close(f);
}
  
  pid_t parent = fork();
  
  if (parent < 0) {
    printf("Error: process creation failed");
    do_exit();
  }

  /* if we are in the child process, execute the command */
  else if (!parent) {
    if (execvp(args[0], args)) {
     if((*args)[0]) {
          printf("Error: ");
          switch(errno) { //based on values in `man errno`
            case 1: printf("Permission denied.\n"); break;
            case 2: printf("Command not found.\n"); break;
case 13: printf("Permission denied.\n"); break;
            default: printf("Unkown error.\n"); break;
     }
      }
    }
    exit(0);
  } else {
/* in parent, wait for child if not a background process */
if (!(parseStatus & BACKGROUND)) {
     waitpid(parent, NULL, 0);
}
     if (quit) {
       do_exit();
    }
  }

  /* restore any file redirections as necessary */
  if(restore_stdin)
  {
   close(STDIN);
   dup(restore_stdin);
   close(restore_stdin);
  }
  if(restore_stdout)
  {
   close(STDOUT);
   dup(restore_stdout);
   close(restore_stdout);
  }
  if(restore_stderr)
  {
   close(STDERR);
   dup(restore_stderr);
   close(restore_stderr);
  }

  /* free all allocated memory */
  for (int i = 0; i < 3; i++) {
    free(file[i]);
  }
  free(file);
  deleteArgs(args);
  free(args);
}

  //Prints <Control-C> and then exits
  void interrupt_handler(int signum) {
      printf("<Control-C>\n");
      signum = 0;
      if (signum == 0) {
        return;
      }
    }

    int main(int argc, char *argv[]) {
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
      USE(argc);
      USE(argv);
      setvbuf(stdout, NULL, _IONBF, 0); 

      //interupt handler code
      signal(SIGINT, interrupt_handler); 
      while (1) {
        //print the prompt
        prompt();
        fflush(stdout);
        //read arguments and execute them
        getargs(); 
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


  char** readArgs(char* error, char** file) {
    char** arguments = (char**)calloc(5, sizeof(char*));
    if (!arguments) {
      memoryError();
    }
    int argCount = 0;
    int argSteps = 1;
    char c = getchar();
    while (c == ' ' || c == '\t') {
    /* remove leading spaces */
      c = getchar();
    }
    char* cmdWord = (char*)calloc(CMD_WORD_CHUNK + 1, sizeof(char));
    if (!cmdWord) {
      memoryError();
    }
    int wordChunks = 1;
    int wordLength = 0;
    fd redirect = -1;

  /* eliminate whitespace */
    while (c != '\n' && c != EOF) {
      if (c == '\t' || c == ' ') {
        while (c == '\t' || c == ' ') {
          c = getchar();
        }
        cmdWord[wordLength] = 0;
        redirect = addWord(cmdWord, &arguments, &argCount, &argSteps, error, redirect, file);
        wordChunks = 1;
        wordLength = 0;
        cmdWord = (char*)calloc(CMD_WORD_CHUNK + 1, sizeof(char));
        if (!cmdWord) {
          memoryError();
        }
        continue;
      }

    /* handle escape characters */
      if (c == '\\') {
        c = getchar();
        switch(c) {
          case '\\':
          case ' ':
          case '&':
              break;
          case 't':
              c = '\t';
              break;
          case 'n':
              c = '\n';
              continue;
          default:
              *error |= INVALID_ESCAPE;
//return NULL;
        }
        cmdWord[wordLength] = c;
        wordLength++;
        c = getchar();
        continue;
      }
/* Picks up any &'s that are not escaped */
      if (c == '&' && ~(*error&BACKGROUND)) {
        if (*error&BACKGROUND)
          *error|=INVALID_SYNTAX;
        background_handler(error,&c);  
        if (*error&BACKGROUND)
          break;
      }

    /* if no space remains for characters in string, reallocate more characters */
      if (wordLength == CMD_WORD_CHUNK * wordChunks) {
        wordChunks++;
        char* newWord = (char*)calloc(CMD_WORD_CHUNK * wordChunks + 1, sizeof(char));
        if (!newWord) {
          memoryError();
        }
        memcpy(newWord, cmdWord, sizeof(char) * CMD_WORD_CHUNK * (wordChunks - 1));
        free(cmdWord);
        cmdWord = newWord;
      }

    /* add character to word and increment wordLength, grab next available character */
      cmdWord[wordLength] = c;
      wordLength++;
      c = getchar();
    }
  /* add words that weren't followed by whitespace */
    if (wordLength) {
      cmdWord[wordLength] = 0;
      redirect = addWord(cmdWord, &arguments, &argCount, &argSteps, error, redirect, file);
    }

  /* mark EOF_FOUND flag if EOF is last character received */
    if (c == EOF) {
      (*error) |= EOF_FOUND;
    }
    return arguments;
  }
  fd addWord(char* word, char*** arguments, int* argCount, int* argSteps, char* error, fd redirect, char** file) {
/* check for unhandled redirection operators first */
    if (redirect != -1) {
      if (strncmp(word, "<", 1) == 0 || strncmp(word, ">", 1) == 0 || strncmp(word, "2>", 2) == 0) {
        *error |= INVALID_SYNTAX;
      }
      file[redirect] = word;
      return -1;
    }
  /* now check for redirection operators */
  /* any attempt to redirect redirected STDIN, STDOUT, or STDERR is a syntax error */
    else if (strncmp(word, "<", 1) == 0) {
      if (file[STDIN][0]) {
        *error |= INVALID_SYNTAX;
        return -1;
      } else {
        *error |= REDIR_STDIN;
        return STDIN;
      }
    } else if (strncmp(word, ">", 1) == 0) {
      if (file[STDOUT][0]) {
        *error |= INVALID_SYNTAX;
        return -1;
      } else {
        *error |= REDIR_STDOUT;
        return STDOUT;
      }
    } else if (strncmp(word, "2>", 2) == 0) {
      if (file[STDERR][0]) {
        *error |= INVALID_SYNTAX;
        return -1;
      } else {
        *error |= REDIR_STDERR;
        return STDERR;
      }
    } else {
      if (file[STDERR][0] || file[STDOUT][0] || file[STDIN][0]) {
        *error |= INVALID_SYNTAX;
        return -1;
      }
    /* check if we have hit edge of allocated space for arguments and reallocate if necessary */
      if (*argCount == (*argSteps * 5)) {
        (*argSteps)++;
        char** newArguments = (char**)calloc(*argSteps * 5, sizeof(char*));
        if (!newArguments) {
          memoryError();
        }
        memcpy(newArguments, *arguments, sizeof(char**) * ((*argSteps) - 1) * 5);
        free(*arguments);
        *arguments = newArguments;
      }
    /* add word to arg array and increment argCount */
      (*arguments)[*argCount] = word;
      (*argCount)++;
      return -1;
    }

}

  void background_handler(char* error, char* next) {
    /* seek characters after ampersand, anything but whitespace designates a syntax error */
          char c = getchar();
          while (c == ' ' || c == '\t') {
                  c = getchar();
          }
          if (c != '\n' && c != EOF) {
                  (*error) |= INVALID_SYNTAX;
          }
          else
                  (*error) |= BACKGROUND;
          *next = c;
  }

  void deleteArgs(char** args) {
    char* arg = *(args);
    while (arg != 0) {
      free(arg);
      arg = *(++args);
    }
  }

  void memoryError() {
    printf("HERE BE DRAGONS");
    do_exit();
  }