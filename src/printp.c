/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"

printprob (probname)
char *probname;
{
    int i, j;
    
    printf ("%s\n\n", probname);
    printf ("                    Starting Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].head);
        putchar ('\n');
    }
    printf ("\n                    Processing Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].body);
        putchar ('\n');
    }
    printf ("\n                    Finishing Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].tail);
        putchar ('\n');
    }
}
