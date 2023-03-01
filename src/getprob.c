/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"

extern int first_machine;

getprob(f) char *f;
{
    FILE *ifile;
    int i, j, m, t;
    jobstep *p;
    char *probname;

    ifile = fopen (f, "r");
    if (!ifile) {
        fprintf (stderr, "Error, unable to open file\n");
        exit (1);
    }

    MALLOC (probname, 100, char);

    fgets (probname, 100, ifile);
    fscanf (ifile, "%d %d", &num_of_jobs, &num_of_machines);

    CALLOC (schedule, num_of_machines, machine);
    CALLOC (jobhead, num_of_jobs, jobstep *);

    for (j = 0; j < num_of_machines; j++)
        CALLOC (schedule[j].jobs, num_of_jobs, jobstep);
    for (i = 0; i < num_of_jobs; i++) {
        p = NULL;
        for (j = 0; j < num_of_machines; j++) {
            fscanf (ifile, "%d %d", &m, &t);
            schedule[m-first_machine].jobs[i].body = t;
            schedule[m-first_machine].jobs[i].job = i;
            schedule[m-first_machine].jobs[i].machine = m - first_machine;
            schedule[m-first_machine].jobs[i].jpred = p;
            if (p) {
                p -> jsucc = schedule[m-first_machine].jobs + i;
            }
            else
                jobhead[i] = schedule[m-first_machine].jobs + i;
            p = schedule[m-first_machine].jobs + i;
        }
        p -> jsucc = NULL;
    }
    fclose (ifile);
}
