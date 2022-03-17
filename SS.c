#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "loglist.h"
#include "envlist.h"
#include <time.h>
#include <sys/wait.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"
#define WRITE_END 1
#define READ_END 0
int x = 0;

int is_whitespace(const char* line)
{
    if (line == NULL) {
        return 1;
    }
    int check = 1;
    int i = 0;
    while (check && line[i] != '\0')
    {
        if (!isspace(line[i]))
        {
            check = 0;
        }
        i++;
    }
    return check;
}

int containsSpace(const char* line) {
    int check = 0;
    int i = 0;
    while (check && line[i] != '\0')
    {
        if (isspace(line[i]))
        {
            check = 1;
            break;
        }
        i++;
    }
    return check;
}

void removeSpace(char* line) {

    int count = 0;
    for (int i = 0; line[i]; i++)
    {
        if (line[i] != ' ' && line[i] != '\n')
        {
            line[count++] = line[i];
        }
    }
    line[count] = 0;
}

void OpSysOutput(struct LogList* LogList, char* command) {
    int fd[2];
    int fd2[2];

    char* arg = strtok(NULL, " ");
    char* allargs[256];
    allargs[0] = command;

    int i = 1;
    while (arg != NULL)
    {
        removeSpace(arg);
        allargs[i] = arg;
        arg = strtok(NULL, " ");
        i++;
    }
    allargs[i] = NULL;

    int check = 0;
    pipe(fd);
    pipe(fd2);

    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    }
    else if (pid == 0)
    {
        dup2(fd[READ_END], STDIN_FILENO);

        close(fd[WRITE_END]);
        close(fd[READ_END]);
        close(fd2[0]);

        if (execvp(command, allargs) < 0) {
            check = 1;
            write(fd2[1], &check, sizeof(check));
            close(fd2[1]);
            char* err = "Failed to execute command";
            write(STDERR_FILENO, err, strlen(err));
            // execl will always be true below, exit(1) malfunctioned in script mode, therefore execl is being used to takeover child instead
            // comment the line below to check, (code will still run except when an incorrect non builtin command is passed in script mode)
            // interactive mode will still run fine.
            execl("/bin/echo", "", NULL); 
            //code never reaches here.
            exit(1);
        };

    }
    else {
        int status;
        close(fd[READ_END]);
        close(fd[WRITE_END]);

        close(fd2[1]);
        read(fd2[0], &check, sizeof(check));
        close(fd2[0]);

        wait(NULL);
        insert(LogList, createLogNode(command, check));
    }
}


int shell_output(struct LogList* LogList, struct EnvList* EnvList, char* fullLine, char* command, char* theme)
{
    //two seperate linked lists (LogList & EnvList), one to store commands as LogNodes for keeping track of commands,
    //the other stores EnvNodes to keep track of all the env variables.
    char* arg = NULL;
    int check = 1;
    char EnvArg[512];
    removeSpace(command);
    strcpy(EnvArg, command);

    if (command != NULL)
    {
        //COMMAND #1 Print: can not be empty, must have an argument (spaces acceptable)
        if (strcmp(command, "print\n") == 0 || strcmp(command, "print") == 0)
        {
            arg = strtok(NULL, "");
            if (arg != NULL && !is_whitespace(arg))
            {
                char* copy2 = malloc(sizeof(char) * strlen(arg + 1000)); //extra size in case envvar values are much larger than their names. ($foo=foobarfoobar)
                int  i = 0;
                while (arg[i] != 0) {
                    if (arg[i] == 36) {
                        int j = 0;
                        strcpy(copy2, "");
                        while (1) {
                            if (arg[i] == 0 || arg[i] == '\n' || arg[i] == ' ') {
                                copy2[j] = 0;
                                break;
                            }
                            else {
                                copy2[j] = arg[i];
                                j++;
                                i++;
                            }
                        }
                        printf("%s", find(EnvList, copy2));
                    }
                    else {
                        printf("%c", arg[i]);
                        i++;
                    }
                }
                insert(LogList, createLogNode(command, 0));
                free(copy2);
            }
            else
            {
                printf("I need an argument to print\n");
                insert(LogList, createLogNode(command, 1));
            }
        }
        //COMMAND #2 Exit: must be empty, can not have an argument (spaces are acceptable)
        else if (strcmp(command, "exit\n") == 0 || strcmp(command, "exit") == 0)
        {
            arg = strtok(NULL, " ");
            insert(LogList, createLogNode(command, 0));
            if (arg == NULL || is_whitespace(arg))
            {
                printf("see you later!\n");
                return 0;
            }
            else
            {
                printf("exit can not be paired with arguments\n");
                insert(LogList, createLogNode(command, 1));
            }
        }
        //COMMAND #3 Theme: must have an argument (acceptable arguments are: red, blue, green and yellow)
        else if (strcmp(command, "theme\n") == 0 || strcmp(command, "theme") == 0)
        {
            arg = strtok(NULL, " ");
            char* extra_arg = strtok(NULL, " ");
            if (arg == NULL || is_whitespace(arg))
            {
                printf("specify color next time!\n");
                insert(LogList, createLogNode(command, 1));
            }
            else if (extra_arg != NULL && !is_whitespace(extra_arg)) {
                printf("Please enter one argument only!\n");
                insert(LogList, createLogNode(command, 1));
            }
            else
            {
                if (strcmp(arg, "red\n") == 0 || strcmp(arg, "red") == 0) {
                    insert(LogList, createLogNode(command, 0));
                    strcpy(theme, RED);
                }
                else if (strcmp(arg, "green\n") == 0 || strcmp(arg, "green") == 0) {
                    insert(LogList, createLogNode(command, 0));
                    strcpy(theme, GRN);
                }
                else if (strcmp(arg, "blue\n") == 0 || strcmp(arg, "blue") == 0) {
                    insert(LogList, createLogNode(command, 0));
                    strcpy(theme, BLU);
                }
                else if (strcmp(arg, "reset\n") == 0 || strcmp(arg, "reset") == 0) {
                    insert(LogList, createLogNode(command, 0));
                    strcpy(theme, RESET);
                }
                else {
                    printf("Please select from: red, green, blue or reset\n");
                    insert(LogList, createLogNode(command, 1));
                }
            }
        }
        else if (strcmp(command, "log\n") == 0 || strcmp(command, "log") == 0) {
            arg = strtok(NULL, " ");
            if (arg == NULL || is_whitespace(arg))
            {
                print(LogList);
                insert(LogList, createLogNode(command, 0));
            }
            else {
                insert(LogList, createLogNode(command, 1));
                printf("Log does not take in arguments\n");
            }
        }
        else if (!containsSpace(EnvArg) && EnvArg[0] == 36) {
            char* name = strtok(EnvArg, "=");

            arg = strtok(NULL, " ");
            fullLine[strlen(fullLine) - 1] = 0;
            if (arg != NULL && name != NULL && strlen(name) > 1) {

                struct EnvNode* temp = NULL;
                temp = exists(EnvList, name);

                if (temp != NULL) {
                    setArg(temp, arg);
                }
                else {
                    insertEnvNode(EnvList, createEnvNode(name, arg));
                }
                struct LogNode* log = createLogNode(command, 0);
                insert(LogList, log);

            }
            else {
                struct LogNode* log = createLogNode(fullLine, 1);
                insert(LogList, log);
            }
        }
        else {
            OpSysOutput(LogList, command);
        }
    }
    return 1;
}

int main(int argc, char** argv)
{
    char* line = NULL;
    char* command = " ";
    char* fullLine = malloc(sizeof(char) * 1000);
    size_t line_size = 0;
    int loop = 1;
    char theme[20];

    struct LogList* LogList = (struct LogList*)malloc(sizeof(struct LogList));
    struct EnvList* EnvList = (struct EnvList*)malloc(sizeof(struct EnvList));
    FILE* fp;

    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (!fp)
        {
            fprintf(stderr, "Error opening file '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    int check = 1;
    while (loop)
    {
        printf(theme);
        if (argc > 1) {
            x++;
            check = getline(&line, &line_size, fp);
            if (check == -1) {
                printf("see you later!\n");
                break;
            }
        }
        else {
            printf("cshell$ " RESET);
            getline(&line, &line_size, stdin);
        }
        printf(theme);

        if (line != NULL)
        {
            if (line[0] == 10) {
                continue;
            }
            strcpy(fullLine, line);
            command = strtok(line, " ");
        }
        else
        {
            printf("error reading line!\n");
        }

        loop = shell_output(LogList, EnvList, fullLine, command, theme);
    }
    if(argc > 1){
        fclose(fp);
    }
    freeLogList(LogList);
    freeEnvList(EnvList);
    free(line);
    free(fullLine);
}