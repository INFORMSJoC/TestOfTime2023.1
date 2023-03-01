[![INFORMS Journal on Computing Logo](https://INFORMSJoC.github.io/logos/INFORMS_Journal_on_Computing_Header.jpg)](https://pubsonline.informs.org/journal/ijoc)

# JobShop

JobShop is a set of C programs for the job-shop scheduling problem. The codes are based on the paper "A computational study of the job-shop scheduling problem" by David Applegate and William Cook, which appeared in ORSA Journal on Computing, Vol 3 No 2, 1991. A set of test problems is included in the gzipped tarfile prob.tar.gz.

## Cite

To cite the contents of this respository, please cite both the paper and this repo, using their respective DOIs.

https://doi.org/10.1287/ijoc.3.2.149

https://github.com/INFORMSJoC/TestOfTime2023.1 **(REPLACE with the DOI)**

Below is the BibTex for citing this snapshot of the respoitory.

```
@article{CacheTest,
  author =        {T. Ralphs},
  publisher =     {INFORMS Journal on Computing},
  title =         {{JobShop}},
  year =          {1990},
  doi =           {**REPLACE with the DOI**},
  url =           {https://github.com/INFORMSJoC/TestOfTime2023.1},
}  
```

## Description

The codes are written in the old Kernighan & Ritchie style C and tested on a variety of unix workstations in 1990. They are certainly old, but still compile with recent versions of gcc (albeit with a number of warnings). The included Makefile will let you build command-line executables for algorithms described in the research paper.

## Command-line Interface

DUMBO: An implementation of the naive branch and bound algorithm.

Usage: dumbo [-s schedule_file] [-0] probfile bound 
         -s: a file containing a list of machines to pre-schedule 
         -0: machine and job #'s start from 0, not 1 

EDGE: An implementation of edge_finder.

Usage: edge [-s schedule_file] [-0] probfile bound
         -s: a file containing a list of machines to pre-schedule
         -0: machine and job #'s start from 0, not 1

BOTTLE: An implementation of the shifting bottleneck algorithm of Adams, Balas, and Zawack, as described in Applegate and Cook.

Usage bottle: [-n: process_name] [-s] jobshop_file
         -s simple run

SLUG: An implementaion of the shuffle heuristic.

Usage slug: [-n integer] [-b integer] [-o integer] [-r] prob_file etimes_file
         -n: number of machines to fix
         -b: bound on the solution wanted
         -o: fix only this machine
         -r: reverse the rule for choosing the machines to fix

ONEMACH: An implementation of Carlier's single machine algorithm.

Usage onemach: /* INTERACTIVE */
