#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#define MAX_NAME 100
#define MAX_PHONE 30
#define MAX_PARTICIPANTS 1000

typedef struct {
    int id;
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    int checked_in; // 0 = não, 1 = sim
} Participant;

int load_participants(Participant list[], int *count, const char *filename);
int save_participants(Participant list[], int count, const char *filename);
int add_participant(Participant list[], int *count,
                    const char *name, const char *phone);
int checkin_participant(Participant list[], int count, int id);
void compute_stats(Participant list[], int count,
                   int *total, int *presentes);

#endif
