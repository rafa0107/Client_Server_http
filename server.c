#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096
#define PORT 5050

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s /caminho/do/diretorio\n", argv[0]);
        return 1;
    }

    char *base_dir = argv[1];
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { perror("bind"); exit(1); }

    listen(server_fd, 5);
    printf("Servidor HTTP rodando na porta %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        int bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) { close(client_fd); continue; }
        buffer[bytes] = '\0';

        char method[8], path[1024];
        sscanf(buffer, "%s %s", method, path);

        char file_path[2048];
        if (strcmp(path, "/") == 0)
            snprintf(file_path, sizeof(file_path), "%s/index.html", base_dir);
        else
            snprintf(file_path, sizeof(file_path), "%s%s", base_dir, path);

        FILE *file = fopen(file_path, "rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            char header[256];
            snprintf(header, sizeof(header),
                     "HTTP/1.0 200 OK\r\n"
                     "Content-Length: %ld\r\n"
                     "\r\n", file_size);
            send(client_fd, header, strlen(header), 0);

            while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                send(client_fd, buffer, bytes, 0);
            }
            fclose(file);
        } else if (strcmp(path, "/") == 0) {
            // --- Lista arquivos do diretório ---
            DIR *dir = opendir(base_dir);
            if (!dir) {
                const char *msg = "HTTP/1.0 500 Internal Server Error\r\n\r\nErro ao abrir diretório\n";
                send(client_fd, msg, strlen(msg), 0);
            } else {
                char list[BUFFER_SIZE];
                list[0] = '\0';
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_name[0] != '.') { // ignora . e ..
                        strcat(list, entry->d_name);
                        strcat(list, "\n");
                    }
                }
                closedir(dir);

                char header[256];
                snprintf(header, sizeof(header),
                         "HTTP/1.0 200 OK\r\n"
                         "Content-Length: %ld\r\n"
                         "Content-Type: text/plain\r\n"
                         "\r\n", strlen(list));
                send(client_fd, header, strlen(header), 0);
                send(client_fd, list, strlen(list), 0);
            }
        } else {
            const char *msg = "HTTP/1.0 404 Not Found\r\n\r\nArquivo não encontrado\n";
            send(client_fd, msg, strlen(msg), 0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
