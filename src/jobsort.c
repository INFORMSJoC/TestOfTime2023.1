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
