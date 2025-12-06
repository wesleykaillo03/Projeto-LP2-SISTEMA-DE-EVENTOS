#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "participants.h"

int load_participants(Participant list[], int *count, const char *filename) {
    FILE *f = fopen(filename, "r");
    *count = 0;

    if (!f) {
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        if (*count >= MAX_PARTICIPANTS) break;

        char *token = strtok(line, "|\n");
        if (!token) continue;
        list[*count].id = atoi(token);

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(list[*count].name, token, MAX_NAME);
        list[*count].name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        strncpy(list[*count].phone, token, MAX_PHONE);
        list[*count].phone[MAX_PHONE - 1] = '\0';

        token = strtok(NULL, "|\n");
        if (!token) continue;
        list[*count].checked_in = atoi(token);

        (*count)++;
    }

    fclose(f);
    return 0;
}

int save_participants(Participant list[], int count, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        return -1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(f, "%d|%s|%s|%d\n",
                list[i].id,
                list[i].name,
                list[i].phone,
                list[i].checked_in);
    }

    fclose(f);
    return 0;
}

int add_participant(Participant list[], int *count,
                    const char *name, const char *phone) {
    if (*count >= MAX_PARTICIPANTS) {
        return -1;
    }

    int new_id = 1;
    if (*count > 0) {
        new_id = list[*count - 1].id + 1;
    }

    Participant p;
    p.id = new_id;
    strncpy(p.name, name, MAX_NAME);
    p.name[MAX_NAME - 1] = '\0';

    strncpy(p.phone, phone, MAX_PHONE);
    p.phone[MAX_PHONE - 1] = '\0';

    p.checked_in = 0;

    list[*count] = p;
    (*count)++;

    return new_id;
}

int checkin_participant(Participant list[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) {
            list[i].checked_in = 1;
            return 0;
        }
    }
    return -1;
}

void compute_stats(Participant list[], int count,
                   int *total, int *presentes) {
    *total = count;
    *presentes = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].checked_in) {
            (*presentes)++;
        }
    }
}
