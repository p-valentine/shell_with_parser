#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/
#include <signal.h>
#include "fcntl.h"
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 25
/*each job will have a number associated with what the user will identify it with, a process id, and the command*/
struct jobs{
  int user_id;
  int process_id;
  char *command;
};


enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,JOBS, CD, KILL, HELP, HISTORY, PREV};




char *
buildPrompt()
{
  char buffer[100];
  char * current_working_directory = getcwd(buffer, sizeof(buffer));
  if( current_working_directory != NULL ){
      printf("%s:: ", current_working_directory);
      return 0;

  }else{
      perror("There has been an error in function buildPrompt with current_working_directory");
  }
}
 
int
isBuiltInCommand(char * cmd){
  
  

  if ( strncmp(cmd, "exit", strlen("exit")) == 0){
    return EXIT;
  } else if( strncmp(cmd, "jobs", strlen("jobs")) == 0){
    return JOBS;
  } else if( strncmp(cmd, "cd", strlen("cd")) == 0){
    return CD;
  } else if( strncmp(cmd, "kill", strlen("kill")) == 0){
    return KILL;
  } else if( strncmp(cmd, "help", strlen("help")) ==0){
    return HELP;
  } else if( strncmp(cmd, "history", strlen("history")) == 0) {
    return HISTORY;
  } else if( strncmp(cmd, "!", strlen("!")) == 0){
    return PREV;
  }
  return NO_SUCH_BUILTIN;
}

/*start process still has bugs in it*/
int
startProcess(char * cmd, parseInfo *info)
{
  pid_t childpid, tpid;
  int child_status;
  childpid = fork();
  if(childpid == 0){
    execvp(cmd, info->CommArray->VarList);
  }else if(childpid == -1){
    printf("failed to fork");
  }
}


int
redirect(char * cmd, parseInfo * info)
{

  pid_t processid, pid;
  int processid_status, in, out;
  processid = fork();

  if(processid < 0){
    printf("Error forking (in redirect)");
  }else if(processid == 0){
    if(info->boolInfile){
      in = open((info->inFile), O_RDONLY); /*specified so that it is read only*/
      dup2(in, 0);
    }
    if(info->boolOutfile){
      out = open((info->outFile), O_WRONLY);/*specified that it is write only*/
      dup2(out, 1);
    }
    execvp(cmd, info->CommArray->VarList);

  } else{
    do{
      pid = waitpid(processid, &processid_status, WUNTRACED);
    }while(!WIFEXITED(processid_status) && !WIFSIGNALED(processid_status));
    return processid_status;
  }
 
}

int 
main (int argc, char **argv)
{

  char * cmdLine;
  char * history[10];
  char * newCommand;
  int i, new_command_index,j;
  parseInfo *info; /*info stores all the information returned by parser.*/
  struct commandType *com; /*com stores command name and Arg list for one command.*/
  struct jobs list_of_jobs[SIZE];/*makes an array of jobs so that jobs can be stored*/

  /*initialize all of the jobs process id's to be */
  i=0;
  for(i; i < SIZE; i++){
    list_of_jobs[i].process_id = -1;
  }

#ifdef UNIX
  
    fprintf(stdout, "This is the UNIX version\n");
#endif

#ifdef WINDOWS
    fprintf(stdout, "This is the WINDOWS version\n");
#endif

  while(1){
    /*insert your code to print prompt here*/
    /*buildPrompt();*/

#ifdef UNIX
    cmdLine = readline(buildPrompt());
    if (cmdLine == NULL) {
      fprintf(stderr, "Unable to read command\n");
      continue;
    }
#endif
   
    /*calls the parser*/
    info = parse(cmdLine);
    if (info == NULL){
      free(cmdLine);
      continue;
    }
    /*prints the info struct*/
    print_info(info);

    /*com contains the info. of the command before the first "|"*/
    com=&info->CommArray[0];
    if ((com == NULL)  || (com->command == NULL)) {
      free_info(info);
      free(cmdLine);
      continue;
    }

  /*insert your code about history and !x !-x here*/
  add_history(cmdLine);

    if(info->boolInfile || info->boolOutfile){
      redirect(com->command, info);
    }
    
    /*com->command tells the command name of com*/
    if (isBuiltInCommand(com->command) == EXIT){
      exit(1);
    } else if (isBuiltInCommand(com->command) == JOBS){
      /*still need to implement jobs*/
    } else if (isBuiltInCommand(com->command) == CD){
      chdir(info->CommArray[0].VarList[1]);
    } else if (isBuiltInCommand(com->command) == HISTORY){
      int i = 0;
      HIST_ENTRY ** history = history_list();
      if(!history){
        return;
      }
      while(history[i] != NULL) {
        HIST_ENTRY *entry = history[i];
        printf("(%d) %s\n", i, entry->line);
        i++;
      }
    } else if (isBuiltInCommand(com->command) == HELP){
      /*system(com->command);*/
      printf("# cd <relative or absolute directory>\n# kill (process_id)\n# history\n# jobs\n# ! (executes previous)\n# exit\n");
    } else if(isBuiltInCommand(com->command) == PREV){
      printf("The call to PREV is working\n");
    } else if(isBuiltInCommand(com->command) == KILL){
      int pid_to_kill;
      printf("Enter PID you want to kill: ");
      scanf("%d", pid_to_kill);
      kill(pid_to_kill, SIGKILL);
    } else if(isBuiltInCommand(com->command) == JOBS){
      printf("Have not implemented jobs yet");
    }

    /*startProcess(com->command, info); --- tried using this but really weird things started happening, works all the time when using redirect function*/
    redirect(com->command, info);

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
  
