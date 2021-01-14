# Concurrency — Operatings Systems Assignment 4 

This repository contains my solutions for the concurrency assignment given in the Operating Systems & Networking course I took at IIIT, Hyderabad during my 3rd semester.
I have presented a summary about the problem and implementation below. Please check the file `questions.pdf` to know more about the details of the questions.

## Q1 — Concurrent Merge Sort

Merge Sort is implemented in two different forms:

1. Using processes 
2. Using threads (through POSIX library)

Performance comparison of these variants and the traditional Merge Sort is also presented in the README.txt 

## Q2 — Back to College 

Problem represented a hypothetical scenario of simulating students being called back to college and vaccinating them in predefined zones. Each student, vaccination zone, and pharmaceutical company is represented by a separate thread and the given situation is simulated via busy waiting.

## Q3 — Musical Mayhem

Problem represented a mini-world of simulating performances of different artists in a musical concert. I had implemented a multi-threaded program using mutex locks semaphores and mutex locks (via binary semaphores to facilitate inter-library communication) to synchronize the performance of multiple artists simultaneously. Correct and efficient implementation of the problem avoiding race conditions was non-trivial 
