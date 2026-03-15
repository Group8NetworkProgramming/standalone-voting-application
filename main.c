#include <stdio.h>
#include "voter.h"

int main()
{

    int choice;

    while (1)
    {

        printf("\n--- SONU Election System ---\n");
        printf("1. Register Voter\n");
        printf("2. Register Candidate\n");
        printf("3. View Candidates\n");
        printf("4. Cast Vote\n");
        printf("5. View Vote Tally\n");
        printf("6. Announce Results\n");
        printf("7. Exit\n");

        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1)
            register_voter();
        else if (choice == 2)
            register_candidate();
        else if (choice == 3)
            display_candidates();
        else if (choice == 4)
            cast_vote();
        else if (choice == 5)
            tally_votes();
        else if (choice == 6)
            announce_results();
        else if (choice == 7)
            break;
        else
            break;
    }

    return 0;
}