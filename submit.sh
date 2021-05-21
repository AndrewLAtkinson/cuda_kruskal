#!/bin/bash
#$ -cwd
#$ -l h_rt=04:00:00
mpirun mpi.out $ARG1
