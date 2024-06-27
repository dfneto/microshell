#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int err(char *str)
{
    while (*str)
        write(2, str++, 1);
    return (1);
}

int cd(char **argv, int i)
{
    if (i != 2)
        return err("error: cd: bad arguments\n");
    if (chdir(argv[1]) == -1)
        return err("error: cd: cannot change directory to "), err(argv[1]), err("\n");
    return (0); //deve retornar 0 em caso de sucesso
}

int exec(char **argv, int i, char **envp)
{
    int fd[2];
    int status = 0;
    int has_pipe = argv[i] && !strcmp(argv[i], "|");

    //cd
    if (!has_pipe && !strcmp(argv[0], "cd"))
        return cd(argv, i);
    //if pipe: pipe
    if (has_pipe && pipe(fd) == -1)
        return err("error: fatal\n");

    //fork
    int pid = fork();
    if (pid == -1) 
        return err("error: fatal\n");
    //filho
    if (pid == 0)
    {
        argv[i] = 0; 
        //redirect
        if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return err("error: fatal\n");
        //cd
        if (!strcmp(argv[0], "cd"))
            return cd(argv, i);
        //executa
        execve(*argv, argv, envp);
        return err("error: cannot execute "), err(*argv), err("\n");
    }
    //pai
        //wait
        waitpid(pid, &status, 0);
        //redirect
        if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return err("error: fatal\n");
        //retorno status
        return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    int status = 0;

    if (argc > 1)
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