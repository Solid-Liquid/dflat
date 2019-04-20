# 430-dflat

## Members
- Joshua Arco
- Avo Hagopian
- Ian Hinze
- Brian McClelland

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

## Currently missing features (as of Control Structures - Initial):
Code generation for methods has not been completed yet. This is because our language does not support free functions,
and we have not fully implemented codegen for classes yet. Classes will be part of abstraction of computation. 

Inheritance for classes is missing. This feature may or may not make it into abstraction of computation. We have not
yet fully solved how to handle classes in C code. 

Generics have not been implemented at the code generation level yet. We honestly need to do more research before
we are ready to complete this feature. 


<h1>Language Design Proposal:</h1>
<h3>Name: Db (D flat)</h3>
 
<h5>Compiler Implementation Language and Reasoning:</h5>
C++ because the team is already familiar with it. It also has a lot of options for optimization, and is very similar to the target language. 
 
<h5>Target Language:</h5>
C
 
<h5>Language Description:</h5>
An object oriented language based in C. An exploration of implementing the power objects on top of the light nature of a fairly low level language. All types are objects. Free functions are allowed. Strict naming conventions will be enforced such as best practices for variable and function names (There is one right way of naming): variables_are_snake_case, functionsAreCamelCase, and ClassesAreStudlyCase.
 
<h5>Planned Restrictions:</h5>
There will be no way to reclaim allocated memory.  
 
<h5>Abstract Syntax:</h5>

ValName is a legal non-type identifier

TypeName is a legal type name

String and Number are literal values

There will be some built-in primitive types Int, Boolean, Void


Program ::= FreeStatement*

VarName ::= ValName

MethodName ::= ValName

FunctionName ::= ValName

BinaryOp ::= ‘+’ | ‘-’ | ‘*’ | ‘/’ | ‘&&’ | ‘||’

UnaryOp ::= ‘!’

ArgList ::= ‘(‘ Exp [‘,’ Exp]* ‘)’

Exp ::= ‘(‘ Exp ‘)’

    | Exp BinaryOp Exp
    
    | UnaryOp Exp
    
    | Valname
    
    | String
    
    | Number
    
    | ‘this’
    
    | Exp ‘.’ MethodName ArgList
    
    | ‘new’ TypeName ArgList
    
    | FunctionName ArgList
    
    | ‘println’ ‘(‘ Exp ‘)’
    
    | Prints something to the terminal

FreeStatement ::= (VarDec | ClassDec | TypeDec | FunctionDec) ‘;’

ClassStatement ::= (DataDec | MethodDec) ‘;’

BlockStatement ::= (VarDec | TypeDec | Assignment | If) ‘;’

Assignment ::= (VarName | Exp ‘.’ VarName) ‘=’ Exp

If ::= ‘if’ ‘(‘ Exp ‘)’ ‘then’ Block ‘else’ Block

Block ::= ‘{‘ BlockStatement* ‘}’

VarDec ::= TypeName VarName [ ‘,’ VarName ]*

ClassDec ::= ‘class’ TypeName [GenericArgList] [ ClassInherit] ClassBody

ClassInherit ::= ‘extends’ TypeName

ClassBody ::= ‘{‘ ClassStatement* ‘}’

TypeDec ::= ‘typedef’ TypeName ‘=’ TypeName [GenericArgList]

FunctionDec ::= TypeName FunctionName [GenericArgList] DecArgList Block

DataDec ::= [MemberMod]* TypeName VarName

MethodDec ::= [MemberMod]* TypeName MethodName DecArgList Block

DecArgList ::= ‘(‘ TypeName VarName [ ‘,’ TypeName VarName ]* ‘)’

MemberMod ::= ‘static’

GenericArgList ::= ‘<’ TypeName [ ‘,’ TypeName ]* ‘>’


<h5>Computation Abstraction Non-Trivial Feature:</h5>
Objects + methods with class-based inheritance.

<h5>Non-Trivial Feature #2:</h5>
Generic Programming

<h5>Non-Trivial Feature #3:</h5>
Function Overloading

<h5>Work Planned for Custom Milestone:</h5>
Objects + methods with class-based inheritance.
