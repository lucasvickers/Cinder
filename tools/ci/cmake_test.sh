# build debug
mkdir build-debug
cd build-debug
cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Release $@
make -j4
cd ..
# build release
mkdir build-release
cd build-release
cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Debug $@
make -j4
# test release
cmake check
# return to root dir
cd ..