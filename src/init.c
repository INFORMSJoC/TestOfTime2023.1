/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include "jobdefs.h"

initialize()
{
    int i, j;
    int t, tsum;
    jobstep *p;
    machine *m;

    initheadsort ();

    for (i=0; i<num_of_jobs; i++) {
        tsum = 0;
        for (j=0; j<num_of_machines; j++)
            tsum += schedule[j].jobs[i].body;
        p = jobhead[i];
        t = 0;
        for (j=0, p = jobhead[i]; j<num_of_machines; j++, p = p -> jsucc) {
            CALLOC(schedule[j].jobs[i].successors.set,num_of_jobs,jobstep *);
           CALLOC(schedule[j].jobs[i].predecessors.set,num_of_jobs,jobstep *);
            CALLOC(schedule[j].jobs[i].successors.inverse,num_of_jobs,
                   jobstep **);
            CALLOC(schedule[j].jobs[i].predecessors.inverse,num_of_jobs,
                   jobstep **);
            schedule[j].jobs[i].successors.setend = 
                schedule[j].jobs[i].successors.set;
            schedule[j].jobs[i].predecessors.setend = 
                schedule[j].jobs[i].predecessors.set;
            p->head = t;
            p->tail = tsum - t - p->body;
            t += p->body;
        }
    }
    for (j = 0, m=schedule; j < num_of_machines; j++, m++) {
        CALLOC (m->cset.set,num_of_jobs,jobstep *);
        CALLOC (m->eset.set,num_of_jobs,jobstep *);
        CALLOC (m->sset.set,num_of_jobs,jobstep *);
        m->cset.setend = m->cset.set;
        m->eset.setend = m->eset.set;
        m->sset.setend = m->sset.set;
        m->cset_sum = 0;
        m->maxbody = 0;
        for (i = 0; i < num_of_jobs; i++) {
            *m->cset.setend++ = &m->jobs[i];
            *m->eset.setend++ = &m->jobs[i];
            *m->sset.setend++ = &m->jobs[i];
            m->cset_sum += m->jobs[i].body;
            if (m->jobs[i].body > m->maxbody)
                m->maxbody = m->jobs[i].body;
        }
    }
}
