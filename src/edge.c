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

extern int bnbeval;
extern int depth;
extern int limit;
extern char searchvector[];
extern machine *machkill;

extern edgelist *alloc_edgelist();
extern setchangelist *alloc_setchangelist();

muncher ()
{
    edgelist *edges = 0;
    setchangelist *setchngs = 0;
    edgelist newedge;
    int value;
    jobstep *t;

    bnbeval++;
    depth++;
    searchvector[depth] = '\0';
    if ((bnbeval & 0x3f) == 0) {
        printf ("%d evals, limit %d depth %d search %s\n", bnbeval, limit,
                depth, searchvector+1);
        fflush (stdout);
    }
    if (bigmunch (&edges, &setchngs)) {
        value = lbound ((machine **) 0);
        if (value <= limit) {
            if (splitit (&newedge, value)) {
                switch (doedge (&newedge)) {
                  case 0:
                    searchvector[depth] = '0';
                    muncher ();
                    if (undoedge (&newedge)) {
                        fprintf (stderr, "Error 1, undoedge failed\n");
                        exit (1);
                    }
                    break;
                  case 1:
                    fprintf (stderr, "Hmm, branch doedge = 1\n");
                    exit (1);
                }
                if (value <= limit) {
                    t = newedge.from;
                    newedge.from = newedge.to;
                    newedge.to = t;
                    switch (doedge (&newedge)) {
                      case 0:
                        searchvector[depth] = '1';
                        muncher ();
                        if (undoedge (&newedge)) {
                            fprintf (stderr, "Error 2, undoedge failed\n");
                            exit (1);
                        }
                        break;
                      case 1:
                        fprintf (stderr, "Hmm, branch doedge = 1\n");
                        exit (1);
                    }
                }
            } else {
                printf ("New solution found, value %d\n", value);
                printf ("head times:\n");
                dumpheads ();
                saveheads ();
                if (value - 1 < limit)
                    limit = value - 1;
            }
        }
    }

    if (undoedge (edges)) {
        fprintf (stderr, "Error 3, undoedge failed\n");
        exit (1);
    }
    fixset (&setchngs);
    freelist (edges);
    searchvector[depth] = '\0';
    --depth;
}

checkprop67(m, newedges, edges)
machine *m;
int *newedges;
edgelist **edges;
{
    jobstep *ehead, *ep, *fhead, *fp, *p, *p1;
    jobstep **jp, **jpend;
    jobstep **x;
    int et,ft;
    int psum, thresh;
    int newedgehold;
    edgelist *myedges = 0;
    
    ehead = 0;
    for (jp = m -> cset.set, jpend = m->cset.setend; jp < jpend; jp++) {
        INSERT (**jp, ehead, head, >, next2, x);
    }
    ep = ehead;
    fhead = 0;
    while (ep) {
        et = ep -> head;
        for (et = ep -> head; ep && ep -> head == et; ep = p) {
            p = ep -> next2;
            INSERT (*ep, fhead, tail, >, next2, x);
        }
        fp = fhead;
        psum = 0;
        while (fp) {
            for (ft = fp -> tail; fp && fp -> tail == ft; fp = fp -> next2) {
                psum += fp -> body;
            }
            thresh = limit - psum - et - ft;
            if (thresh < m -> maxbody) {
                for (p = ep; p; p = p -> next2) {
                    if (p -> body > thresh) {
                        if (psum + et + p -> tail + p -> body > limit) {
                            for (p1 = fhead; p1 != fp; p1 = p1 -> next2)
                                if (!donewedge_noprop (p, p1, &myedges,
                                                       newedges)) {
                                    undoedge (myedges);
                                    freelist (myedges);
                                    return 0;
                                  }
                        }
                        if (psum + ft + p -> head + p -> body > limit) {
                            for (p1 = fhead; p1 != fp; p1 = p1 -> next2)
                                if (!donewedge_noprop (p1, p, &myedges,
                                                       newedges)) {
                                    undoedge (myedges);
                                    freelist (myedges);
                                    return 0;
                                  }
                        }
                    }
                }
                for (p = fp; p; p = p -> next2) {
                    if (p -> body > thresh) {
                        if (psum + et + p -> tail + p -> body > limit) {
                            for (p1 = fhead; p1 != fp; p1 = p1 -> next2)
                                if (!donewedge_noprop (p, p1, &myedges,
                                                       newedges)) {
                                    undoedge (myedges);
                                    freelist (myedges);
                                    return 0;
                                  }
                        }
                        if (psum + ft + p -> head + p -> body > limit) {
                            for (p1 = fhead; p1 != fp; p1 = p1 -> next2)
                                if (!donewedge_noprop (p1, p, &myedges,
                                                       newedges)) {
                                    undoedge (myedges);
                                    freelist (myedges);
                                    return 0;
                                  }
                        }
                    }
                }
            }
        }
    }
    if (propagate (myedges) == 2) {
      undoedge (myedges);
      freelist (myedges);
      return 0;
    } else {
      addtolist (edges, myedges);
      return 1;
    }
}

edgefromset (j, s)
jobstep *j;
jobset *s;
{
  jobstep **j1, **j1end;
  
    for (j1 = s->set, j1end = s->setend; j1 < j1end; j1++)
        if (*j1 != j && (*j1)->successors.inverse[j->job])
          return 1;
    return 0;
}

edgetoset (j, s)
jobstep *j;
jobset *s;
{
  jobstep **j1, **j1end;
  
    for (j1 = s->set, j1end = s->setend; j1 < j1end; j1++)
        if (*j1 != j && j->successors.inverse[(*j1)->job])
          return 1;
    return 0;
}

findnewmax(m)
machine *m;
{
      int max = 0;
      jobstep **q, **qend;

      for (q=m->cset.set,qend = m->cset.setend; q < qend; q++)
            if ((*q)->body > max) max = (*q)->body;
      m->maxbody = max;
}

bigmunch (edges, setchngs)
edgelist **edges;
setchangelist **setchngs;
{
    int newedges;
    jobstep **j, **jend, **j1, **j1end;
    jobstep *jp;
    int mina, minc;
    machine *m, *mend;
    int csump;
    int changed;
    int pre;
    edgelist *myedges = 0;

    do {
        newedges = 0;
        for (m=schedule, mend = m+num_of_machines; m < mend; m++) {
            pre = preemptive(m);
            if (pre > limit) return 0;
            if (pre + m->maxbody > limit) {
                if (!checkprop67(m, &newedges, edges))
                    return 0;
            }
            do {
                changed = 0;
                j = m->eset.set;
                jend = m->eset.setend;
                csump = m->cset_sum;
                while (j < jend && m->eset.setend > m->eset.set+1) {
                    if (edgefromset (*j, &m->cset)) {
                        saveset (setchngs, m, *j, DEL_FROM_ESET);
                        *j = *--m->eset.setend;
                        --jend;
                        continue;
                    }
                    minc = 32767;
                    for (j1 = m->sset.set, j1end = m->sset.setend;
                         j1 < j1end; j1++) {
                        if (*j != *j1) {
                            if ((*j1) -> tail < minc)
                                minc = (*j1) -> tail;
                        }
                    }
                    if ((*j) -> head + csump + minc > limit) {
                        saveset (setchngs, m, *j, DEL_FROM_ESET);
                        *j = *--m->eset.setend;
                        --jend;
                        continue;
                    }
                    j++;
                }
                if (m->eset.setend == m->eset.set+1) {
                    jp = m->eset.set[0];
                    for (j = m->cset.set, jend=m->cset.setend; j<jend; j++) {
                        if (*j != jp)
                            if (!donewedge_noprop (jp, *j, &myedges,
                                                   &newedges)) {
                                undoedge (myedges);
                                freelist (myedges);
                                return 0;
                              }
                    }
                    if (propagate (myedges) == 2) {
                      undoedge (myedges);
                      freelist (myedges);
                      return 0;
                    } else {
                      addtolist (edges, myedges);
                    }
                    myedges = 0;

                    m->cset.setend -= 1;
                    m->eset.setend = m->eset.set;
                    for (j = m->cset.set, jend=m->cset.setend; j<jend; j++) {
                        if (*j == jp)
                            *j = *m->cset.setend;
                        *m->eset.setend++ = *j;
                    }
                    saveset (setchngs, m, jp, RESET_ESET);
                    if (jp -> body == m->maxbody)
                          findnewmax(m);
                    m->cset_sum -= jp -> body;
                    csump -= jp->body;
                   for (j = m->sset.set, jend = m->sset.setend; j<jend; j++) {
                        if (*j == jp) {
                            *j = *--m->sset.setend;
                            --jend;
                            saveset (setchngs, m, jp, DEL_FROM_SSET);
                            break;
                        }
                    }
                    changed++;
                }
                j = m->sset.set;
                jend = m->sset.setend;
                while (j < jend && m->sset.setend > m->sset.set + 1) {
                    if (edgetoset (*j, &m->cset)) {
                            saveset (setchngs, m, *j, DEL_FROM_SSET);
                            *j = *--m->sset.setend;
                            --jend;
                            continue;
                    }
                    mina = 32767;
                    for (j1 = m->eset.set, j1end = m->eset.setend; 
                         j1 < j1end; j1++) {
                        if (*j != *j1) {
                            if ((*j1) -> head < mina)
                                mina = (*j1) -> head;
                        }
                    }
                    if (mina + csump + (*j) -> tail > limit) {
                        saveset (setchngs, m, *j, DEL_FROM_SSET);
                        *j = *--m->sset.setend;
                        --jend;
                        continue;
                    }
                    j++;
                }
                if (m->sset.setend == m->sset.set + 1) {
                    jp = m->sset.set[0];
                    for (j = m->cset.set, jend=m->cset.setend; j<jend; j++) {
                        if (*j != jp)
                            if (!donewedge_noprop (*j, jp, &myedges,
                                                   &newedges)) {
                                undoedge (myedges);
                                freelist (myedges);
                                return 0;
                              }
                    }
                    if (propagate (myedges) == 2) {
                      undoedge (myedges);
                      freelist (myedges);
                      return 0;
                    } else {
                      addtolist (edges, myedges);
                    }
                    myedges = 0;

                    m->cset.setend -= 1;
                    m->sset.setend = m->sset.set;
                    for (j = m->cset.set, jend=m->cset.setend; j<jend; j++) {
                        if (*j == jp)
                            *j = *m->cset.setend;
                        *m->sset.setend++ = *j;
                    }
                    saveset (setchngs, m, jp, RESET_SSET);
                    if (jp -> body == m->maxbody)
                          findnewmax(m);
                    m->cset_sum -= jp->body;
                    csump -= jp->body;
                   for (j = m->eset.set, jend = m->eset.setend; j<jend; j++) {
                        if (*j == jp) {
                            *j = *--m->eset.setend;
                            --jend;
                            saveset (setchngs, m, jp, DEL_FROM_ESET);
                            break;
                        }
                    }
                    changed++;
                }
            } while (changed);
        }
    } while (newedges);
    return 1;
}

saveset (setchngs, m, j, type)
setchangelist **setchngs;
machine *m;
jobstep *j;
int type;
{
        setchangelist *p;

        p = alloc_setchangelist();
        p->m = m;
        p->j = j;
        p->type = type;
        p->next = *setchngs;
        *setchngs = p;
}

fixset (setchngs)
setchangelist **setchngs;
{
    setchangelist *p, *pnew;

    for (p=(*setchngs); p; p = pnew) {
        switch (p->type) {
            case DEL_FROM_ESET:
                *p->m->eset.setend++ = p->j;
                break;
            case DEL_FROM_SSET:
                *p->m->sset.setend++ = p->j;
                break;
            case RESET_ESET:
                *p->m->cset.setend++ = p->j;
                p->m->cset_sum += p->j->body;
                p->m->eset.setend = p->m->eset.set;
                *p->m->eset.setend++ = p->j;
                if (p->j->body > p->m->maxbody)
                      p->m->maxbody = p->j->body;
                break;
            case RESET_SSET:
                *p->m->cset.setend++ = p->j;
                p->m->cset_sum += p->j->body;
                p->m->sset.setend = p->m->sset.set;
                *p->m->sset.setend++ = p->j;
                if (p->j->body > p->m->maxbody)
                      p->m->maxbody = p->j->body;
                break;
        }
        pnew = p->next;
        free_setchangelist (p);
    }
}

addtolist (list, new)
edgelist **list;
edgelist *new;
{
  edgelist *p;

  while (new) {
    p = new->next;
    new->next = *list;
    *list = new;
    new = p;
  }
}

donewedge (from, to, elist, new)
jobstep *from;
jobstep *to;
edgelist **elist;
int *new;
{
        edgelist *p;

        p = alloc_edgelist();
        p->from = from;
        p->to = to;
        p->next = 0;
        switch (doedge(p)) {
            case 2:
                free_edgelist (p);
                return 0;
            case 1:
                free_edgelist (p);
                break;
            case 0:
                p->next = *elist;
                *elist = p;
                ++*new;
                break;
        }
        return 1;
}

donewedge_noprop (from, to, elist, new)
jobstep *from;
jobstep *to;
edgelist **elist;
int *new;
{
        edgelist *p;

        p = alloc_edgelist();
        p->from = from;
        p->to = to;
        p->next = 0;
        switch (doedge_noprop(p)) {
            case 2:
                free_edgelist (p);
                return 0;
            case 1:
                free_edgelist (p);
                break;
            case 0:
                p->next = *elist;
                *elist = p;
                ++*new;
                break;
        }
        return 1;
}

splitit (newedg, v)
edgelist *newedg;
int v;
{
    int mincard;
    int bv1, bv2;
    machine *m, *mend;

    bv1 = -1;
    bv2 = -1;

    newedg -> next = 0;
    newedg -> from = 0;
    newedg -> to = 0;

    if (machkill -> cset.setend > machkill->cset.set) {
        if (machkill -> eset.setend - machkill->eset.set <=
            machkill -> sset.setend - machkill->sset.set)
            setsplit (&machkill -> eset, newedg, v, &bv1, &bv2);
        if (machkill -> sset.setend - machkill->sset.set <=
            machkill -> eset.setend - machkill->eset.set)
            setsplit (&machkill -> sset, newedg, v, &bv1, &bv2);
    } else {
        mincard = 32767;
        for (m = schedule, mend = m + num_of_machines; m < mend; m++) {
            if (m -> eset.setend > m->eset.set &&
                m -> eset.setend - m->eset.set < mincard)
                mincard = m -> eset.setend - m->eset.set;
            if (m -> sset.setend > m->sset.set &&
                m -> sset.setend - m->sset.set < mincard)
                mincard = m -> sset.setend - m->sset.set;
        }
        if (mincard == 32767)
            return 0;
        else {
            for (m = schedule, mend = m + num_of_machines; m < mend; m++) {
                if (m -> eset.setend - m->eset.set == mincard)
                    setsplit (&m -> eset, newedg, v, &bv1, &bv2);
                if (m -> sset.setend - m->sset.set == mincard)
                    setsplit (&m -> sset, newedg, v, &bv1, &bv2);
            }
        }
    }
    return 1;
}

setsplit (s, newedg, v, bv1, bv2)
jobset *s;
edgelist *newedg;
int v;
int *bv1, *bv2;
{
    int d1, d2;
    jobstep **i, **iend, **j;
    jobstep *i1, *j1;

    for (i = s->set, iend = s -> setend - 1; i < iend; i++) {
        i1 = *i;
        for (j = i + 1; j <= iend; j++) {
            j1 = *j;
            d1 = i1 -> head + i1 -> body + j1 -> body + j1 -> tail - v;
            if (d1 < 0)
                d1 = 0;
            d2 = j1 -> head + j1 -> body + i1 -> body + i1 -> tail - v;
            if (d2 < 0)
                d2 = 0;
            if (d1 <= d2) {
                if (d2 - d1 > *bv1 || (d2 - d1 == *bv1 && d1 > *bv2)) {
                    *bv1 = d2 - d1;
                    *bv2 = d1;
                    newedg -> from = i1;
                    newedg -> to = j1;
                }
            } else {
                if (d1 - d2 > *bv1 || (d1 - d2 == *bv1 && d2 > *bv2)) {
                    *bv1 = d1 - d2;
                    *bv2 = d2;
                    newedg -> from = j1;
                    newedg -> to = i1;
                }
            }
        }
    }
}

dumpheads ()
{
        int i;
        int j;

        for (i=0; i<num_of_jobs; i++) {
            for (j=0; j<num_of_machines; j++)
                printf ("%4d ",schedule[j].jobs[i].head);
            printf ("\n");
        }
}

dumptails ()
{
        int i;
        int j;

        for (i=0; i<num_of_jobs; i++) {
            for (j=0; j<num_of_machines; j++)
                printf ("%4d ",schedule[j].jobs[i].tail);
            printf ("\n");
        }
}

dumpsaveset(p)
setchangelist *p;
{
    while (p) {
        switch (p->type) {
            case DEL_FROM_ESET:
                printf ("del job %d from %d's eset\n",
                        p->j-p->m->jobs,p->m-schedule);
                break;
            case DEL_FROM_SSET:
                printf ("del job %d from %d's sset\n",
                        p->j-p->m->jobs,p->m-schedule);
                break;
            case RESET_ESET:
                printf ("reset %d's eset del job %d\n",
                        p->m-schedule,p->j-p->m->jobs);
                break;
            case RESET_SSET:
                printf ("reset %d's sset del job %d\n",
                        p->m-schedule,p->j-p->m->jobs);
                break;
        }
        p=p->next;
    }
}

dumpedgelist (p)
edgelist *p;
{
  while (p) {
    printf ("<%d,%d>-<%d,%d>\n",p->from->job,p->from->machine,p->to->job,
            p->to->machine);
    p=p->next;
  }
}

freelist (p)
edgelist *p;
{
        edgelist *pnew;

        while (p) {
            pnew = p->next;
            free_edgelist (p);
            p = pnew;
        }
}

#define SPACECHUNK 1000

static setchangelist *setchangelist_avail = 0;

setchangelist *alloc_setchangelist()
{
    setchangelist *space, *spaceend;
    setchangelist *p;

    if (!setchangelist_avail) {
        CALLOC (space, SPACECHUNK, setchangelist);
        for (spaceend = space + SPACECHUNK; space < spaceend; space++) {
            space->next = setchangelist_avail;
            setchangelist_avail = space;
        }
    }
    p = setchangelist_avail;
    setchangelist_avail = setchangelist_avail->next;
    return p;
}

free_setchangelist(p)
setchangelist *p;
{
        p->next = setchangelist_avail;
        setchangelist_avail = p;
}

static edgelist *edgelist_avail = 0;

edgelist *alloc_edgelist()
{
    edgelist *space, *spaceend;
    edgelist *p;

    if (!edgelist_avail) {
        CALLOC (space, SPACECHUNK, edgelist);
        for (spaceend = space + SPACECHUNK; space < spaceend; space++) {
            space->next = edgelist_avail;
            edgelist_avail = space;
        }
    }
    p = edgelist_avail;
    edgelist_avail = edgelist_avail->next;
    return p;
}

free_edgelist(p)
edgelist *p;
{
        p->next = edgelist_avail;
        edgelist_avail = p;
}
