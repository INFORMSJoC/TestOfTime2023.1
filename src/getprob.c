/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

/*
MIT License

Copyright (c) 2023 David Applegate and William Cook

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
