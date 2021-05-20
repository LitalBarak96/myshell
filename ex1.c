//lital barak 314877051
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct HistoryCommand
{
    int pid;
    char command_name[100];
    bool is_job;
};

struct HistoryCommand commands_history[100];

int command_history_index = 0;

//checking if it running
bool check_is_running(int pid)
{
    int stat;
    int waited;
    waited = waitpid(pid, &stat, WNOHANG);
    if (waited == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//getting the input and parsering it
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

//change dirctory command
void cd_command(char *dir)
{

    if (strcmp(dir, "~") == 0)
    {
        strcpy(dir, getenv("HOME"));
    }
    int chdirvalue = chdir(dir);
    if (chdirvalue != 0)
    {
        printf("chdir failed\n");
    }
}

//bulid in command
bool is_built_int_command(char *command)
{
    char known_commands[3][20] = {"cd", "history", "jobs"};
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        if (strcmp(known_commands[i], command) == 0)
        {
            return true;
        }
    }
    return false;
}

// jobs command
void jobs_command()
{
    int i = 0;
    for (i = 0; i < command_history_index; i++)
    {
        int stat;
        if (commands_history[i].is_job == true)
        {
            bool is_running = check_is_running(commands_history[i].pid);
            if (is_running)
            {
                printf("%s\n", commands_history[i].command_name);
            }
        }
    }
}
//history command
void history_command()
{
    int i = 0;
    for (i = 0; i < command_history_index; i++)
    {
        printf("%s ", commands_history[i].command_name);
        int stat;
        if (is_built_int_command(commands_history[i].command_name))
        {
            if (i == command_history_index - 1)
            {
                printf("RUNNING");
            }
            else
            {
                printf("DONE");
            }
        }
        else
        {
            bool is_running = check_is_running(commands_history[i].pid);
            if (is_running == true)
            {
                printf("RUNNING");
            }
            else
            {
                printf("DONE");
            }
        }
        printf("\n");
    }
}
// add to history command
void add_to_history(char **command, int pid, bool is_job)
{
    struct HistoryCommand command_history;
    command_history.pid = pid;
    command_history.is_job = is_job;
    char command_with_args[300] = "";
    int len = 0;
    while (command[len] != NULL)
    {
        strcat(command_with_args,command[len]);
        if(command[len + 1] != NULL){
            strcat(command_with_args," ");
        }
        len++;
    }
    strcpy(command_history.command_name, command_with_args);
    commands_history[command_history_index] = command_history;
    command_history_index++;
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

    if (strcmp(command[0], "echo") == 0)
    {
        char *string_to_print = command[1];
        char parsed[500] = "";
        int parsed_index = 0;
        int i = 0;
        for (i = 0; i < strlen(string_to_print); i++)
        {
            if (string_to_print[i] == '"')
            {
            }
            else
            {
                parsed[parsed_index] = string_to_print[i];
                parsed_index++;
            }
        }
        strcpy(command[1], parsed);
    }

    pid = fork();
    if (pid == -1)
    {
        printf("fork failed\n");
        exit(-1);
    }
    add_to_history(command, pid, isbackground);

    if (pid == 0)
    { /* Child */
        ret_code = execvp(command[0], command);
        if (ret_code == -1)
        {
            printf("exec failed\n");
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

//running the bulid in command
void run_built_in_command(char **command)
{
    add_to_history(command, 0, false);
    if (strcmp(command[0], "cd") == 0)
    {
        int len = 0;
        while (command[len] != NULL)
        {
            len++;
        }
        if (len > 2)
        {
            printf("Too many arguments\n");
            return;
        }
        cd_command(command[1]);
    }
    if (strcmp(command[0], "history") == 0)
    {
        history_command();
    }
    if (strcmp(command[0], "jobs") == 0)
    {
        jobs_command();
    }
}

int main(int argc, char *argv[])
{
    char userinput[300];
    char **command;
    printf("$ ");
    fflush(stdout);
    fgets(userinput, 300, stdin);
    userinput[strlen(userinput) - 1] = '\0';
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
        fgets(userinput, 300, stdin);
        userinput[strlen(userinput) - 1] = '\0';
    }
}