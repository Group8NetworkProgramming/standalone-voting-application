#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voter.h"

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

int main() {
    int choice;
    while (1) {
        printf("\n1. Register Voter\n2. Exit\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) register_voter();
        else break;
    }
    return 0;
}