#ifndef VOTER_H
#define VOTER_H

#define MAX_NAME 50
#define MAX_ID 15
#define MAX_POSITION 50 // Added for candidate positions

// Existing Voter Struct
typedef struct {
    char student_id[MAX_ID];
    char name[MAX_NAME];
    int has_voted; // 0 for No, 1 for Yes
} Voter;

// New Candidate Struct
typedef struct {
    char student_id[MAX_ID];
    char name[MAX_NAME];
    char position[MAX_POSITION];
} Candidate;

// Function prototypes
void register_voter();
void display_voters();
int is_id_registered(const char* id);

// New Candidate Prototypes
void register_candidate();
int is_candidate_registered(const char* id);

// Add this new prototype
void display_candidates();

#endif


