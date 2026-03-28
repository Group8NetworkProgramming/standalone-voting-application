#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "protocol.h"
#include "voter.h"

/* ------------------------------------------------------------------ */
/*  HELPER: positions table                                             */
/* ------------------------------------------------------------------ */
static const char *positions[] = {
    "",
    "Chairman", "Vice Chairman", "Secretary General",
    "Organizing Secretary", "Secretary for Finance",
    "Secretary for Academic Affairs", "Secretary for Catering and Accommodation",
    "Secretary for Legal Affairs", "Secretary for Gender Affairs",
    "Secretary for Special Needs", "Campus/Faculty Representatives"
};

/* ------------------------------------------------------------------ */
/*  HELPER: duplicate checkers                                          */
/* ------------------------------------------------------------------ */
int debug_check_duplicate(const char *target_id) {
    FILE *fp = fopen("voters.dat", "rb");
    if (!fp) return 0;
    Voter temp;
    while (fread(&temp, sizeof(Voter), 1, fp)) {
        if (strcmp(temp.student_id, target_id) == 0) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

int debug_check_candidate_duplicate(const char *target_id) {
    FILE *fp = fopen("candidates.dat", "rb");
    if (!fp) return 0;
    Candidate temp;
    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        if (strcmp(temp.student_id, target_id) == 0) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  HANDLER: REQ_GET_VOTER_STATUS                                       */
/*  Looks up voter by ID, returns bitmask + has_voted in response.      */
/* ------------------------------------------------------------------ */
void handle_get_voter_status(ClientRequest *req, ServerResponse *res) {
    FILE *fp = fopen("voters.dat", "rb");
    if (!fp) {
        res->success = 0;
        strcpy(res->message, "Voter database not found.");
        return;
    }

    Voter v;
    int found = 0;
    while (fread(&v, sizeof(Voter), 1, fp)) {
        if (strcmp(v.student_id, req->student_id) == 0) { found = 1; break; }
    }
    fclose(fp);

    if (!found) {
        res->success = 0;
        strcpy(res->message, "Voter not registered.");
        return;
    }
    if (v.has_voted) {
        res->success = 0;
        strcpy(res->message, "You have already completed voting for all positions.");
        return;
    }

    res->success    = 1;
    res->votes_cast = v.votes_cast;
    sprintf(res->message, "Welcome, %s. Returning your session state.", v.name);
}

/* ------------------------------------------------------------------ */
/*  HANDLER: REQ_GET_CANDIDATES_FOR_POS                                 */
/*  Returns a numbered list of candidates for one position.             */
/* ------------------------------------------------------------------ */
void handle_get_candidates_for_pos(ClientRequest *req, ServerResponse *res) {
    if (req->position_id < 1 || req->position_id > NUM_POSITIONS) {
        res->success = 0;
        strcpy(res->message, "Invalid position ID.");
        return;
    }

    FILE *fp = fopen("candidates.dat", "rb");
    if (!fp) {
        res->success = 0;
        strcpy(res->message, "No candidates registered yet.");
        return;
    }

    Candidate temp;
    int count = 0;
    char line[200];

    sprintf(res->message, "--- Candidates for %s ---\n", positions[req->position_id]);

    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        if (strcmp(temp.position, positions[req->position_id]) == 0) {
            count++;
            sprintf(line, "%d. %s (%s)\n", count, temp.name, temp.student_id);
            strcat(res->message, line);
        }
    }
    fclose(fp);

    res->candidate_count = count;

    if (count == 0) {
        res->success = 0;
        strcat(res->message, "  [No candidates for this position]\n");
    } else {
        res->success = 1;
    }
}

/* ------------------------------------------------------------------ */
/*  HANDLER: REQ_CAST_VOTE                                              */
/*  req->student_id   = voter                                           */
/*  req->position_id  = 1–11                                            */
/*  req->candidate_index = 1-based choice among that position's list    */
/* ------------------------------------------------------------------ */
void handle_cast_vote(ClientRequest *req, ServerResponse *res) {
    /* --- 1. Validate position ------------------------------------ */
    if (req->position_id < 1 || req->position_id > NUM_POSITIONS) {
        res->success = 0;
        strcpy(res->message, "Invalid position ID.");
        return;
    }

    /* --- 2. Load voter ------------------------------------------- */
    FILE *vf = fopen("voters.dat", "rb+");
    if (!vf) {
        res->success = 0;
        strcpy(res->message, "Voter database missing.");
        return;
    }

    Voter voter;
    long  voter_offset = 0;
    int   found = 0;

    while (fread(&voter, sizeof(Voter), 1, vf)) {
        if (strcmp(voter.student_id, req->student_id) == 0) { found = 1; break; }
        voter_offset += sizeof(Voter);
    }

    if (!found) {
        fclose(vf);
        res->success = 0;
        strcpy(res->message, "Voter not registered.");
        return;
    }

    if (voter.has_voted) {
        fclose(vf);
        res->success = 0;
        strcpy(res->message, "You have already completed voting.");
        return;
    }

    /* Check if this position was already voted */
    if (voter.votes_cast & (1 << req->position_id)) {
        fclose(vf);
        res->success = 0;
        strcpy(res->message, "You have already voted for this position.");
        return;
    }

    /* --- 3. Load candidates and find the n-th match -------------- */
    FILE *cf = fopen("candidates.dat", "rb+");
    if (!cf) {
        fclose(vf);
        res->success = 0;
        strcpy(res->message, "Candidates database missing.");
        return;
    }

    Candidate candidates[200];
    int total = 0;
    while (fread(&candidates[total], sizeof(Candidate), 1, cf)) total++;
    fclose(cf);

    /* Build index of candidates for this position */
    int index[50], k = 0;
    for (int i = 0; i < total; i++) {
        if (strcmp(candidates[i].position, positions[req->position_id]) == 0)
            index[k++] = i;
    }

    if (k == 0) {
        /* No candidates — auto-mark position as done */
        voter.votes_cast |= (1 << req->position_id);
        fseek(vf, voter_offset, SEEK_SET);
        fwrite(&voter, sizeof(Voter), 1, vf);
        fclose(vf);
        res->success    = 1;
        res->votes_cast = voter.votes_cast;
        strcpy(res->message, "No candidates for this position. Automatically skipped.");
        return;
    }

    if (req->candidate_index < 1 || req->candidate_index > k) {
        fclose(vf);
        res->success = 0;
        strcpy(res->message, "Invalid candidate choice.");
        return;
    }

    /* --- 4. Increment vote --------------------------------------- */
    candidates[index[req->candidate_index - 1]].votes++;

    cf = fopen("candidates.dat", "wb");
    for (int i = 0; i < total; i++)
        fwrite(&candidates[i], sizeof(Candidate), 1, cf);
    fclose(cf);

    /* --- 5. Update voter bitmask --------------------------------- */
    voter.votes_cast |= (1 << req->position_id);

    /* Check if all positions are now done */
    int all_done = 1;
    for (int p = 1; p <= NUM_POSITIONS; p++) {
        if (!(voter.votes_cast & (1 << p))) { all_done = 0; break; }
    }
    if (all_done) voter.has_voted = 1;

    fseek(vf, voter_offset, SEEK_SET);
    fwrite(&voter, sizeof(Voter), 1, vf);
    fclose(vf);

    res->success    = 1;
    res->votes_cast = voter.votes_cast;

    if (all_done) {
        sprintf(res->message,
                "Vote recorded for %s. Voting complete — thank you!",
                positions[req->position_id]);
    } else {
        sprintf(res->message,
                "Vote recorded for %s.",
                positions[req->position_id]);
    }

    printf("[LOG] %s voted for position %d (candidate index %d). all_done=%d\n",
           req->student_id, req->position_id, req->candidate_index, all_done);
}

/* ================================================================== */
/*  MAIN                                                                */
/* ================================================================== */
int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);
    printf("[+] SONU Server is LIVE and listening on port %d...\n", PORT);

    while (1) {
        addr_size     = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == INVALID_SOCKET) continue;

        ClientRequest req;
        int bytes = recv(client_socket, (char *)&req, sizeof(ClientRequest), 0);

        if (bytes > 0) {
            ServerResponse res;
            memset(&res, 0, sizeof(ServerResponse));

            /* ---- dispatch ---- */
            switch (req.type) {

            case REQ_PING:
                res.success = 1;
                strcpy(res.message, "Hello from the SONU Server! Connection is stable.");
                break;

            /* ---- voter registration ---- */
            case REQ_REGISTER_VOTER:
                if (debug_check_duplicate(req.student_id)) {
                    res.success = 0;
                    strcpy(res.message, "Registration Failed: Student ID already registered.");
                } else {
                    FILE *fp = fopen("voters.dat", "ab+");
                    if (fp) {
                        Voter nv;
                        memset(&nv, 0, sizeof(Voter));
                        strcpy(nv.student_id, req.student_id);
                        strcpy(nv.name, req.name);
                        nv.has_voted  = 0;
                        nv.votes_cast = 0;
                        fwrite(&nv, sizeof(Voter), 1, fp);
                        fclose(fp);
                        res.success = 1;
                        strcpy(res.message, "Voter registration successful!");
                    }
                }
                break;

            /* ---- candidate registration ---- */
            case REQ_REGISTER_CANDIDATE:
                printf("\n[LOG] Candidate Reg attempt for ID: '%s'\n", req.student_id);
                if (debug_check_candidate_duplicate(req.student_id)) {
                    res.success = 0;
                    strcpy(res.message, "Registration Failed: Candidate ID already exists.");
                } else {
                    FILE *fp = fopen("candidates.dat", "ab+");
                    if (!fp) {
                        res.success = 0;
                        strcpy(res.message, "Registration Failed: Server database error.");
                    } else {
                        Candidate nc;
                        memset(&nc, 0, sizeof(Candidate));
                        strcpy(nc.student_id, req.student_id);
                        strcpy(nc.name, req.name);
                        strcpy(nc.position, positions[req.position_id]);
                        nc.votes = 0;
                        fwrite(&nc, sizeof(Candidate), 1, fp);
                        fclose(fp);
                        res.success = 1;
                        sprintf(res.message, "Candidate registered for %s!", nc.position);
                        printf("      -> Success: '%s' for %s.\n", req.name, nc.position);
                    }
                }
                break;

            /* ---- view all candidates ---- */
            case REQ_VIEW_CANDIDATES: {
                printf("\n[LOG] Sending candidate list to client.\n");
                FILE *fp = fopen("candidates.dat", "rb");
                if (!fp) {
                    res.success = 0;
                    strcpy(res.message, "No candidates registered yet.");
                } else {
                    res.success = 1;
                    strcpy(res.message,
                           "============================= Registered Candidates =============================\n"
                           "Student ID      | Name                           | Position                                 | Votes\n"
                           "---------------------------------------------------------------------------------\n");
                    Candidate temp;
                    char line[200];
                    while (fread(&temp, sizeof(Candidate), 1, fp)) {
                        sprintf(line, "%-15s | %-30s | %-40s | %-5d\n",
                                temp.student_id, temp.name, temp.position, temp.votes);
                        strcat(res.message, line);
                    }
                    fclose(fp);
                    strcat(res.message, "=================================================================================\n");
                }
                break;
            }

            /* ---- voting handlers ---- */
            case REQ_GET_VOTER_STATUS:
                handle_get_voter_status(&req, &res);
                break;

            case REQ_GET_CANDIDATES_FOR_POS:
                handle_get_candidates_for_pos(&req, &res);
                break;

            case REQ_CAST_VOTE:
                handle_cast_vote(&req, &res);
                break;

            default:
                res.success = 0;
                strcpy(res.message, "Unknown request type.");
                break;
            }

            send(client_socket, (char *)&res, sizeof(ServerResponse), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}