# 🌐 Cliente-Servidor HTTP em Baixo Nível (C / POSIX Sockets)

Este projeto implementa uma arquitetura básica cliente-servidor usando o protocolo HTTP, escrita inteiramente em C.
Ele serve como um recurso educacional para demonstrar os conceitos fundamentais de programação de redes, comunicação via socket e manipulação de requisições/respostas HTTP.

O projeto foi projetado para ser simples e fácil de entender, tornando-se adequado para estudantes e iniciantes que estão aprendendo sobre programação de redes.

O componente cliente é responsável por enviar requisições HTTP ao servidor, enquanto o servidor escuta conexões e processa essas requisições.
O foco está em demonstrar funcionalidades essenciais do HTTP, como lidar com requisições GET e enviar respostas básicas.

Ele oferece uma base para construir aplicações de rede mais complexas e explorar recursos avançados do protocolo HTTP.

As principais tecnologias utilizadas incluem:

Linguagem de programação C
Programação de sockets (usando bibliotecas padrão de C)
Implementação básica do protocolo HTTP
A arquitetura é direta, com clara separação entre as funcionalidades de cliente e servidor.

Este projeto oferece uma abordagem prática para entender como o HTTP funciona em baixo nível, fornecendo uma visão valiosa sobre os princípios da comunicação em rede.

---

## 🧩 Sobre o Projeto

Este ecossistema em baixo nível é composto por dois módulos independentes que se comunicam via rede utilizando o modelo **TCP/IP**:

1. **O Servidor (`server.c`):** Atua como um servidor web estático. Ele abre uma porta de escuta no sistema operacional, aguarda conexões de clientes, interpreta requisições HTTP brutas, manipula o sistema de arquivos local para buscar o recurso e devolve uma resposta HTTP estruturada com os cabeçalhos apropriados.
2. **O Cliente (`client.c`):** Funciona de forma análoga a um navegador web via linha de comando. Ele recebe uma URL, resolve o endereço de rede do servidor, estabelece uma conexão de transporte estável, solicita o recurso e decide se deve renderizá-lo diretamente no terminal ou salvá-lo como arquivo binário com base nos metadados recebidos.

---

## 🎯 Destaques Técnicos (Infra & Redes)

Em relação à conceitos técnicos de infraestrutura e redes, o projeta conta com:

* **Gerenciamento de Sockets POSIX:** Utilização ativa das chamadas de sistema fundamentais da API de redes do Linux:
  * `socket()` para inicializar os descritores de arquivos de rede (Endpoints).
  * `bind()` para acoplar e reservar o processo do servidor à interface de rede e porta local.
  * `listen()` para definir a fila de conexões TCP passivas aguardando processamento.
  * `accept()` e `connect()` para consolidar o *Three-Way Handshake* do protocolo de transporte TCP.
* **Resolução Dinâmica de Nomes (DNS):** Integração com a função `getaddrinfo()`, permitindo que o cliente resolva tanto IPs estáticos quanto domínios textuais (`localhost`, `google.com`, etc.) de forma transparente.
* **Leitura Dinâmica de Content-Type (MIME Types):** O servidor realiza a varredura por meio de `stat()` e checagem de extensões de arquivo, sendo capaz de identificar e injetar dinamicamente no cabeçalho HTTP o tipo exato de mídia trafegada. Possui suporte nativo e tratamento binário para:
  * Páginas e Estilos: `text/html`, `text/css`, `application/javascript`
  * Imagens e Mídias: `image/jpeg`, `image/png`, `image/gif`
  * Dados e Texto Puro: `text/plain`, `application/octet-stream`
* **Parser de Fluxo de Dados:** Implementação manual de varredura de buffers de memória (com `strstr` e `sscanf`) para separação estrita entre o cabeçalho do protocolo (`\r\n\r\n`) e o corpo do dado binário (`payload`), evitando corrupção de arquivos em trânsito.

---

## 🚀 Início Rápido

Clone e execute em alguns passos (certifique-se de estar em um ambiente Linux ou WSL):

```bash
# Clonar o repositório
git clone [https://github.com/rafa0107/Client_Server_http.git](https://github.com/rafa0107/Client_Server_http.git)
cd Client_Server_http

# Compilar o código automaticamente via Makefile
make

# Criar uma pasta de testes simulando o servidor público
mkdir test_files
echo "Servidor HTTP rodando com sucesso!" > test_files/teste.txt

# Iniciar o servidor passando o diretório base
./server ./test_files

# (Em outro terminal) Execute o cliente para ler o arquivo de texto
./client http://localhost:8080/teste.txt




## 🚀 Início Rápido

Clone e execute em alguns passos (certifique-se de estar em um ambiente Linux ou WSL):

```bash
# Clonar o repositório
git clone [https://github.com/rafa0107/Client_Server_http.git](https://github.com/rafa0107/Client_Server_http.git)
cd Client_Server_http

# Compilar o código
make

# Criar uma pasta de testes e iniciar o servidor
mkdir www
echo "Hello World!" > www/index.html
./server ./www

# Em outro terminal, execute o cliente para baixar o arquivo
./client http://localhost:8080/index.html
```

## 📦 Instalação

Pré-requisitos
 - Um compilador C (ex: GCC instalado no Linux/WSL)
 - Ferramenta Make

##  Compilação

# Clonar o repositório
git clone [https://github.com/rafa0107/Client_Server_http.git](https://github.com/rafa0107/Client_Server_http.git)
cd Client_Server_http

# Compilar o cliente e o servidor automaticamente via Makefile
make

##  💻 Uso

# Executando o Servidor

O servidor precisa receber como argumento o diretório base de onde ele irá ler e servir os arquivos públicos. Por padrão, ele escuta na porta 8080.

```bash
./server <diretorio_base>
Exemplo : ./server ./www
```

# Executando o Cliente

O cliente espera receber a URL HTTP completa do arquivo que você deseja baixar ou visualizar.

```bash
./client http://<host>:<porta>/<arquivo>

Exemplo: 

# Para visualizar arquivos de texto (.txt) diretamente no terminal:
./client http://localhost:8080/teste.txt

# Para baixar arquivos binários ou páginas (HTML, PNG, GIF), que serão salvos localmente:
./client http://localhost:8080/exemplo.png
```
## ⚙️ Configuração

Este projeto não utiliza arquivos de configuração externos ou variáveis de ambiente.

# Porta do Servidor: Fixada via código (#define PORT 8080 no arquivo server.c).

# Tamanho do Buffer: Fixado em 8192 bytes (#define BUFFER_SIZE 8192 em ambos os arquivos) para otimização de leitura de rede.

# Parâmetros Dinâmicos: Passados exclusivamente via argumentos de linha de comando no momento da execução (diretório para o servidor e URL para o cliente).