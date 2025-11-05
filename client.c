#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 8192

// Extrai host, porta e caminho da URL informada
void parse_url(const char *url, char *host, char *path, int *port) {
    *port = 80; // porta padrão HTTP

    if (strncmp(url, "http://", 7) == 0)
        url += 7; // ignora o prefixo http://

    const char *path_start = strchr(url, '/');
    const char *port_start = strchr(url, ':');

    // host + porta
    if (port_start && (!path_start || port_start < path_start)) {
        int host_len = port_start - url;
        strncpy(host, url, host_len);
        host[host_len] = '\0';
        *port = atoi(port_start + 1);
    } else {
        if (path_start) {
            int host_len = path_start - url;
            strncpy(host, url, host_len);
            host[host_len] = '\0';
        } else {
            strcpy(host, url);
        }
    }

    // caminho
    if (path_start)
        strcpy(path, path_start);
    else
        strcpy(path, "/");
}

// Cria e conecta o socket TCP ao servidor
int connect_to_host(const char *host, int port) {
    struct addrinfo hints, *res, *p;
    char port_str[10];
    int sock;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    sprintf(port_str, "%d", port);
    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    for (p = res; p; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0)
            continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sock);
    }

    freeaddrinfo(res);
    return p ? sock : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s http://host[:porta]/arquivo\n", argv[0]);
        return 1;
    }

    char host[256], path[1024];
    int port;
    parse_url(argv[1], host, path, &port);

    printf("Conectando em %s:%d...\n", host, port);
    int sock = connect_to_host(host, port);
    if (sock < 0) {
        fprintf(stderr, "Falha ao conectar.\n");
        return 1;
    }

    // Monta a requisição HTTP
    char request[2048];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: simple-client/1.0\r\n"
             "Connection: close\r\n\r\n",
             path, host);

    send(sock, request, strlen(request), 0);

    // Recebe resposta
    char buffer[BUFFER_SIZE];
    int bytes;
    int header_done = 0;
    FILE *file = NULL;
    int is_text_plain = 0; // <== NOVA FLAG
    char content_type[128] = "";

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';

        if (!header_done) {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                header_done = 1;
                int header_len = body - buffer + 4;

                // Mostra o cabeçalho
                printf("===== Resposta do Servidor =====\n");
                printf("%.*s\n", header_len, buffer);

                // Captura código de status
                int code = 0;
                sscanf(buffer, "HTTP/%*s %d", &code);
                if (code != 200) {
                    printf("Servidor respondeu com código %d, abortando.\n", code);
                    close(sock);
                    return 1;
                }

                // Detecta o tipo de conteúdo
                char *ctype = strcasestr(buffer, "Content-Type:");
                if (ctype) {
                    sscanf(ctype, "Content-Type: %127[^\r\n]", content_type);
                    if (strstr(content_type, "text/plain")) {
                        is_text_plain = 1;
                    }
                }

                if (is_text_plain) {
                    // Mostra o corpo diretamente no terminal
                    printf("===== Corpo da resposta =====\n");
                    printf("%s", body + 4);
                } else {
                    // Salva o arquivo normalmente
                    const char *slash = strrchr(path, '/');
                    const char *filename = (slash && *(slash + 1)) ? slash + 1 : "index.html";
                    file = fopen(filename, "wb");
                    if (!file) {
                        perror("fopen");
                        close(sock);
                        return 1;
                    }
                    fwrite(body + 4, 1, bytes - header_len, file);
                }
            }
        } else {
            if (is_text_plain)
                printf("%s", buffer);
            else if (file)
                fwrite(buffer, 1, bytes, file);
        }
    }

    if (file) {
        fclose(file);
        printf("\nArquivo salvo com sucesso!\n");
    }

    close(sock);
    return 0;
}
