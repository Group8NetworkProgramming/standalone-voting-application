#ifndef PROTOCOL_H
#define PROTOCOL_H

// Types of requests the client can make
typedef enum {
    REQ_PING,
    REQ_REGISTER_VOTER,
    REQ_REGISTER_CANDIDATE,
    REQ_VIEW_CANDIDATES,
    REQ_CAST_VOTE,              // Send one vote: voter_id + position_id + candidate_id
    REQ_GET_VOTER_STATUS,       // Check voter's bitmask / session state
    REQ_GET_CANDIDATES_FOR_POS, // Request candidate list for one specific position
    REQ_VIEW_RESULTS
} RequestType;

// The package the Client sends to the Server
typedef struct {
    RequestType type;
    char student_id[15];  // Voter or candidate ID
    char name[50];
    int  position_id;     // 1–11
    int  candidate_index; // 1-based index of the chosen candidate for this position
} ClientRequest;

// The package the Server sends back to the Client
typedef struct {
    int  success;
    int  votes_cast;       // Bitmask — used by REQ_GET_VOTER_STATUS
    int  candidate_count;  // How many candidates came back for a position
    char message[4096];    // Human-readable text / candidate list
} ServerResponse;

#define PORT 8080

#endif