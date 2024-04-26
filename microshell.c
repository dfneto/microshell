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
int ft_execute(char *argv[], int i, char *env[])
{
    printf("Execute: argv[0]=%s, argv[1]=%s, i=%d\n", argv[0], argv[1], i);
    argv[i] = NULL;
    execve(argv[0], argv, env);
    ft_putstr_fd2("error: cannot execute "); //executa essas linhas no caso de execve disparar um erro
    ft_putstr_fd2(argv[0]);
    ft_putstr_fd2("\n");
    return (1);
}
int main(int argc, char *argv[], char *env[])
{
    int i;
    int fd[2];
    int pid;
    (void)argc; // is needed in exam, because the exam tester compiles with -Wall -Wextra -Werror
    i = 0;
    while (argv[i] && argv[i + 1]) //check if the end is reached
    {
        argv = &argv[i + 1];    //the new argv start after the ; or | ou do nome do programa
        i = 0;
        //count until we have all informations to execute the next child;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|")) //conta as instruções: conta os argumentos que há até o fim da linha ou até ; ou |
            i++;
        if (strcmp(argv[0], "cd") == 0) //cd
        {
            if (i != 2) // mas de 2 argumentos suponiendo que el primero es cd
                ft_putstr_fd2("error: cd: bad arguments\n");
            else if (chdir(argv[1]) == -1) //abro o path. argv: cd (argv[0]) /home (argv[1])
            {
                ft_putstr_fd2("error: cd: cannot change directory to ");
                ft_putstr_fd2(argv[1]);
                ft_putstr_fd2("\n");
            }
        }
        else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0)) //se chegou no fim ou em ;
        {                                                                   //exec in stdout
            pid = fork();
            if (pid == 0) //proceso filho
            {
                if (ft_execute(argv, i, env)) //por que preciso de um fork e não simplesmente executo o comando? Porque quando executo execve, ele cria um novo processo e tudo que vem depois dele não é executado, a menos que tenha um erro 
                    return (1);
            }
            else
            {
                waitpid(pid, NULL, WUNTRACED);
            }
        }
        else if(i != 0 && strcmp(argv[i], "|") == 0) //pipe
        {
            pipe(fd); //fd[0] read, fd[1] write
            pid = fork();
            if (pid == 0)
            {
                if (ft_execute(argv, i, env))
                    return (1);
            }
            else
            {
            }
        }
    }
    return (0);
}