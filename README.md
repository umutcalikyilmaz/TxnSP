# TxnSP
TxnSP is a software library used to create instances of the transaction scheduling problem and solving them. For this purpose, the library includes four solvers: `ESSolver` (the exhaustive search solver), `DPSolver` (the dynamic programming solver), `MIPSolver` (the mixed-integer programming solver), and `SASolver` (the simulated annealing solver). It also contains modules for analyzing the solution spaces of problem instances and evaluating the performance of the included solvers.

## Citation
TxnSP is developed as a part of a study on the transaction scheduling problem and its use in transaction-based optimizing multi-agent systems. The study is presented in the paper given below.

[OptiMA: A Transaction-Based Framework with Throughput Optimization for Very Complex Multi-Agent Systems](https://arxiv.org/pdf/2511.03761)

### Version used in Paper

The experiments reported in the paper were conducted using **TxnSP v1.0**.
The exact version of the source code used for those experiments is archived here:

**[TxnSP v1.0](https://github.com/umutcalikyilmaz/TxnSP/tree/v1.0)**

## Installation
TxnSP is designed for Debian-based systems and currently it is not compatible with Windows or MacOS. For standard installation, the following bash instructions should be executed in the project's root folder.

```bash
mkdir build
cd build
cmake ..
sudo make install
```

Mixed-integer programming solver module (MIPSolver) is not included in the standard installation. To install the TxnSP library with MIPSolver, first the SCIP Optimization Suite must be installed from the **[link](https://www.scipopt.org/download.php?fname=scipoptsuite-8.0.2.tgz)**. Then the following bash insructions must be executed in the TxnSP project's root folder.


```bash
mkdir build
cd build
cmake -DENABLE_MIP=ON ..
sudo make install
```

## Usage

### Importing the Library
After installing TxnSP, it can be imported to a project by adding the following lines in the CMakeLists.txt file.

```cmake
find_package(TxnSP REQUIRED)
target_link_libraries(my_project TxnSP::txnsp)
```

To use the problem instance creation and solver functionalities of TxnSP, the main header file should be included as below.

```c++
#include <TxnSP/TxnSP.h>
```

To also use the Analyzer and Evaluator modules, the following code should be used.

```c++
#include <TxnSP/TxnSPTest.h>
```

### Problem Creation
In TxnSP, a problem instance can be created in two ways. The first one is random creation, which uses either normal distribution or uniform distribution to randomly generate the problem parameters. In both cases, the conflict matrix is generated using Bernoulli distribution where the conflict parity is entered by the user. The below code shows random problem creation with normal and uniform distributions.

```c++
// Problem creation using normal distribution
TxnSP::Problem problem(
    jobCount,                                  // int: Number of jobs
    machineCount,                              // int: Number of machines
    TxnSP::ProbabilityDistribution::Normal,    // enum: Distribution type
    lengthMean,                                // double: Mean of job lengths
    lengthStd,                                 // double: Standard deviation of job lengths
    conflictParity                             // double: Parity of conflicting pair of jobs
);
```

```c++
// Problem creation using uniform distribution
TxnSP::Problem problem(
    jobCount,                                  // int: Number of jobs
    machineCount,                              // int: Number of machines
    TxnSP::ProbabilityDistribution::Uniform,   // enum: Distribution type
    lowerLimit,                                // double: Lower limit of job lengths
    upperLimit,                                // double: Upper limit of job lengths
    conflictParity                             // double: Parity of conflicting pair of jobs
);
```

TxnSP also allows creating custom problems by providing the array of lengths and the matrix of conflicts as shown below.

```c++
// Custom problem creation
TxnSP::Problem problem(
    jobCount,          // int: Number of jobs
    machineCount,      // int: Number of machines
    lengths,           // std::vector<double>: Array of job lengths
    conflicts          // std::vector<std::vector<uint8_t>>: Conflict matrix
);
```

### Problem Solution
After a problem instance is created using one of the explained methods, it can be solved using one of the four solvers provided by TxnSP library. Usage of each solver is given below.

#### Dynamic Programming:
```c++
// Create the dynamic programming solver
TxnSP::DPSolver dps;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;                                    // Assign a pointer to the problem instance
input.DP_SolutionType = TxnSP::SolutionType::Exact;      // Use exact solution method, other option is TxnnSP::Approximate

// Solve the problem and get the output
TxnSP::SolverOutput output = dps.solve(input);    // Returns a struct containing solution details
```

#### Exhaustive Search:
```c++
// Create the exhaustive search solver
TxnSP::ESSolver ess;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;           // Assign a pointer to the problem instance

// Solve the problem and get the output
TxnSP::SolverOutput output = ess.solve(input);  // Returns a struct containing solution details
```


#### Mixed-Integer Programming:
```c++
// Create the mixed-integer programming solver
TxnSP::MIPSolver mips;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;           // Assign a pointer to the problem instance

// Solve the problem and get the output
TxnSP::SolverOutput output = mips.solve(input);  // Returns a struct containing solution details
```

#### Simulated Annealing
```c++
// Create the simulated annealing solver
TxnSP::SASolver sas;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;                                            // Assign a pointer to the problem instance
input.SA_MaxTemperature = maxTemperature;                        // double: Set maximum temperature
input.SA_DecrementType = TxnSP::TemperatureEvolution::Linear;    // Use linear temperature decrement, other options are TxnSP::Logarithmic and TxnSP::Slow
input.SA_DecrementParameter = decrementParameter;                // double: Cooling rate

// Solve the problem and get the output
TxnSP::SolverOutput output = sas.solve(input);   // Returns a struct containing solution details
```
