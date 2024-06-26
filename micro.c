#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int err(char *str)
{
    while (*str)
        write(2, str++, 1);
    return 1; //para sinalizar que ocorreu um erro
}

int cd(char **argv, int i)
{
    if (i != 2)
        return err("error: cd: bad arguments\n"); //return 1
    if (chdir(argv[1]) == -1)
        return err("error: cd: cannot change directory to "), err(argv[1]), err("\n"); //return 1
    return 0; //nao ocorreu erro
}

int exec(char **argv, int i, char **envp)
{
    int fd[2];
    int status = 0;
    int has_pipe = argv[i] && !strcmp(argv[i], "|");
    
    if (!has_pipe && !strcmp(*argv, "cd"))
        return cd(argv, i);

    if (has_pipe && pipe(fd) == -1) //se tem pipe, faça o pipe
        return err("error: fatal\n");

    int pid = fork();
    if (pid == -1)
        return err("error: fatal\n");
    if (pid == 0)
    {
        argv[i] = 0; //encerro o comando com NULL
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) //se tem pipe redirecione e feche os fds
            return err("error: fatal\n");
        if (!strcmp(*argv, "cd"))
            return cd(argv, i);
        execve(*argv, argv, envp); //execute o comando no processo filho
        return err("error: cannot execute "), err(*argv), err("\n");
    }
    waitpid(pid, &status, 0);
    if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) //se tem pipe redirecione e feche os fds
        return err("error: fatal\n");
    return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    int status = 0;
    
    if (argc > 1) //if e não while!
    {
        while (argv[i] && argv[++i])
        {
            argv += i;
            i = 0;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
                i++;
            if (i)
                status = exec(argv, i, envp);
        }
    }
    return status;
}