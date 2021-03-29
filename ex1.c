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
    add_to_history(command[0], pid, isbackground);

    if (pid == 0)
    { /* Child */
        ret_code = execvp(command[0], command);
        if (ret_code == -1)
        {
            perror("exec failed\n");
            exit(-1);
        }
    }
    else
    { /* Parent */
        if (!isbackground)
        {
            waited = wait(&stat); /* stat can tell what happened */
        }
        else
        {
            printf("fork failed\n");
        }
    }
}

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

void run_built_in_command(char **command)
{
    add_to_history(command[0], 0, false);
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

void jobs_command()
{
    for (int i = 0; i < command_history_index; i++)
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

void history_command()
{
    for (int i = 0; i < command_history_index; i++)
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

void add_to_history(char *command, int pid, bool is_job)
{
    struct HistoryCommand command_history;
    command_history.pid = pid;
    command_history.is_job = is_job;
    strcpy(command_history.command_name, command);
    commands_history[command_history_index] = command_history;
    command_history_index++;
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