#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 8192

// Retorna o tipo MIME baseado na extensão
const char *mime_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";
    if (!strcasecmp(ext, ".html")) return "text/html";
    if (!strcasecmp(ext, ".css"))  return "text/css";
    if (!strcasecmp(ext, ".js"))   return "application/javascript";
    if (!strcasecmp(ext, ".jpg") || !strcasecmp(ext, ".jpeg")) return "image/jpeg";
    if (!strcasecmp(ext, ".png"))  return "image/png";
    if (!strcasecmp(ext, ".gif"))  return "image/gif";
    if (!strcasecmp(ext, ".txt"))  return "text/plain";
    return "application/octet-stream";
}

// Envia uma resposta de erro HTTP simples
void send_http_error(int client, int code, const char *message) {
    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/plain\r\n"
             "Connection: close\r\n\r\n"
             "%d %s\n",
             code, message, code, message);
    send(client, response, strlen(response), 0);
}

// Envia o conteúdo de um arquivo
void send_file(int client, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        send_http_error(client, 404, "Not Found");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char header[512];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n\r\n",
             mime_type(path), size);
    send(client, header, strlen(header), 0);

    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        send(client, buffer, bytes, 0);

    fclose(fp);
}

// Envia uma listagem de diretório como texto puro (sem HTML)
void send_directory_listing(int client, const char *dirpath, const char *reason) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        send_http_error(client, 500, "Internal Server Error");
        return;
    }

    char response[BUFFER_SIZE * 2];
    int offset = snprintf(response, sizeof(response),
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain; charset=utf-8\r\n\r\n"
                          "== Directory listing (%s) ==\n\n",
                          reason);

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        offset += snprintf(response + offset, sizeof(response) - offset,
                           "%s\n", ent->d_name);
        if (offset >= sizeof(response) - 100) break;
    }

    closedir(dir);
    send(client, response, strlen(response), 0);
}

// Loop principal do servidor
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <diretorio_base>\n", argv[0]);
        return 1;
    }

    const char *base_dir = argv[1];
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Servidor rodando na porta %d\n", PORT);

    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int client_fd = accept(server_fd, (struct sockaddr *)&client, &len);
        if (client_fd < 0) continue;

        char request[BUFFER_SIZE];
        int received = recv(client_fd, request, sizeof(request) - 1, 0);
        if (received <= 0) {
            close(client_fd);
            continue;
        }

        request[received] = '\0';
        printf("Requisição recebida:\n%s\n", request);

        char method[8], path[1024];
        sscanf(request, "%7s %1023s", method, path);

        if (strcmp(method, "GET") != 0) {
            send_http_error(client_fd, 405, "Method Not Allowed");
            close(client_fd);
            continue;
        }

        // Monta caminho completo
        char fullpath[2048];
        snprintf(fullpath, sizeof(fullpath), "%s%s", base_dir,
                 strcmp(path, "/") == 0 ? "" : path);

        struct stat st;
        if (stat(fullpath, &st) < 0) {
            // Se o cliente pediu /index.html e não existe, mostra a listagem
            if (strcmp(path, "/index.html") == 0)
                send_directory_listing(client_fd, base_dir, "index.html not found");
            else
                send_http_error(client_fd, 404, "Not Found");
        } else if (S_ISDIR(st.st_mode)) {
            // Se for diretório, tenta servir index.html
            char index_path[4096];
            snprintf(index_path, sizeof(index_path), "%s/index.html", fullpath);
            if (stat(index_path, &st) == 0)
                send_file(client_fd, index_path);
            else
                send_directory_listing(client_fd, fullpath, "directory view");
        } else {
            send_file(client_fd, fullpath);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
