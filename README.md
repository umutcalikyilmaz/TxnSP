# TxnSp
TxnSP is a software library used to create instances of the transaction scheduling problem and solving them. Thje library includes 4 solvers for the problem. It also contains modules for analyzing problem instances and evaluating the performance of the included solvers.

## Citation
TxnSP is developed as a part of a study on the transaction scheduling problem and its use in transaction-based optimizing multi-agent systems. The results of the study is compiled in a research paper, which is pending approval for publication. The information for citation will be provided after the publication.

## Installation
TxnSP is designed for Debian and currently it is not compatible with Windows or MacOS. For standard installtion, the following bash instructions must be executed in the project's root folder.

```bash
mkdir build
cd build
cmake ..
sudo make install
```

Mixed-integer programming solver module (MIPSolver) is not included in the standard installation. To install the TxnSP library with MIPSolver, first the SCIP Optimization Suite must be installed from https://www.scipopt.org/download.php?fname=scipoptsuite-8.0.2.tgz. Then the following bash insructions must be executed in the TxnSP project's root folder.


```bash
mkdir build
cd build
cmake -DENABLE_MIP=ON ..
sudo make install
```

## Usage
After installing TxnSP, it can be imported to a project by adding the following lines in the CMakeLists.txt file.

```
find_package(TxnSP REQUIRED)
target_link_libraries(my_project TxnSP::txnsp)
```

To use the problem instance creation and solver functionalities of TxnSP, the main header file should be included as below.

```
#include <TxnSP/TxnSP.h>
```

To be able to also use the Analyzer and Evaluator modules, the following code should be used.

```
#include <TxnSP/TxnSPTest.h>
```
