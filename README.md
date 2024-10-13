# VM Compilers Course

## How to build
#### First, clone the repository:
```
git clone https://github.com/OAMichael/VM-Compilers-Course.git --recursive
```

#### Second, run cmake:
```
cmake -S . -B build
cmake --build build -j
```

## Samples
#### One can find samples of VM IR, their generation code and visualization graphs in folder `SampleIR`. Each sample contains its original implementation in C++, self-written IR file, IRGen file which generates this IR and plotted control flow graph (CFG) of the sample

#### After building the project, one can run a sample:
```
cd build/SampleIR/<Sample>
./<Sample>
```

#### The sample with output generated IR to stdout and save .dot file which represents CFG of the sample. After these steps, one can plot the CFG using `dot`. You need to install `graphviz`:
```
sudo apt install graphviz
```

#### Example of running a sample:
```
cd build/SampleIR/FactLoop
./FactLoop
dot -Tpng < FactLoop.dot > FactLoop.png
```