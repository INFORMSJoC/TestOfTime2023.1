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

printprob (probname)
char *probname;
{
    int i, j;
    
    printf ("%s\n\n", probname);
    printf ("                    Starting Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].head);
        putchar ('\n');
    }
    printf ("\n                    Processing Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].body);
        putchar ('\n');
    }
    printf ("\n                    Finishing Times\n\n   ");
    for (i = 0; i < num_of_machines; i++)
        printf ("   %2d ", i + 1);
    putchar ('\n');
    for (i = 0; i < num_of_jobs; i++) {
        printf ("%2d:", i + 1);
        for (j = 0; j < num_of_machines; j++)
            printf (" %5d", schedule[j].jobs[i].tail);
        putchar ('\n');
    }
}
