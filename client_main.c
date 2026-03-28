#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "protocol.h"

/* ------------------------------------------------------------------ */
/*  Low-level: open socket, send request, receive response, close.     */
/*  Returns 1 on success, 0 on connection failure.                     */
/* ------------------------------------------------------------------ */
int send_request(ClientRequest *req, ServerResponse *res) {
    WSADATA wsa;
    SOCKET  sock;
    struct sockaddr_in server_addr;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\n[-] Could not connect to SONU Server. Is it running?\n");
        WSACleanup();
        return 0;
    }

    send(sock, (char *)req, sizeof(ClientRequest), 0);
    memset(res, 0, sizeof(ServerResponse));
    recv(sock, (char *)res, sizeof(ServerResponse), 0);

    closesocket(sock);
    WSACleanup();
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Voting flow                                                         */
/*  Steps:                                                              */
/*    1. Ask for student ID                                             */
/*    2. GET_VOTER_STATUS  → verify + fetch bitmask                    */
/*    3. For each position not yet in bitmask:                          */
/*       a. GET_CANDIDATES_FOR_POS → display list                      */
/*       b. Collect choice                                              */
/*       c. CAST_VOTE → confirm + update bitmask                       */
/* ------------------------------------------------------------------ */
void voting_menu() {
    char voter_id[15];
    printf("\n--- SONU Voting Booth ---\n");
    printf("Enter your Student ID: ");
    scanf("%14s", voter_id);

    /* ---- Step 1: verify voter and get session state ---- */
    ClientRequest  req;
    ServerResponse res;

    memset(&req, 0, sizeof(ClientRequest));
    req.type = REQ_GET_VOTER_STATUS;
    strcpy(req.student_id, voter_id);

    if (!send_request(&req, &res)) return;

    if (!res.success) {
        printf("\n[!] %s\n", res.message);
        return;
    }

    printf("\n%s\n", res.message);

    int votes_cast = res.votes_cast;  /* bitmask from server */

    /* ---- Step 2: count remaining positions ---- */
    int remaining = 0;
    for (int p = 1; p <= 11; p++) {
        if (!(votes_cast & (1 << p))) remaining++;
    }

    if (remaining == 0) {
        printf("[!] All positions already voted. Nothing to do.\n");
        return;
    }

    if (votes_cast != 0)
        printf("[i] Session resumed. %d position(s) remaining.\n", remaining);
    else
        printf("[i] Starting voting. %d positions to vote for.\n", remaining);

    /* ---- Step 3: loop through each position ---- */
    for (int p = 1; p <= 11; p++) {

        /* Skip already-voted positions */
        if (votes_cast & (1 << p)) {
            printf("\n[Skipped] Position %d — already voted.\n", p);
            continue;
        }

        /* ---- 3a. Fetch candidates for this position ---- */
        memset(&req, 0, sizeof(ClientRequest));
        req.type        = REQ_GET_CANDIDATES_FOR_POS;
        req.position_id = p;
        strcpy(req.student_id, voter_id);

        if (!send_request(&req, &res)) return;

        printf("\n%s", res.message);

        /* If no candidates, server auto-skips — update local bitmask and move on */
        if (!res.success) {
            printf("[i] Skipping this position (no candidates).\n");
            votes_cast |= (1 << p);
            continue;
        }

        int candidate_count = res.candidate_count;

        /* ---- 3b. Collect a valid choice ---- */
        int choice = 0;
        while (choice < 1 || choice > candidate_count) {
            printf("Select candidate (1-%d): ", candidate_count);
            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n');
                choice = 0;
            }
            if (choice < 1 || choice > candidate_count)
                printf("[ERROR] Invalid choice. Enter a number between 1 and %d.\n", candidate_count);
        }

        /* ---- 3c. Send the vote to the server ---- */
        memset(&req, 0, sizeof(ClientRequest));
        req.type             = REQ_CAST_VOTE;
        req.position_id      = p;
        req.candidate_index  = choice;
        strcpy(req.student_id, voter_id);

        if (!send_request(&req, &res)) return;

        if (res.success) {
            printf("[✓] %s\n", res.message);
            votes_cast = res.votes_cast;  /* sync bitmask with server */
        } else {
            printf("[!] %s\n", res.message);
            /* If server rejected, retry this position */
            p--;
        }
    }

    /* ---- Step 4: final summary ---- */
    int all_done = 1;
    for (int p = 1; p <= 11; p++) {
        if (!(votes_cast & (1 << p))) { all_done = 0; break; }
    }

    printf("\n=================================\n");
    if (all_done)
        printf("  Voting complete! Thank you.\n");
    else
        printf("  Session saved. Resume anytime.\n");
    printf("=================================\n");
}

/* ------------------------------------------------------------------ */
/*  MAIN MENU                                                           */
/* ------------------------------------------------------------------ */
int main() {
    int choice;

    while (1) {
        printf("\n=================================\n");
        printf("    SONU VOTING SYSTEM CLIENT\n");
        printf("=================================\n");
        printf("1. Register as a Voter\n");
        printf("2. Register as a Candidate\n");
        printf("3. View Registered Candidates\n");
        printf("4. Vote\n");
        printf("5. Test Server Connection (Ping)\n");
        printf("6. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        ClientRequest  req;
        ServerResponse res;

        if (choice == 1) {
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_REGISTER_VOTER;

            printf("\n--- SONU Voter Registration ---\n");
            printf("Enter Student ID: ");
            scanf("%14s", req.student_id);
            printf("Enter Full Name: ");
            scanf(" %[^\n]s", req.name);

            if (send_request(&req, &res))
                printf("\n[Server] %s\n", res.message);

        } else if (choice == 2) {
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_REGISTER_CANDIDATE;

            printf("\n--- SONU Candidate Registration ---\n");
            printf("Enter Student ID: ");
            scanf("%14s", req.student_id);
            printf("Enter Full Name: ");
            scanf(" %[^\n]s", req.name);

            int pos_choice = 0;
            while (pos_choice < 1 || pos_choice > 11) {
                printf("\nAvailable SONU Positions:\n");
                printf(" 1. Chairman\n 2. Vice Chairman\n 3. Secretary General\n");
                printf(" 4. Organizing Secretary\n 5. Secretary for Finance\n");
                printf(" 6. Secretary for Academic Affairs\n 7. Secretary for Catering and Accommodation\n");
                printf(" 8. Secretary for Legal Affairs\n 9. Secretary for Gender Affairs\n");
                printf("10. Secretary for Special Needs\n11. Campus/Faculty Representatives\n");
                printf("Select Position (1-11): ");
                if (scanf("%d", &pos_choice) != 1) { while(getchar()!='\n'); pos_choice=0; }
                if (pos_choice < 1 || pos_choice > 11)
                    printf("[ERROR] Invalid. Enter 1–11.\n");
            }
            req.position_id = pos_choice;

            if (send_request(&req, &res))
                printf("\n[Server] %s\n", res.message);

        } else if (choice == 3) {
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_VIEW_CANDIDATES;

            if (send_request(&req, &res))
                printf("\n%s\n", res.message);

        } else if (choice == 4) {
            voting_menu();

        } else if (choice == 5) {
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_PING;

            if (send_request(&req, &res))
                printf("\n[Server] %s\n", res.message);

        } else if (choice == 6) {
            printf("Exiting client. Goodbye!\n");
            break;

        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}