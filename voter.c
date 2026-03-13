#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voter.h"

// --- VOTER FUNCTIONS ---

void register_voter() {
    FILE *fp = fopen("voters.dat", "ab+");
    if (fp == NULL) {
        printf("Error opening database!\n");
        return;
    }

    Voter new_voter;
    printf("\n--- SONU Voter Registration ---\n");
    printf("Enter Student ID: ");
    scanf("%s", new_voter.student_id);

    // Check for duplicates
    if (is_id_registered(new_voter.student_id)) {
        printf("Error: Student ID %s is already registered.\n", new_voter.student_id);
        fclose(fp);
        return;
    }

    printf("Enter Full Name: ");
    scanf(" %[^\n]s", new_voter.name);
    new_voter.has_voted = 0; // Initialize as not voted

    fwrite(&new_voter, sizeof(Voter), 1, fp);
    fclose(fp);
    printf("Registration successful!\n");
}

int is_id_registered(const char* id) {
    FILE *fp = fopen("voters.dat", "rb");
    if (fp == NULL) return 0;

    Voter temp;
    while (fread(&temp, sizeof(Voter), 1, fp)) {
        if (strcmp(temp.student_id, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// --- CANDIDATE FUNCTIONS ---

int is_candidate_registered(const char* id) {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) return 0; // File doesn't exist yet, which is fine

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

    // Check for duplicates
    if (is_candidate_registered(new_candidate.student_id)) {
        printf("Error: Candidate ID %s is already registered.\n", new_candidate.student_id);
        fclose(fp);
        return;
    }

    printf("Enter Full Name: ");
    scanf(" %[^\n]s", new_candidate.name);

    // Array of positions mapped to choices 1-11 (0 is left empty)
    const char* positions[] = {
        "", 
        "Chairman", "Vice Chairman", "Secretary General",
        "Organizing Secretary", "Secretary for Finance",
        "Secretary for Academic Affairs", "Secretary for Catering and Accommodation",
        "Secretary for Legal Affairs", "Secretary for Gender Affairs",
        "Secretary for Special Needs", "Campus/Faculty Representatives"
    };

    int pos_choice;
    int valid_choice = 0;

    // Loop until a valid choice is made
    while (!valid_choice) {
        // Display SONU Positions
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
        printf("Select Position (1-11): ");

        scanf("%d", &pos_choice);

        if (pos_choice >= 1 && pos_choice <= 11) {
            strcpy(new_candidate.position, positions[pos_choice]);
            valid_choice = 1; // This breaks us out of the loop
        } else {
            // Give an error and let the loop run again
            printf("\n[ERROR] Invalid choice. Please read the list and enter a number between 1 and 11.\n");
        }
    }

    fwrite(&new_candidate, sizeof(Candidate), 1, fp);
    fclose(fp);
    printf("Candidate successfully registered for %s!\n", new_candidate.position);
}

void display_candidates() {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) {
        printf("\nNo candidates registered yet (or the file is missing).\n");
        return;
    }

    Candidate temp;
    printf("\n============================= Registered Candidates =============================\n");
    printf("%-15s | %-30s | %-40s\n", "Student ID", "Name", "Position");
    printf("---------------------------------------------------------------------------------\n");

    // Read the binary file chunk by chunk (sizeof(Candidate)) until EOF
    while (fread(&temp, sizeof(Candidate), 1, fp)) {
        printf("%-15s | %-30s | %-40s\n", temp.student_id, temp.name, temp.position);
    }

    fclose(fp);
    printf("=================================================================================\n");
}

// --- MAIN MENU ---

int main() {
    int choice;
    while (1) {
        printf("\n--- SONU Election System ---\n");
        printf("1. Register Voter\n");
        printf("2. Register Candidate\n");
        printf("3. View Candidates\n"); 
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) register_voter();
        else if (choice == 2) register_candidate();
        else if (choice == 3) display_candidates();
        else break;
    }
    return 0;
}