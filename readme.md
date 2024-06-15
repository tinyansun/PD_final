# Project Description

This project contains the following files and folders:

- `bin`: This folder contains the executable file after make compile.
- `doc`: This folder contains the project documentation, which is ICCAD contest 2024 Problem D.
- `result`: This folder contains our results, including the .rpt file required by the Problem D, the grid.csv file needed for plotting, and the routing result image route.png.
- `src`: This folder contains the source code files.
- `testcases`: This folder contains the test data for the project.
- `makefile`: This file is used for compiling the project.
- `readme.md`: This file introduces the significance of each file in the code.

### How to compile
just type "make" in main directory, then executable file will be compiled into bin directory.  

example : make

### How to run routing program
type "./bin/CGR <XX(tracks/um)> <caseX_def directory> <caseX_cfg.json file> <caseX.json file>", then output .rpt file will be generated in main directory.  

exameple : ./bin/CGR 20 testcases/case4/case4_def testcases/case4/case4_cfg.json testcases/case4/case4.json

### How to run plot program
type "python3 ./bin/draw_route.py --grid <grid.csv file> --lines <.rpt file> --output <output .png file>", then output .png file will be generated in main directory.  

exameple : python3 ./bin/draw_route.py --grid ./result/grid4.csv --lines ./result/case4_net.rpt --output route4.png