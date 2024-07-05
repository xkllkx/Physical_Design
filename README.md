.gt-post-content img {
    display: block;
    max-width: 30%;
    border-radius: 2px;
    margin: 18px auto;
}



# Physical-Design
## PA1：Fiduccia–Mattheyses algorithm (Partition)
### Goal: Minimum two groups of cut-size

| Benchmark | Pin num | Initial Cut_size | Final Cut_size | RunTime(s) |
| --- |--- |--- |--- |--- |
| adapted1	  | 210904	| 7963	 | 7960	| 4.02 |
| adapted2	  | 254457	| 10068 | 10067	| 5.1  |
| adapted3	  | 450927	| 25809 | 25803	| 9.61 |
| superbule1	| 765102	| 52750 | 52749	| 28.62 |
| superbule1	| 921273	| 60973 | 60893	| 23.37 |

<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA1_FM/FM.png" width="55%" height="55%">

## PA2：Simulated Annealing Algorithm (Placement)
### Goal: Perturbation Randomly to place objects.

<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_1_10/ami33_1_10_1099_1127.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_1_15/ami33_1_15_1099_1127.png" width="50%" height="50%">

<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_2_10/ami33_2_10_784_1589.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_2_15/ami33_2_15_784_1589.png" width="50%" height="50%">

<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_3_10/ami33_3_10_651_1904.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_3_15/ami33_3_15_651_1904.png" width="50%" height="50%">

### PA3：Improving Wirelength based on Abacus (Detail-Placement)



### PA4：Routing congestion detection based on Flute (Routing)
