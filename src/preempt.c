/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"

preemptive (m)
machine *m;
{
    typedef struct qentry {
        jobstep *job;
        int time;
    } qentry;
    static qentry *qspace = 0;
    qentry *q;
    qentry *qtop;
    int t, tstop, bnd;
    jobstep bottom, top;
    jobstep **hp;
    int i;
    qentry *j, *k;

    bnd = 0;
    if (!qspace)
        MALLOC (qspace, num_of_jobs+2, qentry);
    bottom.tail = ENDOFTIME;
    top.tail = ENDOFTIME;
    top.head = ENDOFTIME;
    qtop = qspace;
    q = qspace;
    q++->job = &bottom;
    for (i = 0; i <= num_of_jobs; i++)
        q[i].job = &top;

    headsort (m);

    t = m->headorder[0] -> head;
    hp = m->headorder;

    while (t < ENDOFTIME) {
        while ((*hp) -> head <= t) {
            for (k = qtop++, j = k + 1; k -> job -> tail < (*hp) -> tail;
                 j = k, --k)
                *j = *k;
            j -> time = (*hp) -> body;
            j -> job = *hp++;
        }
        tstop = (*hp) -> head;
        if (t + q -> time < tstop) {
            t += q -> time;
            if (t + q -> job -> tail > bnd)
                bnd = t + q -> job -> tail;
            q++->job = &bottom;
            if (t < q -> job -> head)
                if (q -> job -> head < tstop)
                    t = q -> job -> head;
                else
                    t = tstop;
        } else {
            q -> time -= (tstop - t);
            t = tstop;
        }
    }
    return bnd;
}
