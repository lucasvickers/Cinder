# build debug
mkdir build-debug
cd build-debug
# TODO enable once samples build
#cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Release -DCINDER_BUILD_TESTS=1 -DCINDER_BUILD_SAMPLE=1
cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Release -DCINDER_BUILD_TESTS=1
make -j4
cd ..
# build release
mkdir build-release
cd build-release
# TODO enable once samples build
#cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Debug -DCINDER_BUILD_TESTS=1 -DCINDER_BUILD_SAMPLE=1
cmake .. -DCINDER_VERBOSE:BOOL=1 -DCMAKE_BUILD_TYPE=Debug -DCINDER_BUILD_TESTS=1
make -j4
# test release
# TODO enable once tests work
#cmake check
# return to root dir
cd ..