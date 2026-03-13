#ifndef VOTER_H
#define VOTER_H

#define MAX_NAME 50
#define MAX_ID 15

typedef struct {
    char student_id[MAX_ID];
    char name[MAX_NAME];
    int has_voted; // 0 for No, 1 for Yes
} Voter;

// Function prototypes
void register_voter();
void display_voters();
int is_id_registered(const char* id);

#endif