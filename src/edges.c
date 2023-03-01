/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"

#define NEW 1

doedge (list) edgelist *list;
{
    edgelist *p;
    int t;

    if (t = doedge_noprop (list))
        return t;
    else
        return propagate (list);
}

doedge_noprop (list) edgelist *list;
{
    edgelist *p;
    int t;

    for (p = list; p; p = p -> next) {
        if (p->from->successors.inverse[p->to->job])
            return 1;
        if (p->from->predecessors.inverse[p->to->job])
            return 2;
    }
    for (p = list; p; p = p -> next)
        newedge (p -> from, p -> to);
    return 0;
}

undoedge (list) edgelist *list;
{
    edgelist *p;
    int t;

    for (p = list; p; p = p -> next) {
        if ((t = testedge (p -> from, p -> to)) <= 0) {
            fprintf (stderr, "Whoa, undoedge going to return %d\n",
                     t == 0 ? 1 : 2);
            return t == 0 ? 1 : 2;
        }
        if ((t = testedge (p -> to, p -> from)) >= 0) {
            fprintf (stderr, "Whoa, undoedge going to return %d\n",
                     t == 0 ? 1 : 2);
            return t == 0 ? 1 : 2;
        }
    }
    for (p = list; p; p = p -> next)
        killedge (p -> from, p -> to);
    return propagate (list);
}

testedge (from, to)
jobstep *from;
jobstep *to;
{
    if (from->successors.inverse[to->job]) {
            return 1;
          }
    if (from->predecessors.inverse[to->job]) {
            return -1;
    }
    return 0;
}

newedge (from, to)
jobstep *from;
jobstep *to;
{
    from->successors.inverse[to->job] = from->successors.setend;
    *from -> successors.setend++ = to;
    to->predecessors.inverse[from->job] = to->predecessors.setend;
    *to -> predecessors.setend++ = from;
}

undo_newedges (list)
edgelist *list;
{
    while (list) {
        list -> from -> successors.setend--;
        list -> from -> successors.inverse[list->to->job] = 0;
        list -> to -> predecessors.setend--;
        list -> to -> predecessors.inverse[list->from->job] = 0;
        list = list -> next;
    }
}

killedge (from, to)
jobstep *from;
jobstep *to;
{
    jobstep **x;

    x = from->successors.inverse[to->job];
    *x = *--from -> successors.setend;
    from -> successors.inverse[(*x)->job] = x;
    from -> successors.inverse[to->job] = 0;
    x = to->predecessors.inverse[from->job];
    *x = *--to -> predecessors.setend;
    to->predecessors.inverse[(*x)->job] = x;
    to->predecessors.inverse[from->job] = 0;
}

propagate (list)
edgelist *list;
{
    edgelist *p;
    int tmpcnt;

    for (p = list; p; p = p -> next) {
        if (!waitforward (p -> to)) {        /* waitforward can only fail if
                                         * adding edges */
            zero_waitcnt ();
            undo_newedges (list);
            return 2;
        }
    }
    for (p = list; p; p = p -> next)
        numforward (p -> to);
    for (p = list; p; p = p -> next) {
        tmpcnt = p -> from -> waitcnt;
        waitbackward (p -> from);
        if (p -> from -> waitcnt > tmpcnt + 1) {
            fprintf (stderr, "Error - waitbackward looped\n");
            exit (1);
        }
    }
    for (p = list; p; p = p -> next)
        numbackward (p -> from);
    return 0;
}

waitforward (p)
jobstep *p;
{
    jobstep **q, **qend;

    if (p -> onstack)
        return 0;
    if (p -> waitcnt++ == 0) {
        p -> onstack = 1;
        if (p -> jsucc)
            if (!waitforward (p -> jsucc))
                return 0;
        for (q = p -> successors.set, qend = p -> successors.setend; 
             q < qend; q++)
            if (!waitforward (*q))
                return 0;
        p -> onstack = 0;
    }
    return 1;
}

numforward (p)
jobstep *p;
{
    jobstep **q, **qend = p->predecessors.setend;
    int v;
    int sump;
#if        NEW
    jobstep **r, **s;
    jobstep *t;
#else        /* NEW */
    jobstep *sorthead;
    jobstep **x;
#endif        /* NEW */
    
    if (--p -> waitcnt == 0) {
#if        NEW
        for (q = p->predecessors.set+1; q < qend; q++) {
          t = *q;
          v = t->head;
          for (r = q, s = r-1; s >= p->predecessors.set && (*s)->head < v; 
               r=s, --s) {
            *r = *s;
            p->predecessors.inverse[(*s)->job] = r;
          }
          *r = t;
          p->predecessors.inverse[t->job] = r;
        }
        v = p -> jpred ? p -> jpred -> head + p -> jpred -> body : 0;
        sump = 0;
        for (q = p->predecessors.set; q < qend; q++) {
          sump += (*q)->body;
            if (sump + (*q) -> head > v)
              v = sump + (*q) -> head;
        }
#else        /* NEW */
        v = p -> jpred ? p -> jpred -> head + p -> jpred -> body : 0;
        sorthead = 0;
        for (q = p -> predecessors.set; q < qend; q++) {
            INSERT (**q, sorthead, head, >, next, x);
        }
        sump = 0;
        while (sorthead) {
            sump += sorthead -> body;
            if (sump + sorthead -> head > v)
                v = sump + sorthead -> head;
            sorthead = sorthead -> next;
        }
#endif        /* NEW */
        p -> head = v;
        if (p -> jsucc)
            numforward (p -> jsucc);
        for (q = p -> successors.set, qend = p -> successors.setend;
             q < qend; q++)
            numforward (*q);
    }
}

waitbackward (p)
jobstep *p;
{
    jobstep **q, **qend;

    if (p -> onstack)
        return 0;
    if (p -> waitcnt++ == 0) {
        p -> onstack = 1;
        if (p -> jpred)
            if (!waitbackward (p -> jpred))
                return 0;
        for (q = p -> predecessors.set, qend = p -> predecessors.setend;
             q < qend; q++)
            if (!waitbackward (*q))
                return 0;
        p -> onstack = 0;
    }
    return 1;
}

numbackward (p)
jobstep *p;
{
    jobstep **q, **qend = p->successors.setend;
    int v;
    int sump;
#if        NEW
    jobstep **r, **s;
    jobstep *t;
#else        /* NEW */
    jobstep *sorthead;
    jobstep **x;
#endif        /* NEW */

    if (--p -> waitcnt == 0) {
#if        NEW
        for (q = p->successors.set+1; q < qend; q++) {
          t = *q;
          v = t->tail;
          for (r = q, s = r-1; s >= p->successors.set && (*s)->tail < v;
               r=s, --s) {
            *r = *s;
            p->successors.inverse[(*s)->job] = r;
          }
          *r = t;
          p->successors.inverse[t->job] = r;
        }
        v = p -> jsucc ? p -> jsucc -> tail + p -> jsucc -> body : 0;
        sump = 0;
        for (q = p->successors.set; q < qend; q++) {
          sump += (*q)->body;
            if (sump + (*q) -> tail > v)
              v = sump + (*q) -> tail;
        }
#else        /* NEW */
        v = p -> jsucc ? p -> jsucc -> tail + p -> jsucc -> body : 0;
        sorthead = 0;
        for (q = p -> successors.set; q < qend; q++) {
            INSERT (**q, sorthead, tail, >, next, x);
        }
        sump = 0;
        while (sorthead) {
            sump += sorthead -> body;
            if (sump + sorthead -> tail > v)
                v = sump + sorthead -> tail;
            sorthead = sorthead -> next;
        }
#endif        /* NEW */
        p -> tail = v;
        if (p -> jpred)
            numbackward (p -> jpred);
        for (q = p -> predecessors.set, qend = p -> predecessors.setend;
             q < qend; q++)
            numbackward (*q);
    }
}

zero_waitcnt()
{
    int i, j;

    for (i = 0; i < num_of_machines; i++) {
        for (j = 0; j < num_of_jobs; j++) {
            schedule[i].jobs[j].waitcnt = 0;
            schedule[i].jobs[j].onstack = 0;
        }
    }
}
