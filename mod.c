// /*********************************************************************
//    Program  : miniShell                   Version    : 1.3
//  --------------------------------------------------------------------
//    skeleton code for linix/unix/minix command line interpreter
//  --------------------------------------------------------------------
//    File			: minishell.c
//    Compiler/System	: gcc/linux

// ********************************************************************/

// #include <sys/types.h>
// #include <sys/wait.h>
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <signal.h>

// #define NV 20			/* max number of command tokens */
// #define NL 100			/* input buffer size */
// char            line[NL];	/* command input buffer */


// /*
// 	shell prompt
//  */

// void prompt(void)
// {
//   // ## REMOVE THIS 'fprintf' STATEMENT BEFORE SUBMISSION
//   fprintf(stdout, "\n msh> ");
//   fflush(stdout);
// }


// /* argk - number of arguments */
// /* argv - argument vector from command line */
// /* envp - environment pointer */
// int main(int argk, char *argv[], char *envp[])
// {
//    int             frkRtnVal;	    /* value returned by fork sys call */
//   char           *v[NV];	        /* array of pointers to command line tokens */
//   char           *sep = " \t\n";  /* command line token separators    */
//   int             i;		          /* parse index */

//     /* prompt for and process one command line at a time  */

//   while (1) {			/* do Forever */
//     prompt();
//     fgets(line, NL, stdin);
//     fflush(stdin);

//     // This if() required for gradescope
//     if (feof(stdin)) {		/* non-zero on EOF  */
//       exit(0);
//     }
//     if (line[0] == '#' || line[0] == '\n' || line[0] == '\000'){
//       continue;			/* to prompt */
//     }

//     v[0] = strtok(line, sep);
//     for (i = 1; i < NV; i++) {
//       v[i] = strtok(NULL, sep);
//       if (v[i] == NULL){
// 	      break;
//       }
//     }
//     /* assert i is number of tokens + 1 */

//     /* fork a child process to exec the command in v[0] */
//     switch (frkRtnVal = fork()) {
//       case -1:			/* fork returns error to parent process */
//       {
// 	      break;
//       }
//       case 0:			/* code executed only by child process */
//       {
// 	      execvp(v[0], v);
//       }
//       default:			/* code executed only by parent process */
//       {
//       	wait(0);
//         // REMOVE PRINTF STATEMENT BEFORE SUBMISSION
//         printf("%s done \n", v[0]);
//     	  break;
//       }
//     }				/* switch */
//   }				/* while */
// }				/* main */
/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/limits.h>

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
char            line[NL];	/* command input buffer */


/*
	shell prompt
 */

void prompt(void)
{
  // // ## REMOVE THIS 'fprintf' STATEMENT BEFORE SUBMISSION
  // fprintf(stdout, "\n msh> ");
  fflush(stdout);
}


/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */
int main(int argk, char *argv[], char *envp[])
{
   int             frkRtnVal;	    /* value returned by fork sys call */
  char           *v[NV];	        /* array of pointers to command line tokens */
  char           *sep = " \t\n";  /* command line token separators    */
  int             i;		          /* parse index */

  /*
    how to track background process
    use a flag to track when & is hit in command lines
    use a string to track the command for child process
    use fork to sparse a child process, store the child process id in an array
    after the command, use wait pid to track if the job in child proces done
  */

  // flag to sparse child process
  int bg_flag = 0;
  // array to track child process ids
  pid_t bg_pid_nums[100] = {0};
  // array to track child process commands
  char bg_pid_args[100][NL];
  memset(bg_pid_args, 0, sizeof(bg_pid_args));
  // increment based on number of background job
  int count = 1;

    /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    fflush(stdin);

    // This if() required for gradescope
    if (feof(stdin)) {		/* non-zero on EOF  */
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000'){
      continue;			/* to prompt */
    }

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL){
	      break;
      }
    }
    /* assert i is number of tokens + 1 */

    // check if & is at the end of the command line
    if (i > 1 && strcmp("&", v[i-1]) == 0) {
        bg_flag = 1;
        // remove & for valid commands
        v[i-1] = NULL;
    }

    // store the commands for background job
    char bg_commands[NL] = "";
    if (v[0] != NULL) {
        for (int k = 0; v[k] != NULL; k++) {
            strcat(bg_commands, v[k]);
            if (k < i - 1) strcat(bg_commands, " ");
        }
    }

    // handle built-in cd
    if (v[0] && strcmp("cd", v[0]) == 0) {
        static char prev_dir_name[PATH_MAX] = "";
        char curr_dir_name[PATH_MAX];
        getcwd(curr_dir_name, sizeof(curr_dir_name));

        const char *target = NULL;
        // go to $HOME : no arguments
        if (v[1] == NULL) {
            target = getenv("HOME");
            if (target == NULL) {
                target = "/";
            }
        } else {
          target = v[1]; 
        }
        if (chdir(target) == -1) {
            perror("chdir");
        }
        else {
            strncpy(prev_dir_name, curr_dir_name, sizeof(prev_dir_name));
            prev_dir_name[sizeof(prev_dir_name) - 1] = '\0';
        }
        continue;
    }

    /* fork a child process to exec the command in v[0] */
    switch (frkRtnVal = fork()) {
      case -1:			/* fork returns error to parent process */
      {   
        perror("fork failed");
	    break;
      }
      case 0:			/* code executed only by child process */
      {
	    execvp(v[0], v);
        perror("execvp fail");
        exit(1);
      }
      default:			/* code executed only by parent process */
      {
        // sparse child process
        if (bg_flag) {
            bg_pid_nums[count] = frkRtnVal;
            strncpy(bg_pid_args[count], bg_commands, NL-1);
            bg_pid_args[count][NL-1] = '\0';
            printf("[%d] %d\n", count, frkRtnVal);
            count++;
        }
        else {
            if (waitpid(frkRtnVal, NULL, 0) == -1) perror("waitpid");
        }
        bg_flag = 0;
        // check if any background job finish
        for (int job = 1; job < count; job++) {
            if (bg_pid_nums[job] > 0) {
                pid_t result = waitpid(bg_pid_nums[job], NULL, WNOHANG);
                if (result == bg_pid_nums[job]) {
                    printf("[%d]+ Done %s\n", job, bg_pid_args[job]);
                    bg_pid_nums[job] = 0;
                }
            }
        }

    	break;
      }
    }				/* switch */
  }
  return 0;				/* while */
}				/* main */
