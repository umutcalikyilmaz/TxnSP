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

## Library Contents

### Problem Creation and Solution
The main purpose of TxnSP to provide an environment for creating transaction scheduling problem instances and solving them. A problem instance can be created in two ways. The first one is random creation, which uses either normal distribution or uniform distribution to randomly generate the problem parameters. In both cases, the conflict matrix is generated using Bernoulli distribution where the conflict parity is entered by the user. The below code shows random problem creation with normal and uniform distributions.

```
// Problem creation using normal distribution
TxnSP::Problem problem(
    jobNumber,                     // int: Number of jobs
    machineNumber,                 // int: Number of machines
    TxnSP::NormalDistribution,     // Distribution type
    lengthMean,                    // double: Mean of job lengths
    lengthStd,                     // double: Standard deviation of job lengths
    conflictParity                 // double: Parity of conflicting pair of jobs
);
```

```
// Problem creation using uniform distribution
TxnSP::Problem problem(
    jobNumber,                     // int: Number of jobs
    machineNumber,                 // int: Number of machines
    TxnSP::UniformDistribution,    // Distribution type
    lowerLimit,                    // double: Lower limit of job lengths
    upperLimit,                    // double: Upper limit of job lengths
    conflictParity                 // double: Parity of conflicting pair of jobs
);
```

TxnSP also allows creating custom problems by providing the array of lengths and the matrix of conflicts as shown below.

```
// Custom problem creation
TxnSP::Problem problem(
    jobNumber,         // int: Number of jobs
    machineNumber,     // int: Number of machines
    lengths,           // double*: Array of job lengths
    conflicts          // bool**: Conflict matrix
);
```

After a problem instance is created using one of the explained methids, it can be solved using one of the 4 solvers provided by TxnSP library.

```
// Custom problem creation
TxnSP::Problem problem(
    jobNumber,         // int: Number of jobs
    machineNumber,     // int: Number of machines
    lengths,           // double*: Array of job lengths
    conflicts          // bool**: Conflict matrix
);
```

### Analysis and Evaluation
