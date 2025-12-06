    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>

    #define SERVER_PORT 5000
    #define SERVER_ADDR "127.0.0.1"

    void trim_newline(char *s) {
        s[strcspn(s, "\r\n")] = '\0';
    }

    int connect_to_server() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            return -1;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
        memset(&(server_addr.sin_zero), 0, 8);

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("connect");
            close(sock);
            return -1;
        }

        return sock;
    }

    void opcao_cadastrar() {
        char nome[128];
        char telefone[64];

        printf("Nome: ");
        fflush(stdout);
        if (!fgets(nome, sizeof(nome), stdin)) return;
        trim_newline(nome);

        printf("Telefone: ");
        fflush(stdout);
        if (!fgets(telefone, sizeof(telefone), stdin)) return;
        trim_newline(telefone);

        int sock = connect_to_server();
        if (sock < 0) {
            printf("Nao foi possivel conectar ao servidor.\n");
            return;
        }

        char comando[256];
        snprintf(comando, sizeof(comando), "ADD|%s|%s\n", nome, telefone);

        send(sock, comando, strlen(comando), 0);

        char resposta[256];
        memset(resposta, 0, sizeof(resposta));
        ssize_t n = recv(sock, resposta, sizeof(resposta) - 1, 0);
        if (n > 0) {
            resposta[n] = '\0';
            printf("Resposta do servidor: %s
", resposta);
        } else {
            printf("Nenhuma resposta do servidor.\n");
        }

        close(sock);
    }

    void opcao_listar() {
        int sock = connect_to_server();
        if (sock < 0) {
            printf("Nao foi possivel conectar ao servidor.\n");
            return;
        }

        const char *comando = "LIST\n";
        send(sock, comando, strlen(comando), 0);

        char buffer[256];
        printf("Participantes cadastrados:\n");
        while (1) {
            ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            printf("%s", buffer);
        }

        printf("\n");
        close(sock);
    }

    void opcao_checkin() {
        char input[32];
        printf("ID do participante para check-in: ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) return;
        trim_newline(input);

        int sock = connect_to_server();
        if (sock < 0) {
            printf("Nao foi possivel conectar ao servidor.\n");
            return;
        }

        char comando[64];
        snprintf(comando, sizeof(comando), "CHECKIN|%s\n", input);

        send(sock, comando, strlen(comando), 0);

        char resposta[256];
        memset(resposta, 0, sizeof(resposta));
        ssize_t n = recv(sock, resposta, sizeof(resposta) - 1, 0);
        if (n > 0) {
            resposta[n] = '\0';
            printf("Resposta do servidor: %s
", resposta);
        } else {
            printf("Nenhuma resposta do servidor.\n");
        }

        close(sock);
    }

    void opcao_stats_em_desenvolvimento() {
        printf("Opcao de estatisticas ainda em desenvolvimento neste checkpoint.\n");
        printf("O comando STATS ja existe no servidor e pode ser testado via nc/telnet.\n");
    }

    int main(void) {
        while (1) {
            printf("\n==== SISTEMA DE INSCRICOES (CLIENTE) ====\n");
            printf("1 - Cadastrar participante\n");
            printf("2 - Listar participantes\n");
            printf("3 - Fazer check-in\n");
            printf("4 - Ver estatisticas (em desenvolvimento)\n");
            printf("0 - Sair\n");
            printf("Escolha uma opcao: ");

            char opcao[8];
            if (!fgets(opcao, sizeof(opcao), stdin)) {
                break;
            }
            trim_newline(opcao);

            if (strcmp(opcao, "1") == 0) {
                opcao_cadastrar();
            } else if (strcmp(opcao, "2") == 0) {
                opcao_listar();
            } else if (strcmp(opcao, "3") == 0) {
                opcao_checkin();
            } else if (strcmp(opcao, "4") == 0) {
                opcao_stats_em_desenvolvimento();
            } else if (strcmp(opcao, "0") == 0) {
                printf("Saindo...\n");
                break;
            } else {
                printf("Opcao invalida.\n");
            }
        }

        return 0;
    }
