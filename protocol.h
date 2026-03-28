#ifndef PROTOCOL_H
#define PROTOCOL_H

// Types of requests the client can make
typedef enum {
    REQ_PING,
    REQ_REGISTER_VOTER,
    REQ_REGISTER_CANDIDATE, // Added for Candidate Registration
    REQ_VIEW_CANDIDATES,    // Added for Viewing Candidates
    REQ_CAST_VOTE,
    REQ_VIEW_RESULTS
} RequestType;

// The package the Client sends to the Server
typedef struct {
    RequestType type;
    char student_id[15]; // Matches MAX_ID
    char name[50];       // Matches MAX_NAME
    int position_id;     // Used to send the 1-11 position choice
    int candidate_id;
} ClientRequest;

// The package the Server sends back to the Client
typedef struct {
    int success;          
    char message[4096];  // Upgraded to 4096 to hold the entire candidate table
} ServerResponse;

#define PORT 8080

#endif