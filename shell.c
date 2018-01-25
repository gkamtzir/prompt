#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

//STRICT_MODE: 0
//When a command execution fails and there is not a ';'
//delimiter in the remaining command, the program prints
//an error message and returns to the prompt.
//STRICT_MODE: 1
//If the previous scenario occurs, the program exits,
//instead of returning to the prompt.
#ifdef STRICT
    #define STRICT_MODE 1
#else
    #define STRICT_MODE 0
#endif

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define COMMAND_LENGTH 512
#define BATCH_MODE 1
#define INTERACTIVE_MODE 2

typedef struct Command {
    char *command;
    int right;
}  Command;

int verify_command(char *command, char character, int allowed_occurrences);
int is_file_empty(FILE *file);
int parse_commands(Command commands[], char *command, char *delimiter);
void parse_arguments(char **args, char *output);

int main(int argc, char **argv)
{

    char command[COMMAND_LENGTH];
    Command commands[COMMAND_LENGTH];
    char *args[COMMAND_LENGTH];
    char *output;

    FILE *file;

    int program_mode;

    pid_t pid;

    if (argc == 1)
    {
        program_mode = INTERACTIVE_MODE;
    }
    else if (argc == 2)
    {
        program_mode = BATCH_MODE;
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("Could not open file \n");
            exit(1);
        }

        //Checking if the file is empty.
        if (is_file_empty(file))
        {
            printf("The batch file is empty \n");
            exit(1);
        }

        //Setting the pointer to the beginning of
        //the file.
        fseek(file, 0, SEEK_SET);

    }
    else
    {
        printf("You do not have to provide any arguments for interactive mode. \n");
        printf("You have to provide 1 argument (file name) for batch mode. \n");
        exit(1);
    }

    while(1)
    {

        if (program_mode == BATCH_MODE)
        {
            //Reading from file line by line.
            if (fgets(command, COMMAND_LENGTH, file) == NULL)
            {
                fclose(file);
                exit(1);
            }

            //Verifying the command.
            if (!verify_command(command,'&', 2) || !verify_command(command, ';', 1))
            {
                printf("Undefined delimiter \n");
                continue;
            }

        }
        else
        {
            //Shell prompt.
            printf(ANSI_COLOR_GREEN "kamtziridis_8542>" ANSI_COLOR_RESET);

            //Getting the command.
            if (fgets(command, COMMAND_LENGTH, stdin) == NULL)
            {
                printf("Could not read from prompt \n");
                exit(1);
            }

            //Verifying the command.
            if (!verify_command(command,'&', 2) || !verify_command(command, ';', 1))
            {
                printf("Undefined delimiter \n");
                continue;
            }
        }

        //Removing the new line character
        output = strtok(command, "\n");

        int number_of_commands = parse_commands(commands, output, ";");

        int i = 0;

        while (i < number_of_commands)
        {

            if ((pid = fork()) < 0)
            {

                printf("Could not fork \n");
                exit(1);

            }
            else if (pid == 0)
            {

                parse_arguments(args, commands[i].command);

                //Checking if the user wants to exit.
                if (!strcmp(args[0], "quit"))
                {
                    _exit(1);

                }

                if (execvp(args[0], args) == -1)
                {
                    printf("Cound not execute this command \n");
                    _exit(2);
                }

            }
            else
            {

                //Waiting for the child-process
                //to exit.
                int status;
                pid = wait(&status);
                if (WIFEXITED(status)) {
                    status = WEXITSTATUS(status);
                }

                if (status == 0)
                {
                    i++;
                }
                else if (status == 1)
                {
                    if (program_mode == BATCH_MODE)
                    {
                        fclose(file);
                    }
                    exit(1);
                }
                else
                {

                    if (commands[i].right == 1)
                    {
                        //Checking if there is a ';' delimiter in the remaining
                        //command.
                        while (commands[i].right == 1 && i < number_of_commands)
                        {
                            i++;
                        }
                    }

                    i++;

                    //If STRICT_MODE is set to 1 and there are no
                    //remaining ';' delimiter the program exits.
                    if (STRICT_MODE == 1)
                    {
                        if (i >= number_of_commands)
                        {
                            exit(1);
                        }
                    }
                }

            }

        }

    }

    return 0;

}

int verify_command(char *command, char character, int allowed_occurrences)
{

    int i = 0;
    int counter = 0;

    while (command[i] != '\0')
    {
        if (command[i] == character)
        {

            counter++;

            if (counter == allowed_occurrences + 1)
            {
                return 0;
            }

        }
        else
        {
            if (counter > 0 && counter < allowed_occurrences)
            {
                return 0;
            }
            counter = 0;
        }

        i++;
    }

    return 1;

}

int is_file_empty(FILE *file)
{

    char c;
    int empty = 1;
    while((c = fgetc(file)) != EOF)
    {
        if (isalnum(c))
        {
            empty = 0;
            break;
        }
    }

    return empty;

}

int parse_commands(Command commands[], char *command, char *delimiter)
{

    char *token;
    char *saveptr;
    static int i = 0;
    int temp;

    token = strtok_r(command, delimiter, &saveptr);

    while (token != NULL)
    {
        if (!strcmp(delimiter, ";"))
        {
            parse_commands(commands, token, "&&");
            token = strtok_r(NULL, delimiter, &saveptr);
            commands[i-1].right = 0;
        }
        else
        {
            commands[i].command = token;
            commands[i].right = 1;
            i++;
            token = strtok_r(NULL, delimiter, &saveptr);
        }
    }

    //Only the initially called function will return
    //the number of commands that are about to be
    //executed.
    if (!strcmp(delimiter, ";"))
    {
        temp = i;
        //Initializing the i variable.
        i = 0;
        return temp;
    }

}

void parse_arguments(char **args, char* command)
{

    char *token;
    int i = 0;

    //Looping through each token and
    //populating the args array with
    //tokens.
    token = strtok(command, " ");
    while (token != NULL)
    {

        args[i] = token;
        i++;
        token = strtok(NULL, " ");

    }
    args[i] = NULL;

}
