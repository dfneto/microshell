#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return (i);
}
int ft_putstr_fd2(char *str)
{
    write(2, str, ft_strlen(str));
    return (1);
}
int ft_execute(char *argv[], int i, int tmp_fd, char *env[])
{
    //overwrite ; or | or NULL with NULL to use the array as input for execve.
    //we are here in the child so it has no impact in the parent process.
    argv[i] = NULL; // le ponemos un NULL donde termina para que no le pasemos nada mas. Si se lo pusieramos en el padre nos cargariamos el parseo
    dup2(tmp_fd, 0); // donde se printa por defecto sera el temp_fd
    close(tmp_fd);
    execve(argv[0], argv, env);
    ft_putstr_fd2("error: cannot execute ");
    ft_putstr_fd2(argv[0]);
    ft_putstr_fd2("\n");
    return (1);
}
int main(int argc, char *argv[], char *env[])
{
    int i;
    int fd[2];
    int tmp_fd;
    int pid;
    (void)argc; // is needed in exam, because the exam tester compiles with -Wall -Wextra -Werror
    i = 0;
    tmp_fd = dup(0);
    while (argv[i] && argv[i + 1]) //check if the end is reached
    {
        argv = &argv[i + 1];    //the new argv start after the ; or |
        i = 0;
        //count until we have all informations to execute the next child;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|")) //conta os argumentos que há até o fim da linha ou até ; ou |
            i++;
        if (strcmp(argv[0], "cd") == 0) //cd
        {
            printf("JOAN CD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            if (i != 2) // mas de 2 argumentos suponiendo que el primero es cd
                ft_putstr_fd2("error: cd: bad arguments\n");
            else if (chdir(argv[1]) == -1)
            {
                ft_putstr_fd2("error: cd: cannot change directory to ");
                ft_putstr_fd2(argv[1]);
                ft_putstr_fd2("\n");
            }
        }
        else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0)) //exec in stdout
        {
            pid = fork();
            if (pid == -1)
                return 1;
            if (pid == 0)
            {
                if (ft_execute(argv, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(tmp_fd);
                waitpid(pid, NULL, WUNTRACED);
                tmp_fd = dup(0);
            }
        }
        else if(i != 0 && strcmp(argv[i], "|") == 0) //pipe
        {
            pipe(fd);
            pid = fork();
            if (pid == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                if (ft_execute(argv, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return (0);
}