#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "protocol.h"
#include "voter.h"

// X-Ray duplicate checker for voters
int debug_check_duplicate(const char* target_id) {
    FILE *fp = fopen("voters.dat", "rb");
    if (fp == NULL) return 0; 
    Voter temp;
    while (fread(&temp, sizeof(Voter), 1, fp)) {
        if (strcmp(temp.student_id, target_id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// X-Ray duplicate checker for candidates
int debug_check_candidate_duplicate(const char* target_id) {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) return 0; 
    Candidate temp;
    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        if (strcmp(temp.student_id, target_id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size;

    WSAStartup(MAKEWORD(2,2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_socket, 5);
    printf("[+] SONU Server is LIVE and listening on port %d...\n", PORT);

    const char* positions[] = {
        "", "Chairman", "Vice Chairman", "Secretary General",
        "Organizing Secretary", "Secretary for Finance",
        "Secretary for Academic Affairs", "Secretary for Catering and Accommodation",
        "Secretary for Legal Affairs", "Secretary for Gender Affairs",
        "Secretary for Special Needs", "Campus/Faculty Representatives"
    };

    while (1) {
        addr_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket == INVALID_SOCKET) continue; 
        
        ClientRequest req;
        int bytes_received = recv(client_socket, (char*)&req, sizeof(ClientRequest), 0);
        
        if (bytes_received > 0) {
            ServerResponse res;
            memset(&res, 0, sizeof(ServerResponse));

            if (req.type == REQ_PING) {
                res.success = 1;
                strcpy(res.message, "Hello from the SONU Server! Connection is stable.");
            }
            // --- VOTER REGISTRATION ---
            else if (req.type == REQ_REGISTER_VOTER) {
                if (debug_check_duplicate(req.student_id)) {
                    res.success = 0;
                    strcpy(res.message, "Registration Failed: Student ID is already registered.");
                } else {
                    FILE *fp = fopen("voters.dat", "ab+");
                    if (fp != NULL) {
                        Voter new_voter;
                        memset(&new_voter, 0, sizeof(Voter)); 
                        strcpy(new_voter.student_id, req.student_id);
                        strcpy(new_voter.name, req.name);
                        new_voter.has_voted = 0;
                        new_voter.votes_cast = 0;
                        fwrite(&new_voter, sizeof(Voter), 1, fp);
                        fclose(fp);
                        res.success = 1;
                        strcpy(res.message, "Voter Registration successful!");
                    }
                }
            }
            // --- CANDIDATE REGISTRATION ---
            else if (req.type == REQ_REGISTER_CANDIDATE) {
                printf("\n[LOG] Candidate Reg attempt for ID: '%s'\n", req.student_id);
                if (debug_check_candidate_duplicate(req.student_id)) {
                    res.success = 0;
                    strcpy(res.message, "Registration Failed: Candidate ID already exists.");
                    printf("      -> Rejected: Duplicate ID.\n");
                } else {
                    FILE *fp = fopen("candidates.dat", "ab+");
                    if (fp == NULL) {
                        res.success = 0;
                        strcpy(res.message, "Registration Failed: Server database error.");
                    } else {
                        Candidate new_cand;
                        memset(&new_cand, 0, sizeof(Candidate));
                        strcpy(new_cand.student_id, req.student_id);
                        strcpy(new_cand.name, req.name);
                        strcpy(new_cand.position, positions[req.position_id]);
                        new_cand.votes = 0;

                        fwrite(&new_cand, sizeof(Candidate), 1, fp);
                        fclose(fp);
                        
                        res.success = 1;
                        sprintf(res.message, "Candidate successfully registered for %s!", new_cand.position);
                        printf("      -> Success: Registered '%s' for %s.\n", req.name, new_cand.position);
                    }
                }
            }
            // --- VIEW CANDIDATES ---
            else if (req.type == REQ_VIEW_CANDIDATES) {
                printf("\n[LOG] Sending candidate list to client.\n");
                FILE *fp = fopen("candidates.dat", "rb");
                if (fp == NULL) {
                    res.success = 0;
                    strcpy(res.message, "No candidates registered yet.");
                } else {
                    res.success = 1;
                    // We will build a massive string to send back
                    strcpy(res.message, "============================= Registered Candidates =============================\n");
                    strcat(res.message, "Student ID      | Name                           | Position                                 | Votes\n");
                    strcat(res.message, "---------------------------------------------------------------------------------\n");
                    
                    Candidate temp;
                    char line[200];
                    while (fread(&temp, sizeof(Candidate), 1, fp)) {
                        sprintf(line, "%-15s | %-30s | %-40s | %-5d\n", temp.student_id, temp.name, temp.position, temp.votes);
                        strcat(res.message, line);
                    }
                    fclose(fp);
                    strcat(res.message, "=================================================================================\n");
                }
            }

            send(client_socket, (char*)&res, sizeof(ServerResponse), 0);
        }
        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}