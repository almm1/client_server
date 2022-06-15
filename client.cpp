#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <cstdio>

using namespace std;

int main()
{
    //	Создание сокета
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Ошибка сокета: ");
        return 0;
    }

    int port = 9000;
    string ip_address = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);

    //	Подключение к серверу
    int connectRes = connect(sock, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        perror("Ошибка подключения к серверу: ");
        return 0;
    }

    char recv_buf[4096]; // буфер для ответа от сервера

    string file_line;            // переменная для считанной строки
    ifstream file("source.txt"); // файл для чтения
    if (file.is_open())
    {
        while (getline(file, file_line))
        {
            int send_res = send(sock, file_line.c_str(), file_line.size() + 1, 0);
            if (send_res == 0) // если пустая строка ничего не отправлять
                continue;

            memset(recv_buf, 0, 4096);

            int receiv = recv(sock, recv_buf, 4096, 0);
            if (receiv == -1)
                cout << "Ошибка получения ответа от сервера\r\n";
            else
                cout << "Сервер получил строку: " << string(recv_buf, receiv) << "\r\n";
        }
        file.close();
    }
    else
        cout << "Файл не найден";

    close(sock);
    exit(0);
    return 0;
}