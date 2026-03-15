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
    new_voter.has_voted = 0;

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
        // "Vice Chairman","Secretary General",
        // "Organizing Secretary","Secretary for Finance",
        // "Secretary for Academic Affairs","Secretary for Catering and Accommodation",
        // "Secretary for Legal Affairs","Secretary for Gender Affairs",
        // "Secretary for Special Needs","Campus/Faculty Representatives"
    };

    int pos_choice;
    int valid_choice = 0;

    while (!valid_choice) {

        printf("\nAvailable SONU Positions:\n");
        printf("1. Chairman\n");
        // printf("2. Vice Chairman\n");
        // printf("3. Secretary General\n");
        // printf("4. Organizing Secretary\n");
        // printf("5. Secretary for Finance\n");
        // printf("6. Secretary for Academic Affairs\n");
        // printf("7. Secretary for Catering and Accommodation\n");
        // printf("8. Secretary for Legal Affairs\n");
        // printf("9. Secretary for Gender Affairs\n");
        // printf("10. Secretary for Special Needs\n");
        // printf("11. Campus/Faculty Representatives\n");
        printf("Select Position (1): ");

        scanf("%d", &pos_choice);

        if (pos_choice >= 1 && pos_choice <= 1) {
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

// --- VOTING FUNCTION ---

void cast_vote() {

    char voter_id[20];
    Voter voter;

    FILE *vf = fopen("voters.dat","rb+");

    if(vf==NULL){
        printf("Voter database missing!\n");
        return;
    }

    printf("Enter your Student ID: ");
    scanf("%s",voter_id);

    int found=0;

    while(fread(&voter,sizeof(Voter),1,vf)){
        if(strcmp(voter.student_id,voter_id)==0){
            found=1;

            if(voter.has_voted){
                printf("You have already voted!\n");
                fclose(vf);
                return;
            }
            break;
        }
    }

    if(!found){
        printf("Voter not registered!\n");
        fclose(vf);
        return;
    }

    FILE *cf=fopen("candidates.dat","rb+");

    if(cf==NULL){
        printf("No candidates available!\n");
        fclose(vf);
        return;
    }

    Candidate candidates[200];
    int count=0;

    while(fread(&candidates[count],sizeof(Candidate),1,cf)){
        count++;
    }

    fclose(cf);

    const char* positions[] = {
        "",
        "Chairman","Vice Chairman","Secretary General",
        "Organizing Secretary","Secretary for Finance",
        "Secretary for Academic Affairs","Secretary for Catering and Accommodation",
        "Secretary for Legal Affairs","Secretary for Gender Affairs",
        "Secretary for Special Needs","Campus/Faculty Representatives"
    };

    int choice;

    for(int p=1;p<=11;p++){

        printf("\nVote for %s\n",positions[p]);

        int index[50];
        int k=0;

        for(int i=0;i<count;i++){
            if(strcmp(candidates[i].position,positions[p])==0){
                printf("%d. %s (%s)\n",k+1,candidates[i].name,candidates[i].student_id);
                index[k]=i;
                k++;
            }
        }

        if(k==0){
            printf("No candidates for this position\n");
            continue;
        }

        printf("Select candidate: ");
        scanf("%d",&choice);

        if(choice>=1 && choice<=k){
            candidates[index[choice-1]].votes++;
        }
    }

    cf=fopen("candidates.dat","wb");

    for(int i=0;i<count;i++){
        fwrite(&candidates[i],sizeof(Candidate),1,cf);
    }

    fclose(cf);

    fseek(vf, -(long)sizeof(Voter), SEEK_CUR);
    voter.has_voted = 1;
    fwrite(&voter, sizeof(Voter), 1, vf);

    fclose(vf);

    printf("\nVoting completed successfully!\n");
}

// --- MAIN MENU ---

int main() {

    int choice;

    while (1) {

        printf("\n--- SONU Election System ---\n");
        printf("1. Register Voter\n");
        printf("2. Register Candidate\n");
        printf("3. View Candidates\n");
        printf("4. Cast Vote\n");
        printf("5. Exit\n");

        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) register_voter();
        else if (choice == 2) register_candidate();
        else if (choice == 3) display_candidates();
        else if (choice == 4) cast_vote();
        else break;
    }

    return 0;
}