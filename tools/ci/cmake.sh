mkdir build
cd build
cmake .. -DCINDER_VERBOSE:BOOL=1 $@
make -j4