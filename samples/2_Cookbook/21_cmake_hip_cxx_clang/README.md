### This sample tests CXX Language support with amdclang++
I. Build

```
mkdir -p build; cd build
rm -rf *;
CXX="$(hipconfig -l)"/amdclang++ cmake -DCMAKE_PREFIX_PATH=/opt/rocm ..
make
```

II. Test

```
$ ./square
info: running on device AMD Radeon Graphics
info: allocate host mem (  7.63 MB)
info: allocate device mem (  7.63 MB)
info: copy Host2Device
info: launch 'vector_square' kernel
info: copy Device2Host
info: check result
PASSED!
```
