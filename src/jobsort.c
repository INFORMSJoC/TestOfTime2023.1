/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include "jobdefs.h"

initheadsort ()
{
    int i;
    int j;
    static jobstep p, q;


    p.head = STARTOFTIME;
    q.head = ENDOFTIME;

    for (i=0; i < num_of_machines; i++) {
        MALLOC(schedule[i].headorder, num_of_jobs+2, jobstep *);
        *(schedule[i].headorder++) = &p;
        schedule[i].headorder[num_of_jobs] = &q;
        for (j=0; j < num_of_jobs; j++)
            schedule[i].headorder[j] = schedule[i].jobs + j;
    }
}
    
headsort (m)
machine *m;
{
    register jobstep **i,**j, **k;
    register int v;
    register jobstep *t;
    register jobstep **bound = m->headorder + num_of_jobs;

    for (i = m->headorder+1; i < bound; i++) {
        t = *i;
        v = t->head;
        for (j = i, k=j-1; (*k)->head > v; j = k, --k)
            *j = *k;
        *j = t;
    }
}
