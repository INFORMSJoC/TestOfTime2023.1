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
#include "bottle.h"

machine *schedule;
jobstep **jobhead;

int num_of_jobs;
int num_of_machines;

int limit, inlimit;

int bnbeval = 0;
int depth = 0;
char searchvector[1024];

machine *machkill;

edgelist *alloc_edgelist();
setchangelist *alloc_setchangelist();

french_loadprob(njobs, nmachs, jobs)
int njobs;
int nmachs;
jobtype *jobs;
{
    int i, j;
    jobstep *p;

    num_of_jobs = njobs;
    num_of_machines = nmachs;

    CALLOC (schedule, num_of_machines, machine);
    CALLOC (jobhead, num_of_jobs, jobstep *);

    for (j = 0; j < num_of_machines; j++)
        CALLOC (schedule[j].jobs, num_of_jobs, jobstep);
    for (i = 0; i < num_of_jobs; i++) {
        p = NULL;
        for (j = 0; j < num_of_machines; j++) {
            schedule[jobs[i].order[j]].jobs[i].body = 
                jobs[i].proctime[jobs[i].order[j]];
            schedule[jobs[i].order[j]].jobs[i].job = i;
            schedule[jobs[i].order[j]].jobs[i].machine = jobs[i].order[j];
            schedule[jobs[i].order[j]].jobs[i].jpred = p;
            if (p) {
                p -> jsucc = schedule[jobs[i].order[j]].jobs + i;
            }
            else
                jobhead[i] = schedule[jobs[i].order[j]].jobs + i;
            p = schedule[jobs[i].order[j]].jobs + i;
        }
        p -> jsucc = NULL;
    }
    initialize();
}

static edgelist *initsched = 0;

french_schedmach (m,s)
int m;
int *s;
{
    edgelist *e;
    int i;


    for (i = 1; i < num_of_jobs; i++) {
        e = alloc_edgelist ();
        e -> from = &schedule[m].jobs[s[i - 1]];
        e -> to = &schedule[m].jobs[s[i]];
        e -> next = 0;
        doedge (e);
        e -> next = initsched;
        initsched = e;
    }
}

static jobtype *savejobloc;

french_finish (jobs, limit_in)
jobtype *jobs;
int limit_in;
{
        int i;

        savejobloc = jobs;
        
        i = lbound (&machkill);
        printf ("Initial bound %d, killing machine %d\n",i,machkill-schedule);

        searchvector[0] = searchvector[1] = '\0';
        limit = limit_in;
        muncher();
        printf ("%d nodes evaluated, limit = %d\n",bnbeval,limit);
        undoedge(initsched);
        freelist (initsched);
        initsched = 0;
        if (limit < limit_in)
            return limit+1;
        else
            return -1;
}

saveheads()
{
    int i, j;

    for (i=0; i<num_of_jobs; i++) {
        for (j=0; j<num_of_machines; j++) {
            savejobloc[i].start[j] = schedule[j].jobs[i].head;
        }
    }
}
