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
 
<h5>Compiler Implementation Language and Reasoning</h5>
C++ because the team is already familiar with it. It also has a lot of options for optimization, and is very similar to the target language. 
 
<h5>Target Language</h5>
C
 
<h5>Language Description</h5>
An object oriented language based in C. An exploration of implementing the power objects on top of the light nature of a fairly low level language. All types are objects. Free functions are allowed. Strict naming conventions will be enforced such as best practices for variable and function names (There is one right way of naming): variables_are_snake_case, functionsAreCamelCase, and ClassesAreStudlyCase.
 
<h5>Planned Restrictions</h5>
There will be no way to reclaim allocated memory.

You cannot chain the dot operator (a.b.c). 
 
<h5>Abstract Syntax</h5>

Program 	::= ClassDec*

ClassDec 	::= ‘class’ ClassName [ ‘extends’ ClassName ] ClassBody

ClassBody 	::= { (MemberDec | MethodDec | ConstrDec )* }

MemberDec 	::= TypeName VarName

MethodDec 	::= RetTypeName MethodName ( FormalArgs ) Block

ConstrDec	::= cons ( FormalArgs ) Block

FormalArgs 	::= ( ) | ( TypeName VarName [ , TypeName VarName ]* )

ClassName 	::= Identifier

MethodName	::= Identifier

MemberName ::= Identifier

VarName 	::= Identifier

Identifier 	::= [A-Za-z_][A-Za-z0-9_]*	

TypeName	::= void | int | bool | ClassName

RetTypeName	::= void | TypeName

Block 		::= { Statement* }

Statement 	::= Statement’ ;

Statement’	::= VarDec | VarDecAssign | AssignStm 

| If | While | Method | RetStm | Print

VarDec 	::= TypeName VarName

VarDecAssign	::= TypeName VarName = Exp

AssignStm 	::= (VarName | Exp . VarName) = Exp

If 		::= if ( Exp ) Block else Block

	While 		::= while ( Exp ) Block

Method 	::= MethodName ActualArgs

RetStm 	::= return Exp

Print		::= print ( Exp )


Exp 		::= LogicalDown

ActualArgs	::= ( ) | ( Exp [ , Exp ]* )

LogicalDown	::= Logical | AdditiveDown

Logical		::= AdditiveDown ( == | != | && | || ) LogicalDown

AdditiveDown	::= Additive | MultiveDown

Additive	::= MultiveDown ( + | - ) AdditiveDown

MultiveDown	::= Multive | Primary

Multive		::= Primary ( * | / ) MultiveDown

Primary	::= Number | Variable | Unary | Call | Boolean | New | ( Exp )

Number	::= [0-9]+

Variable	::= this | this . MemberName | VarName . MemberName | VarName

Boolean	::= true | false

Unary		::= ( ! | - ) Primary

New		::= ClassName ActualArgs

Call		::= MethodName ActualArgs

<h5>Computation Abstraction Non-Trivial Feature:</h5>
Objects + methods with class-based inheritance.

<h5>Non-Trivial Feature #2:</h5>
Subtyping

<h5>Non-Trivial Feature #3:</h5>
Function Overloading

<h5>Work Planned for Custom Milestone:</h5>
Objects + methods with class-based inheritance.
