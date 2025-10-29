# 🌐 Cliente-Servidor HTTP

Uma aplicação simples de cliente-servidor demonstrando comunicação HTTP em C.  
Uma implementação básica de um modelo cliente-servidor usando o protocolo HTTP, escrita em C para fins educacionais.

---

## 📋 Índice

- [Sobre](#sobre)
- [Recursos](#recursos)
- [Início Rápido](#início-rápido)
- [Instalação](#instalação)
- [Uso](#uso)
- [Configuração](#configuração)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Testes](#testes)
- [FAQ](#faq)
- [Licença](#licença)


---

## 🧩 Sobre

Este projeto implementa uma arquitetura básica cliente-servidor usando o protocolo **HTTP**, escrita inteiramente em **C**.  
Ele serve como um recurso educacional para demonstrar os conceitos fundamentais de **programação de redes**, **comunicação via socket** e **manipulação de requisições/respostas HTTP**.

O projeto foi projetado para ser simples e fácil de entender, tornando-se adequado para estudantes e iniciantes que estão aprendendo sobre programação de redes.

O componente **cliente** é responsável por enviar requisições HTTP ao servidor, enquanto o **servidor** escuta conexões e processa essas requisições.  
O foco está em demonstrar funcionalidades essenciais do HTTP, como lidar com requisições **GET** e enviar respostas básicas.

Ele oferece uma base para construir aplicações de rede mais complexas e explorar recursos avançados do protocolo HTTP.

As principais tecnologias utilizadas incluem:
- Linguagem de programação **C**
- **Programação de sockets** (usando bibliotecas padrão de C)
- Implementação básica do **protocolo HTTP**

A arquitetura é direta, com clara separação entre as funcionalidades de cliente e servidor.

Este projeto oferece uma abordagem prática para entender como o HTTP funciona em baixo nível, fornecendo uma visão valiosa sobre os princípios da comunicação em rede.

---

## ✨ Recursos

- 🎯 **Manipulação básica de requisições HTTP:** Suporte para requisições HTTP do tipo `GET`.
- ⚡ **Comunicação via Socket:** Utiliza programação de sockets para comunicação entre cliente e servidor.
- 🔒 **Segurança básica:** Demonstra considerações básicas de segurança em programação de rede.
- 🛠️ **Educacional:** Projetado para aprendizado sobre arquitetura cliente-servidor e protocolo HTTP.

---

## 🚀 Início Rápido

Clone e execute em 3 passos:

```bash
git clone https://github.com/rafa0107/Client_Server_http.git
cd Client_Server_http
make
```

---

## 📦 Instalação

### Pré-requisitos

- Um compilador C (ex: GCC)
- Make

### A partir do código-fonte

```bash
# Clonar o repositório
git clone https://github.com/rafa0107/Client_Server_http.git
cd Client_Server_http

# Compilar o código
make
```

---

## 💻 Uso

### Executando o Servidor

```bash
./server [porta]
```
Substitua `[porta]` pelo número da porta em que o servidor deve escutar (ex: `8080`).

### Executando o Cliente

```bash
./client [ip_servidor] [porta] [caminho]
```
Parâmetros:

- `[ip_servidor]`: Endereço IP do servidor. Use `localhost` ou `127.0.0.1` se o servidor estiver na mesma máquina.  
- `[porta]`: A porta na qual o servidor está escutando.  
- `[caminho]`: O caminho solicitado ao servidor (ex: `/index.html`).  

Exemplo:

```bash
./client localhost 8080 /index.html
```

---

## ⚙️ Configuração

Este projeto **não utiliza arquivos de configuração**.  
Os parâmetros são passados diretamente via linha de comando.

---

## 📁 Estrutura do Projeto

Client_Server_http/
├── client.c # Código do cliente
├── server.c # Código do servidor
├── Makefile # Instruções de compilação
├── README.md # Documentação do projeto
└── screenshots/ # Imagens para demonstração

## 🧪 Testes

Para testar a aplicação:

1. Compile o código usando `make`.  
2. Execute o servidor em um terminal.  
3. Execute o cliente em outro terminal, enviando requisições para o servidor.  
4. Verifique se o servidor recebe as requisições e envia as respostas corretamente.

---

## ❓ FAQ

**Q:** Como alterar a porta que o servidor escuta?  
**A:** Passe o número da porta como argumento ao executar o servidor (ex: `./server 8080`).

**Q:** Como especificar o IP do servidor ao executar o cliente?  
**A:** Informe o IP como primeiro argumento (ex: `./client 127.0.0.1 8080 /`).

---

## 📄 Licença

Este projeto está licenciado sob a **Licença MIT** – veja o arquivo `LICENSE` para detalhes.

### Resumo da Licença

| Recurso        | Status |
|----------------|:------:|
| Uso comercial  | ✅     |
| Modificação    | ✅     |
| Distribuição   | ✅     |
| Uso privado    | ✅     |
| Responsabilidade | ❌   |
| Garantia       | ❌     |

---


