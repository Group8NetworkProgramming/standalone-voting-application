#ifndef VOTER_H
#define VOTER_H

#define MAX_NAME     50
#define MAX_ID       15
#define MAX_POSITION 50
#define NUM_POSITIONS 11

// Voter struct
typedef struct {
    char student_id[MAX_ID];
    char name[MAX_NAME];
    int  has_voted; 
    int  votes_cast;
} Voter;

// Candidate struct
typedef struct {
    char student_id[MAX_ID];
    char name[MAX_NAME];
    char position[MAX_POSITION];
    int  votes;          
} Candidate;

/* VOTER FUNCTIONS — voter.c */
void register_voter();
void display_voters();
int  is_id_registered(const char *id);

/* CANDIDATE FUNCTIONS — candidate.c */
void register_candidate();
void display_candidates();
int  is_candidate_registered(const char *id);

/* VOTING FUNCTION — voting.c */
void cast_vote();

/* RESULTS FUNCTIONS — results.c */
void tally_votes();
void announce_results();

#endif


