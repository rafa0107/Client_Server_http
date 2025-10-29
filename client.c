#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 4096

// ---- Funções auxiliares ----
void parse_url(const char *url, char *host, char *path, int *port);
int connect_to_server(const char *host, int port);
void send_http_request(int sock, const char *host, const char *path);
void receive_response(int sock, const char *path);

// ---- Função principal ----
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s http://host[:porta]/caminho\n", argv[0]);
        return 1;
    }

    char host[256], path[1024];
    int port;

    parse_url(argv[1], host, path, &port);

    int sock = connect_to_server(host, port);
    if (sock < 0) {
        fprintf(stderr, "Erro ao conectar em %s:%d\n", host, port);
        return 1;
    }

    send_http_request(sock, host, path);
    receive_response(sock, path);

    close(sock);
    return 0;
}

// ---- Implementações ----

// Extrai host, porta e caminho da URL
void parse_url(const char *url, char *host, char *path, int *port) {
    *port = 80; // porta padrão HTTP
    strcpy(path, "/");

    if (strncmp(url, "http://", 7) == 0) {
        url += 7;
    }

    const char *p = strchr(url, '/');
    if (p) {
        strncpy(host, url, p - url);
        host[p - url] = '\0';
        strcpy(path, p);
    } else {
        strcpy(host, url);
    }

    // Verifica se há porta (ex: localhost:8080)
    char *port_ptr = strchr(host, ':');
    if (port_ptr) {
        *port_ptr = '\0';
        *port = atoi(port_ptr + 1);
    }
}

// Conecta ao servidor (TCP)
int connect_to_server(const char *host, int port) {
    struct addrinfo hints, *res, *p;
    char port_str[10];
    int sock;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    sprintf(port_str, "%d", port);
    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;

        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0)
            break; // sucesso

        close(sock);
    }

    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "Não foi possível conectar ao host\n");
        return -1;
    }

    return sock;
}

// Envia requisição HTTP GET
void send_http_request(int sock, const char *host, const char *path) {
    char request[2048];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "User-Agent: meu_navegador/1.0\r\n"
             "\r\n",
             path, host);

    send(sock, request, strlen(request), 0);
}

// Recebe a resposta e salva o conteúdo em arquivo
void receive_response(int sock, const char *path) {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    FILE *file = NULL;
    int header_done = 0;
    char *body_start;
    char filename[256] = "index.html";

    // Extrai nome do arquivo a partir do caminho
    const char *slash = strrchr(path, '/');
    if (slash && *(slash + 1) != '\0')
        strcpy(filename, slash + 1);

    // Cria arquivo para salvar o conteúdo
    file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return;
    }

    while ((bytes_read = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        if (!header_done) {
            // Procura fim dos cabeçalhos HTTP
            body_start = strstr(buffer, "\r\n\r\n");
            if (body_start) {
                header_done = 1;

                // Mostra cabeçalho
                *body_start = '\0';
                printf("%s\n\n", buffer);

                // Verifica se o status é 200
                if (strstr(buffer, "200") == NULL) {
                    printf("Erro: resposta não é 200 OK.\n");
                    fclose(file);
                    remove(filename);
                    return;
                }

                // Escreve o corpo (parte após os cabeçalhos)
                body_start += 4;
                int body_len = bytes_read - (body_start - buffer);
                fwrite(body_start, 1, body_len, file);
            }
        } else {
            fwrite(buffer, 1, bytes_read, file);
        }
    }

    fclose(file);
    printf("Arquivo salvo como: %s\n", filename);
}
