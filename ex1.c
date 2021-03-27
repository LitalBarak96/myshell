#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct HistoryCommand {
   int  pid;
   bool  is_running;
   char  command_name[100];
}; 


struct HistoryCommand commands_history[100];
int command_history_index = 0;

char **get_input(char *input)
{
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL)
    {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

bool is_built_int_command(char *command)
{
    char known_commands[3][20] = {"cd", "history", "jobs"};
    for (int i = 0; i < 3; i++)
    {
        if (strcmp(known_commands[i], command) == 0)
        {
            return true;
        }
    }
    return false;
}

void run_command(char **command)
{
    int stat, waited, ret_code;
    pid_t pid;
    bool isbackground = false;
    int len = 0;
    while (command[len] != NULL)
    {
        len++;
    }

    if (strcmp(command[len - 1], "&") == 0)
    {
        command[len - 1] = NULL;
        isbackground = true;
    }

    pid = fork();

    if (pid == 0)
    { /* Child */
        ret_code = execvp(command[0], command);
        if (ret_code == -1)
        {
            perror("exec failed");
            exit(-1);
        }
    }
    else
    { /* Parent */
        if (!isbackground)
        {
            waited = wait(&stat); /* stat can tell what happened */
        }
    }
}

void run_built_in_command(char **command){
    if(strcmp(command[0],"cd") == 0){
        cd_command(command[1]);
    }
}

void cd_command(char *dir){
    if(strcmp(dir,"~") == 0){
        strcpy(dir,getenv("HOME"));
    }
    chdir(dir);
}

void history_command(){
    
}

void add_to_history(char *command,int pid){
    struct HistoryCommand command_history;
    command_history.is_running = true;
    strcpy(command_history.command_name, command);
    commands_history[command_history_index] = command_history;
}

int main(int argc, char *argv[])
{
    char userinput[300];
    char **command;
    printf("$ ");
    fflush(stdout);
    gets(userinput);

    while (strcmp(userinput, "exit") != 0)
    {
        command = get_input(userinput);

        bool built_in_cmd = is_built_int_command(command[0]);
        if (built_in_cmd)
        {
           run_built_in_command(command);
        }
        else
        {
            run_command(command);
        }

        printf("$ ");
        fflush(stdout);
        gets(userinput);
    }
}