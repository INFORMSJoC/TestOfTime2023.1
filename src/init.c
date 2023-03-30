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
