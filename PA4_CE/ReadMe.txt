============ HW4 Congestion Map ==========

A. File hierarchy：
    CE (root)
        |_ benchmarks
            |_ adaptec1
                |_ .nets
                |_ .nodes
                |_ .scl
                |_ .shapes
                |_ .aux
                |_ .legal
                |_ .pl
                |_ .route
                |_ .log (執行過程)

            |_ adaptec2
                |_ ...
            |_ adapte3
                |_ ...
        
        |_ output (優化合法結果)
            |_ adaptec1
                |_ block.dpx
                |_ H.dpx
                |_ V.dpx
                |_ H.m
                |_ V.m
                |_ H.png
                |_ V.png
                |_ .txt

            |_ adaptec2
                |_ ...
            |_ adapte3
                |_ ...

        程式部分：
        |_ include
            |_ CE.h
        |_ src
            |_ main.cpp
            |_ CE.cpp
        
        Other：
        |_ CE
        |_ Makefile
        |_ display.x
        |_ POST9.dat
        |_ POWV9.dat

B. 執行指令總覽：
    make
    make clean

    執行：
        ./CE –bench /input/adaptec1.aux –output /output/adaptec1.txt
        ./CE –bench /input/adaptec2.aux –output /output/adaptec2.txt
        ./CE –bench /input/adaptec3.aux –output /output/adaptec3.txt

    快速執行：
        make run TESTCASE=adaptec1
        make run TESTCASE=adaptec2
        make run TESTCASE=adaptec3
