// results.c
#include <stdio.h>
#include <string.h>
#include "voter.h"

// The 11 official SONU positions — same order as cast_vote()
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

#define NUM_POSITIONS 11

// ── tally_votes() ─────────────────────────────────────────────────────────
// Reads candidates.dat and prints a full vote count table grouped by position.
// This is the raw tally — every candidate and their vote count, per position.

void tally_votes() {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) {
        printf("\nNo candidates database found. Has anyone registered?\n");
        return;
    }

    Candidate all[200];
    int count = 0;

    while (fread(&all[count], sizeof(Candidate), 1, fp)) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo candidates registered.\n");
        return;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║              SONU ELECTION — VOTE TALLY                         ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    for (int p = 1; p <= NUM_POSITIONS; p++) {

        int found_any = 0;

        // Check if any candidate is registered for this position
        for (int i = 0; i < count; i++) {
            if (strcmp(all[i].position, positions[p]) == 0) {
                found_any = 1;
                break;
            }
        }

        if (!found_any) continue;   // skip positions with no candidates

        printf("\n  %-45s\n", positions[p]);
        printf("  %-30s %-15s %-10s\n", "Name", "Student ID", "Votes");
        printf("  -------------------------------------------------------\n");

        for (int i = 0; i < count; i++) {
            if (strcmp(all[i].position, positions[p]) == 0) {
                printf("  %-30s %-15s %-10d\n",
                       all[i].name,
                       all[i].student_id,
                       all[i].votes);
            }
        }
    }

    printf("\n════════════════════════════════════════════════════════════════════\n");
}

// ── announce_results() ────────────────────────────────────────────────────
// Reads candidates.dat, finds the winner of each position (highest votes),
// handles ties, and prints the official results announcement.

void announce_results() {
    FILE *fp = fopen("candidates.dat", "rb");
    if (fp == NULL) {
        printf("\nNo candidates database found.\n");
        return;
    }

    Candidate all[200];
    int count = 0;

    while (fread(&all[count], sizeof(Candidate), 1, fp)) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo candidates registered.\n");
        return;
    }

    // Count total votes cast across all candidates
    int total_votes = 0;
    for (int i = 0; i < count; i++) {
        total_votes += all[i].votes;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║           SONU ELECTION — OFFICIAL RESULTS                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("  Total votes cast: %d\n", total_votes);
    printf("════════════════════════════════════════════════════════════════════\n");

    for (int p = 1; p <= NUM_POSITIONS; p++) {

        // ── Step 1: find the highest vote count for this position
        int max_votes = -1;

        for (int i = 0; i < count; i++) {
            if (strcmp(all[i].position, positions[p]) == 0) {
                if (all[i].votes > max_votes)
                    max_votes = all[i].votes;
            }
        }

        if (max_votes == -1) continue;  // no candidates for this position

        printf("\n  %s\n", positions[p]);
        printf("  ─────────────────────────────────────────────\n");

        // ── Step 2: count how many candidates share the max (tie check)
        int tie_count = 0;

        for (int i = 0; i < count; i++) {
            if (strcmp(all[i].position, positions[p]) == 0 &&
                all[i].votes == max_votes) {
                tie_count++;
            }
        }

        // ── Step 3: announce winner or declare a tie
        if (max_votes == 0) {
            printf("  ⚠  No votes were cast for this position.\n");

        } else if (tie_count > 1) {
            printf("  ⚠  TIE — %d candidates are tied with %d vote(s) each:\n",
                   tie_count, max_votes);

            for (int i = 0; i < count; i++) {
                if (strcmp(all[i].position, positions[p]) == 0 &&
                    all[i].votes == max_votes) {
                    printf("       • %s (%s)\n", all[i].name, all[i].student_id);
                }
            }
            printf("  A re-vote or tiebreaker procedure is required.\n");

        } else {
            // Single winner — find and print them
            for (int i = 0; i < count; i++) {
                if (strcmp(all[i].position, positions[p]) == 0 &&
                    all[i].votes == max_votes) {
                    printf("  WINNER : %s\n", all[i].name);
                    printf("  ID     : %s\n", all[i].student_id);
                    printf("  Votes  : %d\n", all[i].votes);
                    break;
                }
            }
        }
    }

    printf("\n════════════════════════════════════════════════════════════════════\n");
    printf("                     END OF OFFICIAL RESULTS\n");
    printf("════════════════════════════════════════════════════════════════════\n");
}