# 430-dflat

## Members
- Joshua Arco
- Avo Hagopian
- Ian Hinze
- Brian McClelland

## Documentation
https://docs.google.com/document/d/1wfgvMtN3cCAjJY_HuqsTks-IuwwELZK9XyW2E1kNmAs/edit?usp=sharing

## Dependencies
- CMake
- C++17 or later compiler

## Build/Run
In project directory:
```
mkdir build
cd build
cmake ..
make
./dflat <input filename>
./tests
```

## Generage Coverage Report
Run ```gen-coverage``` from the project root. Requires gcov+lcov. Produces a report at coverage/report/index.html.

## Currently missing features
None (we believe).


<h1>Language Design Proposal</h1>
<h3>Name: Db (D flat)</h3>
Most of the information that belongs here has been moved to the Documentation (above)
 
<h5>Compiler Implementation Language and Reasoning</h5>
C++ because the team is already familiar with it. It also has a lot of options for optimization, and is very similar to the target language. 
 
<h5>Target Language</h5>
C 

<h5>Computation Abstraction Non-Trivial Feature:</h5>
Objects + methods with class-based inheritance.

<h5>Non-Trivial Feature #2:</h5>
Subtyping

<h5>Non-Trivial Feature #3:</h5>
Function Overloading

<h5>Work Planned for Custom Milestone:</h5>
Objects + methods with class-based inheritance.
