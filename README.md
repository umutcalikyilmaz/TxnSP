# TxnSP
TxnSP is a software library for creating and solving instances of the transaction scheduling problem. For this purpose, the library includes four solvers: `ESSolver` (the exhaustive search solver), `DPSolver` (the dynamic programming solver), `MIPSolver` (the mixed-integer programming solver), and `SASolver` (the simulated annealing solver). It also contains modules for analyzing the solution spaces of problem instances and evaluating the performance of the included solvers.

## Citation
TxnSP was developed as part of a study on the transaction scheduling problem and its use in transaction-based optimizing multi-agent systems. The study is presented in the paper below.

[OptiMA: A Transaction-Based Framework with Throughput Optimization for Very Complex Multi-Agent Systems](https://arxiv.org/pdf/2511.03761)

### Version Used in the Paper

The experiments reported in the paper were conducted using **TxnSP v1.0**. The exact version of the source code used for those experiments is archived here:

**[TxnSP v1.0](https://github.com/umutcalikyilmaz/TxnSP/tree/v1.0)**

## Installation
TxnSP requires a compiler with `C++20` support. It is designed for Debian-based systems and is currently not compatible with Windows or macOS. For a standard installation, execute the following commands in the project's root directory:

```bash
mkdir build
cd build
cmake ..
sudo make install
```

The `MIPSolver` module is not included in the standard installation. To install the TxnSP library with `MIPSolver`, the **[SCIP Optimization Suite](https://www.scipopt.org/download.php?fname=scipoptsuite-8.0.2.tgz)** must first be installed. Then, execute the following commands in the TxnSP project's root directory:


```bash
mkdir build
cd build
cmake -DENABLE_MIP=ON ..
sudo make install
```

## Usage

### Importing the Library
After installing TxnSP, it can be imported into a project by adding the following lines to the `CMakeLists.txt` file:

```cmake
find_package(TxnSP REQUIRED)
target_link_libraries(my_project TxnSP::txnsp)
```

To use the problem instance creation and solver functionalities of TxnSP, include the main header file as follows.

```c++
#include <TxnSP/TxnSP.h>
```

To also use the Analyzer and Evaluator modules, include the following header:

```c++
#include <TxnSP/TxnSPTest.h>
```

### Problem Generation
In TxnSP, a problem instance can be created in two ways. The first is random problem generation, which uses either a normal distribution or a uniform distribution to randomly generate the problem parameters. In both cases, the conflict matrix is generated using a Bernoulli distribution, where the conflict probability is specified by the user. The following examples show random problem creation using normal and uniform distributions.

```c++
// Problem creation using normal distribution
TxnSP::Problem problem(
    jobCount,                                  // int: Number of jobs
    machineCount,                              // int: Number of machines
    TxnSP::ProbabilityDistribution::Normal,    // enum: Distribution type
    lengthMean,                                // double: Mean of job lengths
    lengthStd,                                 // double: Standard deviation of job lengths
    conflictParity                             // double: Probability that any pair of jobs conflicts
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
    conflictParity                             // double: Probability that any pair of jobs conflicts
);
```

TxnSP also allows custom problem instances to be created by providing a vector of job lengths and a conflict matrix, as shown below.

```c++
// Custom problem creation
TxnSP::Problem problem(
    jobCount,          // int: Number of jobs
    machineCount,      // int: Number of machines
    lengths,           // std::vector<double>: Job lengths
    conflicts          // std::vector<std::vector<uint8_t>>: Conflict matrix
);
```

### Problem Solution
After a problem instance has been created using one of the methods described above, it can be solved using one of the four solvers provided by the TxnSP library. Examples of using each solver are shown below.

#### Dynamic Programming
```c++
// Create the dynamic programming solver
TxnSP::DPSolver dps;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;                                    // Assign a pointer to the problem instance
input.DP_SolutionType = TxnSP::SolutionType::Exact;      // Use the exact solution method, other option is TxnSP::SolutionType::Approximate

// Solve the problem and get the output
TxnSP::SolverOutput output = dps.solve(input);    // Returns a struct containing solution details
```

#### Exhaustive Search
```c++
// Create the exhaustive search solver
TxnSP::ESSolver ess;

// Prepare the input structure
TxnSP::SolverInput input;
input.prb = &problem;           // Assign a pointer to the problem instance

// Solve the problem and get the output
TxnSP::SolverOutput output = ess.solve(input);  // Returns a struct containing solution details
```


#### Mixed-Integer Programming
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
