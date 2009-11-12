#!/bin/bash
echo "Compiling OpenIRT..."
# g++ -O3 -funroll-loops openirt.cc -o openirt
# g++ -O3 -funroll-loops -I /usr/local/include/boost-1_39/ openirt.cpp -o build/openirt /lib/libboost_thread-xgcc40-mt.a
#g++ -O3 -funroll-loops -I /usr/local/include/boost-1_39/  openirt.cpp -o build/openirt
g++ -DSCYTHE_DEBUG=0 -O3 -funroll-loops -I /usr/local/include/boost-1_39/ openirt.cpp -o build/openirt
# source /opt/intel/Compiler/11.0/059/bin/iccvars.sh ia32
#icpc -DSCYTHE_DEBUG=0 -O3 -fast -I /usr/local/include/boost-1_39/ openirt.cpp -o ../Stata/openirt.exe
#[ $? -ne 0 ] && exit 1
