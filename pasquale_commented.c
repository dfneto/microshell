#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdio.h>

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

int exec(char **argv, int i, char **envp)
{

    int fd[2];  //fd[0] read, fd[1] write
    int status;
    int has_pipe = argv[i] && !strcmp(argv[i], "|"); // se argv == |

    if (!has_pipe && !strcmp(*argv, "cd")) //se argv == cd
        return cd(argv, i);

    if (has_pipe && pipe(fd) == -1)
        return err("error: fatal\n");

    int pid = fork(); 
    if (!pid) //filho: pid = 0
    {
        argv[i] = 0; //essa funcao recebe sempre o i apontando para um elemento que nao faz parte do comando. Ex: /bin/ls | com i = 1. Ou seja, temos que terminar o comando com nulo e nao com |. Teste um echo hola ";" /bin/ls com e sem isso.
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) //dup2(fd[1], 1): ao invés de printar no terminal, vou printar para onde fd[1] aponta
            return err("error: fatal\n");
        if (!strcmp(*argv, "cd")) //se argv == cd. para o caso cd ... | ...
            return cd(argv, i);
        execve(*argv, argv, envp);//recebe o path do comando a ser executado que é o primeiro parâmetro de argv (*argv), argv sao os argumentos para o comando e environ ??)
        return err("error: cannot execute "), err(*argv), err("\n"); //executa essa linha no caso de execve disparar um erro
    }

    waitpid(pid, &status, 0); //Entendo que status vai receber o valor do status retornado pelo filho
    if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) //dup2(fd[0], 0): stdin passa a apontar para fd[0]. Assim, quando for ler, vou ler para onde fd[0] aponta
        return err("error: fatal\n");
    return WIFEXITED(status) && WEXITSTATUS(status);
    /*
    * WIFEXITED(status)
    * returns true if the child terminated normally, that is, 
    * by calling exit(3) or _exit(2), or by returning from main().
    * WEXITSTATUS(status)
    * returns the exit status of the child. This macro should only 
    * be employed if WIFEXITED returned true.
    */
}

int main(int argc, char **argv, char **envp)
{
    int    i = 0;
    int    status = 0;

    if (argc > 1)
    {
        while (argv[i] && argv[++i]) //enquanto houver o parametro atual e o seguinte
        {
            argv += i; //eh uma forma de fazer o argv apontar para o comando
	        i = 0;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";")) //enquanto argv nao seja | ou ;
                i++;
            if (i) //i=0 quando o comando começa com "|" ou ";", assim não executa
                status = exec(argv, i, envp); 
        }
    }
    return status; //acredito que retorno o status do último comando executado
}