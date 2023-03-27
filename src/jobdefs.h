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

typedef struct jobset {
        struct jobstep **set;
        struct jobstep **setend;
} jobset;

typedef struct jobset2 {
        struct jobstep **set;
        struct jobstep **setend;
        struct jobstep ***inverse;
} jobset2;

#define DEL_FROM_ESET 1
#define DEL_FROM_SSET 2
#define RESET_ESET 3
#define RESET_SSET 4

typedef struct setchangelist {
    struct machine *m;
    struct jobstep *j;
    int type;
    struct setchangelist *next;
} setchangelist;

typedef struct jobstep {
    int job;                        /* used for succ/pred inverses */
    int machine;                /* useless? */
    int head;                        /* setup time */
    int body;                        /* processing time */
    int tail;                        /* finishing time */
    struct jobstep *jsucc;   /* doubly linked list of machines for this job */
    struct jobstep *jpred;
    jobset2 successors;                /* successor jobs on this machine */
    jobset2 predecessors;        /* predecessor jobs on this machine */
    int waitcnt;                /* used in edges */
    int onstack;                /* used in edges */
    struct jobstep *next;        /* for sorted lists of jobs */
    struct jobstep *next2;
} jobstep;

typedef struct machine {
    jobstep *jobs;                /* jobs for this machine */
    jobstep **headorder;          /* pointers to jobs sorted by head time 
                                     in increasing order */
    int cset_sum;
    jobset cset;
    jobset eset;
    jobset sset;
    int maxbody;
} machine;

extern machine *schedule;        /* machines for this problem */
extern jobstep **jobhead;        /* jobs linked by jsucc */

extern int num_of_jobs;
extern int num_of_machines;

typedef struct edgelist {
    jobstep *from;
    jobstep *to;
    struct edgelist *next;
} edgelist;

#define STARTOFTIME -1000000
#define ENDOFTIME 1000000

char *malloc();
char *calloc();
#define MALLOC(x,num,type) do {                                           \
        if ((x = (type *) malloc ((unsigned) ((num) * sizeof (type))))    \
             == NULL) {                                                 \
                fprintf (stderr, "Out of memory\n");                    \
                exit (1);                                               \
        }} while (0)

#define CALLOC(x,num,type) do {                                           \
        if ((x = (type *) calloc ((unsigned) (num), sizeof (type)))     \
             == NULL) {                                                 \
                fprintf (stderr, "Out of memory\n");                    \
                exit (1);                                               \
        }} while (0)

#define INSERT(object, list, field, order, next, temp) do {                \
        for ((temp) = &list;                                                \
             (*(temp)) && (*(temp))->field order (object).field;        \
             (temp) = &((*(temp))->next));                                \
        (object).next = (*(temp));                                        \
        *(temp) = &(object);                                                \
} while (0)
