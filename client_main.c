#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "protocol.h"

void send_request_to_server(ClientRequest *req) {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    WSAStartup(MAKEWORD(2,2), &wsa);
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\n[-] Error: Could not connect to the SONU Server. Is it running?\n");
        return;
    }

    send(client_socket, (char*)req, sizeof(ClientRequest), 0);

    ServerResponse res;
    // Notice we don't zero out memory here, we just overwrite it with the massive response
    recv(client_socket, (char*)&res, sizeof(ServerResponse), 0);

    printf("\n--- SYSTEM RESPONSE ---\n");
    printf("%s\n", res.message);
    printf("-----------------------\n");

    closesocket(client_socket);
    WSACleanup();
}

int main() {
    int choice;

    while (1) {
        printf("\n=================================\n");
        printf("    SONU VOTING SYSTEM CLIENT\n");
        printf("=================================\n");
        printf("1. Register as a Voter\n");
        printf("2. Register as a Candidate\n");
        printf("3. View Registered Candidates\n");
        printf("4. Test Server Connection (Ping)\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); 
            continue;
        }

        if (choice == 1) {
            ClientRequest req;
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_REGISTER_VOTER;

            printf("\n--- SONU Voter Registration ---\n");
            printf("Enter Student ID: ");
            scanf("%14s", req.student_id);
            printf("Enter Full Name: ");
            scanf(" %[^\n]s", req.name); 

            send_request_to_server(&req);

        } else if (choice == 2) {
            ClientRequest req;
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_REGISTER_CANDIDATE;

            printf("\n--- SONU Candidate Registration ---\n");
            printf("Enter Student ID: ");
            scanf("%14s", req.student_id);
            printf("Enter Full Name: ");
            scanf(" %[^\n]s", req.name);

            int pos_choice;
            int valid_choice = 0;
            while (!valid_choice) {
                printf("\nAvailable SONU Positions:\n");
                printf("1. Chairman\n2. Vice Chairman\n3. Secretary General\n");
                printf("4. Organizing Secretary\n5. Secretary for Finance\n");
                printf("6. Secretary for Academic Affairs\n7. Secretary for Catering and Accommodation\n");
                printf("8. Secretary for Legal Affairs\n9. Secretary for Gender Affairs\n");
                printf("10. Secretary for Special Needs\n11. Campus/Faculty Representatives\n");
                printf("Select Position (1 - 11): ");
                
                scanf("%d", &pos_choice);
                if (pos_choice >= 1 && pos_choice <= 11) {
                    req.position_id = pos_choice;
                    valid_choice = 1;
                } else {
                    printf("\n[ERROR] Invalid choice. Enter number between 1 and 11.\n");
                }
            }

            send_request_to_server(&req);

        } else if (choice == 3) {
            ClientRequest req;
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_VIEW_CANDIDATES;
            send_request_to_server(&req);

        } else if (choice == 4) {
            ClientRequest req;
            memset(&req, 0, sizeof(ClientRequest));
            req.type = REQ_PING;
            send_request_to_server(&req);

        } else if (choice == 5) {
            printf("Exiting client...\n");
            break;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}