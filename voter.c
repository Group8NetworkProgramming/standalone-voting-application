// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "voter.h"

// // --- VOTER FUNCTIONS ---

// void register_voter() {
//     FILE *fp = fopen("voters.dat", "ab+");
//     if (fp == NULL) {
//         printf("Error opening database!\n");
//         return;
//     }

//     Voter new_voter;
//     printf("\n--- SONU Voter Registration ---\n");
//     printf("Enter Student ID: ");
//     scanf("%s", new_voter.student_id);

//     // Check for duplicates
//     if (is_id_registered(new_voter.student_id)) {
//         printf("Error: Student ID %s is already registered.\n", new_voter.student_id);
//         fclose(fp);
//         return;
//     }

//     printf("Enter Full Name: ");
//     scanf(" %[^\n]s", new_voter.name);
//     new_voter.has_voted = 0;

//     fwrite(&new_voter, sizeof(Voter), 1, fp);
//     fclose(fp);
//     printf("Registration successful!\n");
// }

// int is_id_registered(const char* id) {
//     FILE *fp = fopen("voters.dat", "rb");
//     if (fp == NULL) return 0;

//     Voter temp;
//     while (fread(&temp, sizeof(Voter), 1, fp)) {
//         if (strcmp(temp.student_id, id) == 0) {
//             fclose(fp);
//             return 1;
//         }
//     }
//     fclose(fp);
//     return 0;
// }

// // --- VOTING FUNCTION ---

// void cast_vote() {

//     char voter_id[20];
//     Voter voter;

//     FILE *vf = fopen("voters.dat","rb+");

//     if(vf==NULL){
//         printf("Voter database missing!\n");
//         return;
//     }

//     printf("Enter your Student ID: ");
//     scanf("%s",voter_id);

//     int found=0;

//     while(fread(&voter,sizeof(Voter),1,vf)){
//         if(strcmp(voter.student_id,voter_id)==0){
//             found=1;

//             if(voter.has_voted){
//                 printf("You have already voted!\n");
//                 fclose(vf);
//                 return;
//             }
//             break;
//         }
//     }

//     if(!found){
//         printf("Voter not registered!\n");
//         fclose(vf);
//         return;
//     }

//     FILE *cf=fopen("candidates.dat","rb+");

//     if(cf==NULL){
//         printf("No candidates available!\n");
//         fclose(vf);
//         return;
//     }

//     Candidate candidates[200];
//     int count=0;

//     while(fread(&candidates[count],sizeof(Candidate),1,cf)){
//         count++;
//     }

//     fclose(cf);

//     const char* positions[] = {
//         "",
//         "Chairman","Vice Chairman","Secretary General",
//         "Organizing Secretary","Secretary for Finance",
//         "Secretary for Academic Affairs","Secretary for Catering and Accommodation",
//         "Secretary for Legal Affairs","Secretary for Gender Affairs",
//         "Secretary for Special Needs","Campus/Faculty Representatives"
//     };

//     int choice;

//     for(int p=1;p<=11;p++){

//         printf("\nVote for %s\n",positions[p]);

//         int index[50];
//         int k=0;

//         for(int i=0;i<count;i++){
//             if(strcmp(candidates[i].position,positions[p])==0){
//                 printf("%d. %s (%s)\n",k+1,candidates[i].name,candidates[i].student_id);
//                 index[k]=i;
//                 k++;
//             }
//         }

//         if(k==0){
//             printf("No candidates for this position\n");
//             continue;
//         }

//         printf("Select candidate: ");
//         scanf("%d",&choice);

//         if(choice>=1 && choice<=k){
//             candidates[index[choice-1]].votes++;
//         }
//     }

//     cf=fopen("candidates.dat","wb");

//     for(int i=0;i<count;i++){
//         fwrite(&candidates[i],sizeof(Candidate),1,cf);
//     }

//     fclose(cf);

//     fseek(vf, -(long)sizeof(Voter), SEEK_CUR);
//     voter.has_voted = 1;
//     fwrite(&voter, sizeof(Voter), 1, vf);

//     fclose(vf);

//     printf("\nVoting completed successfully!\n");
// }

// voting.c
#include <stdio.h>
#include <string.h>
#include "voter.h"

static const char *positions[] = {
    "",
    "Chairman",
    "Vice Chairman",
    "Secretary General",
    "Organizing Secretary",
    "Secretary for Finance",
    "Secretary for Academic Affairs",
    "Secretary for Catering and Accommodation",
    "Secretary for Legal Affairs",
    "Secretary for Gender Affairs",
    "Secretary for Special Needs",
    "Campus/Faculty Representatives"
};

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

void cast_vote() {

    // ── Step 1: open voter database ───────────────────────────────
    FILE *vf = fopen("voters.dat", "rb+");
    if (vf == NULL) {
        printf("Voter database missing!\n");
        return;
    }

    // ── Step 2: find the voter record ─────────────────────────────
    char voter_id[MAX_ID];
    printf("Enter your Student ID: ");
    scanf("%s", voter_id);

    Voter voter;
    long  voter_offset = 0;
    int   found = 0;

    // Track the byte offset of each record so we can seek back later
    while (fread(&voter, sizeof(Voter), 1, vf)) {
        if (strcmp(voter.student_id, voter_id) == 0) {
            found = 1;
            break;
        }
        voter_offset += sizeof(Voter);
    }

    if (!found) {
        printf("Voter not registered!\n");
        fclose(vf);
        return;
    }

    // ── Step 3: check if fully done ───────────────────────────────
    if (voter.has_voted) {
        printf("You have already completed voting for all positions.\n");
        fclose(vf);
        return;
    }

    // ── Step 4: show status if resuming a partial session ─────────
    int positions_remaining = 0;
    for (int p = 1; p <= NUM_POSITIONS; p++) {
        if (!(voter.votes_cast & (1 << p)))
            positions_remaining++;
    }

    if (voter.votes_cast != 0) {
        printf("\nWelcome back. You have %d position(s) remaining.\n",
               positions_remaining);
    }

    // ── Step 5: load all candidates into memory ───────────────────
    FILE *cf = fopen("candidates.dat", "rb+");
    if (cf == NULL) {
        printf("No candidates registered yet!\n");
        fclose(vf);
        return;
    }

    Candidate candidates[200];
    int count = 0;

    while (fread(&candidates[count], sizeof(Candidate), 1, cf)) {
        count++;
    }
    fclose(cf);

    // ── Step 6: loop through positions ────────────────────────────
    for (int p = 1; p <= NUM_POSITIONS; p++) {

        // Skip if already voted for this position
        if (voter.votes_cast & (1 << p)) {
            printf("\n[Skipping %s — already voted]\n", positions[p]);
            continue;
        }

        // Build list of candidates for this position
        int index[50];
        int k = 0;

        for (int i = 0; i < count; i++) {
            if (strcmp(candidates[i].position, positions[p]) == 0) {
                index[k++] = i;
            }
        }

        if (k == 0) {
            printf("\n%s — No candidates registered. Skipping.\n", positions[p]);
            // Mark as voted so it doesn't block has_voted completion
            voter.votes_cast |= (1 << p);
            fseek(vf, voter_offset, SEEK_SET);
            fwrite(&voter, sizeof(Voter), 1, vf);
            continue;
        }

        // Display candidates for this position
        printf("\n--- Vote for %s ---\n", positions[p]);
        for (int i = 0; i < k; i++) {
            printf("%d. %s (%s)\n",
                   i + 1,
                   candidates[index[i]].name,
                   candidates[index[i]].student_id);
        }

        // Get a valid choice
        int choice = 0;
        while (choice < 1 || choice > k) {
            printf("Select candidate (1-%d): ", k);
            scanf("%d", &choice);
            if (choice < 1 || choice > k)
                printf("[ERROR] Invalid choice. Try again.\n");
        }

        // Increment vote in memory
        candidates[index[choice - 1]].votes++;

        // ── Write this position's vote to candidates.dat immediately
        cf = fopen("candidates.dat", "wb");
        for (int i = 0; i < count; i++) {
            fwrite(&candidates[i], sizeof(Candidate), 1, cf);
        }
        fclose(cf);

        // ── Mark this position as voted in the voter record on disk
        voter.votes_cast |= (1 << p);
        fseek(vf, voter_offset, SEEK_SET);
        fwrite(&voter, sizeof(Voter), 1, vf);

        printf("Vote recorded for %s.\n", positions[p]);
    }

    // ── Step 7: check if all positions are now complete ───────────
    int all_done = 1;
    for (int p = 1; p <= NUM_POSITIONS; p++) {
        if (!(voter.votes_cast & (1 << p))) {
            all_done = 0;
            break;
        }
    }

    if (all_done) {
        voter.has_voted = 1;
        fseek(vf, voter_offset, SEEK_SET);
        fwrite(&voter, sizeof(Voter), 1, vf);
        printf("\nVoting completed successfully! Thank you.\n");
    } else {
        printf("\nSession saved. You can continue voting next time you log in.\n");
    }

    fclose(vf);
}