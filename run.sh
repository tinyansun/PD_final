#測試測資4 : ./test.sh 4
#測試測資5 : ./test.sh 5
#default測資4和5 : ./test.sh
make

if [ -n "$1" ]; then
    if [ $1 -eq "4" ]; then
        ./bin/CGR 20 testcases/case4/case4_def testcases/case4/case4.cfg.json testcases/case4/case4.connection_matrix.json
    elif [ $1 -eq "5" ]; then
        ./bin/CGR 20 testcases/case5/case5_def testcases/case5/case5.cfg.json testcases/case5/case5.connection_matrix.json
    fi
else
    ./bin/CGR 20 testcases/case4/case4_def testcases/case4/case4.cfg.json testcases/case4/case4.connection_matrix.json
    ./bin/CGR 20 testcases/case5/case5_def testcases/case5/case5.cfg.json testcases/case5/case5.connection_matrix.json
fi