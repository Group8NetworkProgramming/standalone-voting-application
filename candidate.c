#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voter.h"


// --- CANDIDATE FUNCTIONS ---

int is_candidate_registered(const char* id) {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) return 0;

    Candidate temp;
    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        if (strcmp(temp.student_id, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void register_candidate() {
    FILE *fp = fopen("candidates.dat", "ab+");
    if (fp == NULL) {
        printf("Error opening candidates database!\n");
        return;
    }

    Candidate new_candidate;
    printf("\n--- SONU Candidate Registration ---\n");
    printf("Enter Student ID: ");
    scanf("%s", new_candidate.student_id);

    if (is_candidate_registered(new_candidate.student_id)) {
        printf("Error: Candidate ID %s is already registered.\n", new_candidate.student_id);
        fclose(fp);
        return;
    }

    printf("Enter Full Name: ");
    scanf(" %[^\n]s", new_candidate.name);

    new_candidate.votes = 0;

    const char* positions[] = {
        "",
        "Chairman",
        "Vice Chairman","Secretary General",
        "Organizing Secretary","Secretary for Finance",
        "Secretary for Academic Affairs","Secretary for Catering and Accommodation",
        "Secretary for Legal Affairs","Secretary for Gender Affairs",
        "Secretary for Special Needs","Campus/Faculty Representatives"
    };

    int pos_choice;
    int valid_choice = 0;

    while (!valid_choice) {

        printf("\nAvailable SONU Positions:\n");
        printf("1. Chairman\n");
        printf("2. Vice Chairman\n");
        printf("3. Secretary General\n");
        printf("4. Organizing Secretary\n");
        printf("5. Secretary for Finance\n");
        printf("6. Secretary for Academic Affairs\n");
        printf("7. Secretary for Catering and Accommodation\n");
        printf("8. Secretary for Legal Affairs\n");
        printf("9. Secretary for Gender Affairs\n");
        printf("10. Secretary for Special Needs\n");
        printf("11. Campus/Faculty Representatives\n");
        printf("Select Position (1 - 11): ");

        scanf("%d", &pos_choice);

        if (pos_choice >= 1 && pos_choice <= 11) {
            strcpy(new_candidate.position, positions[pos_choice]);
            valid_choice = 1;
        } else {
            printf("\n[ERROR] Invalid choice. Enter number between 1 and 11.\n");
        }
    }

    fwrite(&new_candidate, sizeof(Candidate), 1, fp);
    fclose(fp);

    printf("Candidate successfully registered for %s!\n", new_candidate.position);
}

void display_candidates() {

    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) {
        printf("\nNo candidates registered yet.\n");
        return;
    }

    Candidate temp;

    printf("\n============================= Registered Candidates =============================\n");
    printf("%-15s | %-30s | %-40s | %-5s\n", "Student ID", "Name", "Position", "Votes");
    printf("---------------------------------------------------------------------------------\n");

    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        printf("%-15s | %-30s | %-40s | %-5d\n",
        temp.student_id,temp.name,temp.position,temp.votes);
    }

    fclose(fp);
    printf("=================================================================================\n");
}