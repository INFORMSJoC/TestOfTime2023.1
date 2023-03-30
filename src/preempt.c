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
