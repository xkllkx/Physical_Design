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
### Ex: Benchmark ami33
- H/W ratio 1 | white-space 10% (ami33_1_10)
  
<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_1_10/ami33_1_10_1099_1127.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_1_15/ami33_1_15_1099_1127.png" width="50%" height="50%">

- H/W ratio 2 | white-space 10% (ami33_2_10)
  
<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_2_10/ami33_2_10_784_1589.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_2_15/ami33_2_15_784_1589.png" width="50%" height="50%">

- H/W ratio 3 | white-space 10% (ami33_3_10)
  
<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_3_10/ami33_3_10_651_1904.png" width="50%" height="50%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA2_SA/final_solution/ami33_3_15/ami33_3_15_651_1904.png" width="50%" height="50%">

## PA3：Improving Wirelength based on Abacus (Detail-Placement)
### Goal: Improving Wirelength in 5 minute
### Ex: Benchmark superblue1
```bash
============ SCL ==========
NumRows : 1829
Numrow_array : 1829
----------
============ NODE ==========
NumNodes : 847441
NumTerminals : 82339
Numnode_array : 847441
NumMovableNodes : 765102
NumTerminal : 52627
NumTerminal_NI : 29712

----------
============ LEGAL ==========
Numinitial_movable_node : 765102

NumInRow : 817729 (817729)
----------
============ SHAPE ==========
NumNonRectangularNodes : 37
NumNonRect : 37
----------
========== NET ==========
NumNets : 822744
NumPins : 2861188
Numnet_array : 822744
----------
Run time : 4.23 s
=============== LG ===============
============ ROW_LIST INITIAL ==========
row_counter : 817729
----------
============ TOTAL HPWL_CAL ==========
initial_HPWL : 339097654
total_HPWL : 339097654 (100 %)
----------
=============== ITER 0 ===============
Iter time : 60.31 s
=============== ITER 1 ===============
Iter time : 62.66 s
=============== ITER 2 ===============
Iter time : 63.58 s
=============== ITER 3 ===============
Iter time : 64.1 s
=============== ITER 4 ===============
Iter time : 39.66 s
============ TOTAL HPWL_CAL ==========
total_HPWL : 316232559 (93.2571 %)
----------
===============================
Run time : 296.1 s
```
## PA4：Routing congestion estimation based on Flute (Routing)
### Goal: Using Flute to transfer nets to rectilinear Steiner minimal trees (RSMT) and estimate routing congestion with them.
### Ex: Benchmark adaptec1 (Routing blockage |  Horizontal Routing Congestion | Vertical Routing Congestion)

<img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA4_CE/output/adaptec1_block_dpx.png" width="33%" height="33%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA4_CE/output/adaptec1_H_dpx.png" width="33%" height="33%"><img src="https://github.com/xkllkx/NCKU_Physical_Design/blob/main/PA4_CE/output/adaptec1_V_dpx.png" width="33%" height="33%">

