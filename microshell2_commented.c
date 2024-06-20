#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int err(char *str)
{
    while (*str)
        write(2, str++, 1);
    return 1;
}

int cd(char **argv, int i)
{
    if (i != 2)
        return err("error: cd: bad arguments\n");
    if (chdir(argv[1]) == -1)
        return err("error: cd: cannot change directory to "), err(argv[1]), err("\n");
    return 0;
}

int exec(char **argv, int i)
{

    printf("i = %d, argv[i] = %s\n", i, argv[i]);
    int fd[2];
    int status;
    int has_pipe = argv[i] && !strcmp(argv[i], "|");

    if (!has_pipe && !strcmp(*argv, "cd"))
        return cd(argv, i);

    if (has_pipe && pipe(fd) == -1)
        return err("error: fatal\n");

    int pid = fork();
    if (!pid)
    {
        argv[i] = 0; //essa funcao recebe sempre o i apontando para um elemento que nao faz parte do comando. Ex: /bin/ls | com i = 1. Ou seja, temos que terminar o comando com nulo e nao com |.
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return err("error: fatal\n");
        if (!strcmp(*argv, "cd"))
            return cd(argv, i);
	printf("--->*argv = %s, argv = %s\n", *argv, argv[0]);
        execve(*argv, argv, environ);//recebe o path do comando a ser executado, argv sao os argumentos para o comando e environ ??)
        return err("error: cannot execute "), err(*argv), err("\n");
    }

    waitpid(pid, &status, 0);
    if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
        return err("error: fatal\n");
    return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv)
{
    int    i = 0;
    int    status = 0;

//    printf("argv[0] = %s\n", argv[0]);
    if (argc > 1)
    {
        while (argv[i] && argv[++i]) //enquanto houver o parametro atual e o seguinte
        {
            argv += i; //eh uma forma de fazer o argv apontar para o comando
            printf("i = %d, argv[0] = %s\n", i, argv[0]);
	    i = 0;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";")) //enquanto nao seja | ou ;
                i++;
            if (i) //i=0 quando o comando termina com "|" ou ";"  
	    {
		printf("Entrou em: %s\n", argv[i]);
                status = exec(argv, i);
	    }
        }
    }
    return status;
}
/*
./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
               ^ argv
i=1
*/
