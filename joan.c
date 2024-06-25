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
    printf("Execute: tmp_fd=%d, argv[0]=%s, argv[1]=%s, i=%d\n", tmp_fd, argv[0], argv[1], i);
    //overwrite ; or | or NULL with NULL to use the array as input for execve.
    //we are here in the child so it has no impact in the parent process.
    argv[i] = NULL; // le ponemos un NULL donde termina para que no le pasemos nada mas. Si se lo pusieramos en el padre nos cargariamos el parseo
    dup2(tmp_fd, 0); // donde se printa por defecto sera el temp_fd -> agora leio do pipe que foi aberto em pipe(fd); 
    close(tmp_fd);
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
    int tmp_fd;
    int pid;
    (void)argc; // is needed in exam, because the exam tester compiles with -Wall -Wextra -Werror
    i = 0;
    tmp_fd = dup(0);
    while (argv[i] && argv[i + 1]) //check if the end is reached
    {
        argv = &argv[i + 1];    //the new argv start after the ; or | ou do nome do programa
        i = 0;
        //count until we have all informations to execute the next child;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|")) //conta os argumentos que há até o fim da linha ou até ; ou |. Assim que encontra um dos três sai do while e executa
            i++;
        // printf("i=%d\n", i);
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
            if (pid == -1)
                return 1;
            if (pid == 0) //proceso filho
            {
                printf("comando pos pipe: ");
                printf("tmp_fd=%d\n", tmp_fd);
                if (ft_execute(argv, i, tmp_fd, env)) //por que preciso de um fork e não simplesmente executo o comando? Porque quando executo execve, ele cria um novo processo e tudo que vem depois dele não é executado, a menos que tenha um erro 
                    return (1);
            }
            else
            {
                // close(tmp_fd);
                waitpid(pid, NULL, WUNTRACED);//preciso desse wait para esperar o comando (do filho) ser executado antes de ir para o próximo 
                // tmp_fd = dup(0);
            }
        }
        else if(i != 0 && strcmp(argv[i], "|") == 0) //se encontrou um pipe
        {             //read from ========= write to
            pipe(fd); //fd[0] read end, fd[1] write end
            pid = fork();
            if (pid == 0)
            {
                printf("comando pre pipe: ");
                dup2(fd[1], 1);//agora imprimo no pipe
                close(fd[0]); //o processo filho não precisa ler nada
                close(fd[1]); //depois de dup2 agora posso fechar o fd[1]
                printf("tmp_fd=%d\n", tmp_fd);
                if (ft_execute(argv, i, tmp_fd, env)) //tmp_fd aqui é desnecessário
                    return (1); //se entrar aqui eh pq a função exec deu erro e retorna 1
            }
            else
            {
                close(fd[1]); //o processo pai não precisa escrever nada
                close(tmp_fd);
                tmp_fd = fd[0];
                //close(fd[0]); não funciona com isso, acho que porque temp_fd aponta para fd[0]
            }
        }
    }
    close(tmp_fd);
    return (0);
} //o segundo elseif vai executar o lado esquerdo do pipe