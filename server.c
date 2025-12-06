#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "participants.h"

#define PORT 5000
#define DATA_FILE "participantes.txt"

Participant participants[MAX_PARTICIPANTS];
int participant_count = 0;

void handle_client(int client_sock);
void handle_add(int client_sock, char *name, char *phone);
void handle_list(int client_sock);
void handle_checkin(int client_sock, int id);
void handle_stats(int client_sock);

int main(void) {
    if (load_participants(participants, &participant_count, DATA_FILE) != 0) {
        fprintf(stderr, "Erro ao carregar participantes.\n");
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_sock, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        return 1;
    }

    printf("Servidor rodando na porta %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);

        int client_sock = accept(server_sock,
                                 (struct sockaddr *)&client_addr,
                                 &client_size);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_sock);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}

void handle_client(int client_sock) {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        return;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (strncmp(buffer, "ADD|", 4) == 0) {
        char *rest = buffer + 4;
        char *name = strtok(rest, "|");
        char *phone = strtok(NULL, "|");

        if (!name || !phone) {
            const char *resp = "ERR|Comando ADD invalido\n";
            send(client_sock, resp, strlen(resp), 0);
            return;
        }

        handle_add(client_sock, name, phone);

    } else if (strcmp(buffer, "LIST") == 0) {
        handle_list(client_sock);

    } else if (strncmp(buffer, "CHECKIN|", 8) == 0) {
        char *rest = buffer + 8;
        int id = atoi(rest);
        if (id <= 0) {
            const char *resp = "ERR|ID invalido\n";
            send(client_sock, resp, strlen(resp), 0);
            return;
        }
        handle_checkin(client_sock, id);

    } else if (strcmp(buffer, "STATS") == 0) {
        handle_stats(client_sock);

    } else {
        const char *resp = "ERR|Comando nao reconhecido\n";
        send(client_sock, resp, strlen(resp), 0);
    }
}

void handle_add(int client_sock, char *name, char *phone) {
    int id = add_participant(participants, &participant_count, name, phone);
    if (id < 0) {
        const char *resp = "ERR|Nao foi possivel adicionar participante\n";
        send(client_sock, resp, strlen(resp), 0);
        return;
    }

    if (save_participants(participants, participant_count, DATA_FILE) != 0) {
        const char *resp = "ERR|Falha ao salvar no arquivo\n";
        send(client_sock, resp, strlen(resp), 0);
        return;
    }

    char resp[64];
    snprintf(resp, sizeof(resp), "OK|%d\n", id);
    send(client_sock, resp, strlen(resp), 0);
}

void handle_list(int client_sock) {
    char line[256];

    for (int i = 0; i < participant_count; i++) {
        snprintf(line, sizeof(line), "ITEM|%d|%s|%s|%d\n",
                 participants[i].id,
                 participants[i].name,
                 participants[i].phone,
                 participants[i].checked_in);
        send(client_sock, line, strlen(line), 0);
    }

    const char *end = "END\n";
    send(client_sock, end, strlen(end), 0);
}

void handle_checkin(int client_sock, int id) {
    if (checkin_participant(participants, participant_count, id) != 0) {
        const char *resp = "ERR|Participante nao encontrado\n";
        send(client_sock, resp, strlen(resp), 0);
        return;
    }

    if (save_participants(participants, participant_count, DATA_FILE) != 0) {
        const char *resp = "ERR|Falha ao salvar no arquivo\n";
        send(client_sock, resp, strlen(resp), 0);
        return;
    }

    const char *resp = "OK\n";
    send(client_sock, resp, strlen(resp), 0);
}

void handle_stats(int client_sock) {
    int total = 0;
    int presentes = 0;
    compute_stats(participants, participant_count, &total, &presentes);

    char resp[64];
    snprintf(resp, sizeof(resp), "STATS|%d|%d\n", total, presentes);
    send(client_sock, resp, strlen(resp), 0);
}
