#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <sys/wait.h>
#include <fstream>

using namespace std;

int daemon(void);

int main()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        printf("\ncan't fork");
        exit(1);
    }
    else if (pid != 0)
        exit(0);
    setsid();
    daemon();
    return 0;
}

ofstream out("Result.txt", ios::out);
int listening;
int client_socket;

void closing()
{
    close(client_socket);
    out.close();
    exit(0);
}

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGHUP:
        close(listening);
        closing();
        break;
    case SIGTERM:
        close(listening);
        closing();
        break;
    case SIGCHLD:
        pid_t pid;
        int stat;
        while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        {
        }
        break;
    }
}

int daemon()
{
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        perror("Ошибка сокета: ");
        return 0;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(9000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    bind(listening, (sockaddr *)&hint, sizeof(hint));
    listen(listening, SOMAXCONN);

    sockaddr_in client;
    socklen_t client_size = sizeof(client);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    char recv_buf[4096];

    while (true)
    {
        memset(recv_buf, 0, 4096);
        signal(SIGCHLD, signal_handler);

        client_socket = accept(listening, (sockaddr *)&client, &client_size);
        if (client_socket > 0)
        {
            if (fork() == 0)
            {
                do
                {
                    int receiv = recv(client_socket, recv_buf, 4096, 0);
                    if (receiv == 0)
                    {
                        closing();
                        break;
                    }

                    if (out.is_open())
                    {
                        out << recv_buf << endl;
                    }

                    send(client_socket, recv_buf, receiv + 1, 0);
                } while (true);
                out.close();
            }
        }
    }
    close(listening);
    return 0;
}