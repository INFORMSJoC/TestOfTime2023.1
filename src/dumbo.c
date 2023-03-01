/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"

machine *schedule;
jobstep **jobhead;
char *probname;

int num_of_jobs;
int num_of_machines;

int limit, inlimit;

char *filename;
char *sched_file;

int bnbeval = 0;
int depth = 0;
char searchvector[1024];

int first_machine = 1;

typedef struct bnbnode {
    int value;
    edgelist *branchlist;
    struct bnbnode *next;
} bnbnode;

edgelist *alloc_edgelist();
setchangelist *alloc_setchangelist();
bnbnode *alloc_bnbnode();

main(ac, av)
int ac;
char **av;
{
    int i;
    bnbnode *bnblist, *bnbnext;
    edgelist *edges;
    int holdval;
    int bestv1, bestv2;
    jobstep *bestj1, *bestj2;
    
    parseargs (ac, av);

    printf ("filename: %s\n",filename);
    getprob (filename);

    limit = inlimit;

    initialize ();

    bnblist = alloc_bnbnode();
    bnblist->next = 0;
    bnblist->value = lbound(0);
    bnblist->branchlist = 0;
    bnbeval = 0;
    
    printf ("Initial bound %d\n", bnblist->value);

    if (sched_file) {
            getsched (sched_file);
        bnblist->value = lbound(0);
        printf ("Bound after forced schedule %d\n",bnblist->value);
    }

    while (bnblist) {
        bnbnext = bnblist;
        bnblist = bnblist->next;
        bnbeval++;
        printf ("Node %d: value = %d\n",bnbeval,bnbnext->value);
        holdval = bnbnext->value;
        if (holdval > limit)
            break;
        edges = bnbnext->branchlist;
        (void) doedge(edges);
        free_bnbnode (bnbnext);
        if (munchit(&bestv1, &bestv2, &bestj1, &bestj2, &edges)) {
            printf ("    split into %d %d\n",bestv1,bestv2);
            splitit(&bnblist, bestv1, bestv2, bestj1, bestj2, edges);
        } else {
            printf ("%d (%d) killed\n",bnbeval,holdval);
            fflush (stdout);
        }
        (void) undoedge(edges);
    }
            
    printf ("%d nodes evaluated, %s = %d\n",bnbeval,
        limit == inlimit ? "lower bound" : "optimal solution", limit+1);
    fflush (stdout);
    exit (1);
}

parseargs(ac, av)
int ac;
char **av;
{
    int c;
    extern int optind;
    extern char *optarg;

    while ((c = getopt (ac, av, "s:0")) != EOF) switch (c) {
        case 's':
            sched_file = optarg;
            break;
        case '0':
            first_machine = 0;
            break;
        default:
            usage (*av);
    }
    if (optind >= ac)
        usage (*av);
    filename = av[optind++];
    if (optind >= ac)
        usage (*av);
    inlimit = atoi(av[optind++]);
    if (optind < ac)
        usage (*av);
}

usage (prog)
char *prog;
{
    fprintf (stderr, "Usage: %s [-s schedule_file] [-0] probfile bound\n",
                     prog);
    fprintf (stderr,
            "    -s: a file containing a list of machines to pre-schedule\n");
    fprintf (stderr, "    -0: machine and job #'s start from 0, not 1\n");
    exit (1);
}

getsched (f)
char *f;
{
        FILE *fin;
        int m;
        edgelist e;
        int last;
        int this;
        int i;

        if ((fin = fopen (f, "r")) == NULL) {
            perror (f);
            fprintf (stderr, "Unable to open schedule file for input\n");
            exit (1);
        }

        while (fscanf (fin, " %d ", &m) == 1) {
            if (fscanf (fin, " %d ", &last) != 1) {
                fprintf (stderr, "Syntax error in schedule file\n");
                exit (1);
            }
            for (i=1; i<num_of_jobs; i++) {
                if (fscanf (fin, " %d ", &this) != 1) {
                    fprintf (stderr, "Syntax error in schedule file\n");
                    exit (1);
                }
                e.from = &schedule[m-first_machine].jobs[last-first_machine];
                e.to = &schedule[m-first_machine].jobs[this-first_machine];
                e.next = 0;
                doedge(&e);
                last = this;
            }
            printf ("machine %d pre-scheduled\n",m);
        }
        fclose (fin);
}

munchit (bestv1, bestv2, bestj1, bestj2, edges)
int *bestv1;
int *bestv2;
jobstep **bestj1;
jobstep **bestj2;
edgelist **edges;
{
        int newedges;
        machine *m, *mend;
        int v1;
        int mbestv, medgechk, bestv, bedgechk, edgechk, mbestv1, mbestv2;
        jobstep *mbestj1, *mbestj2;

        do {
            newedges = 0;
            edgechk = 0;
            bestv = -1;
            bedgechk = 32767;
            for (m = schedule, mend = m+num_of_machines; m<mend; m++) {
                if (!munmach(m,edges, &newedges, &mbestv, &medgechk, &mbestj1,
                             &mbestj2, &mbestv1, &mbestv2))
                    return 0;
                if (medgechk > 0) {
                    edgechk += medgechk;
                    if (mbestv > bestv || (mbestv == bestv &&
                                           medgechk < bedgechk)) {
                        bedgechk = medgechk;
                        *bestj1 = mbestj1;
                        *bestj2 = mbestj2;
                        *bestv1 = mbestv1;
                        *bestv2 = mbestv2;
                        bestv = mbestv;
                    }
                }
            }
            printf ("%d new edges found out of %d\n",newedges,edgechk);
            fflush (stdout);
            if (edgechk == 0) {
                v1 = lbound(0);
                printf ("solution found, value %d\n",v1);
                printf ("head times:\n");
                dumpheads();
                if (v1 - 1< limit)
                    limit = v1 - 1;
                return 0;
            }
        } while (newedges);
        return 1;
}

munmach(m, edges, newedges, mbestv, medgechk, mbestj1, mbestj2, mbestv1,
        mbestv2)
machine *m;
edgelist **edges;
int *newedges;
int *mbestv;
int *medgechk;
jobstep **mbestj1;
jobstep **mbestj2;
int *mbestv1;
int *mbestv2;
{
    edgelist newedge1, newedge2;
    jobstep *j, *jend, *k;
    int v1, v2, newv;
    edgelist *p;

    newedge1.next = 0;
    newedge2.next = 0;

    *mbestv = -1;
    *medgechk = 0;

    for (j = m -> jobs, jend = j + num_of_jobs - 1; j < jend; j++) {
        newedge1.from = j;
        newedge2.to = j;
        for (k = j + 1; k <= jend; k++) {
            if (testedge (j, k) == 0) {
                newedge1.to = k;
                newedge2.from = k;
                if (!doedge (&newedge1)) {
                    v1 = lbound (0);
                    (void) undoedge (&newedge1);
                    if (!doedge (&newedge2)) {
                        ++*medgechk;
                        v2 = lbound (0);
                        if (v1 > limit) {
                            if (v2 > limit) {
                                (void) undoedge (&newedge2);
                                return 0;
                            } else {
                                p = alloc_edgelist ();
                                p -> from = k;
                                p -> to = j;
                                p -> next = *edges;
                                *edges = p;
                                ++*newedges;
                            }
                        } else {
                            if (v2 > limit) {
                                (void) undoedge (&newedge2);
                                (void) doedge (&newedge1);
                                p = alloc_edgelist ();
                                p -> from = j;
                                p -> to = k;
                                p -> next = *edges;
                                *edges = p;
                                ++*newedges;
                            } else {
                                (void) undoedge (&newedge2);
                                if (v1 <= v2) {
                                    newv = v1 * 1000 + v2;
                                    if (newv > *mbestv) {
                                        *mbestv = newv;
                                        *mbestv1 = v1;
                                        *mbestv2 = v2;
                                        *mbestj1 = j;
                                        *mbestj2 = k;
                                    }
                                } else {
                                    newv = v2 * 1000 + v1;
                                    if (newv > *mbestv) {
                                        *mbestv = newv;
                                        *mbestv1 = v2;
                                        *mbestv2 = v1;
                                        *mbestj1 = k;
                                        *mbestj2 = j;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    /* return efind(); */
    return 1;
}

splitit (bnblist, v1, v2, j1, j2, edges)
bnbnode **bnblist;
int v1, v2;
jobstep *j1, *j2;
edgelist *edges;
{
    bnbnode *b1, *b2;
    edgelist *e1, *e2;
    bnbnode **p;

    b1 = alloc_bnbnode();
    b2 = alloc_bnbnode();
    b1->value = v1;
    b2->value = v2;
    e1 = alloc_edgelist();
    e2 = alloc_edgelist();
    e1->from = j1;
    e2->from = j2;
    e1->to = j2;
    e2->to = j1;
    e1->next = edges;
    e2->next = edges;
    b1->branchlist = e1;
    b2->branchlist = e2;
    INSERT(*b1,*bnblist,value,<,next, p);
    INSERT(*b2,*bnblist,value,<,next, p);
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

static bnbnode *bnbnode_avail = 0;

bnbnode *alloc_bnbnode()
{
    bnbnode *space, *spaceend;
    bnbnode *p;

    if (!bnbnode_avail) {
        CALLOC (space, SPACECHUNK, bnbnode);
        for (spaceend = space + SPACECHUNK; space < spaceend; space++) {
            space->next = bnbnode_avail;
            bnbnode_avail = space;
        }
    }
    p = bnbnode_avail;
    bnbnode_avail = bnbnode_avail->next;
    return p;
}

free_bnbnode(p)
bnbnode *p;
{
        p->next = bnbnode_avail;
        bnbnode_avail = p;
}
