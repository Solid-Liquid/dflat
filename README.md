# 430-dflat

## Dependencies
- CMake
- C++17 or later compiler

<h1>Language Design Proposal:</h1>
Db
 
<h5>Student Name(s):</h5>
Joshua Arco, Brian McClelland, Ian Hinze, Avo Hagopian

<h5>Language Name:</h5>
Db (D flat)
 
<h5>Compiler Implementation Language and Reasoning:</h5>
C++ because the team is already familiar with it. It also has a lot of options for optimization, and is very similar to the target language. 
 
<h5>Target Language:</h5>
C
 
<h5>Language Description:</h5>
An object oriented language based in C. An exploration of implementing the power objects on top of the light nature of a fairly low level language. All types are objects. Free functions are allowed. The are no line ending characters; instead, there are line continuing characters (like backslash). Strict naming conventions will be enforced such as best practices for variable and function names (There is one right way of naming): variables_are_snake_case, functionsAreCamelCase, and ClassesAreStudlyCase.
 
<h5>Planned Restrictions:</h5>
There will be no way to reclaim allocated memory.  
 
<h5>Abstract Syntax:</h5>

ValName is a legal non-type identifier

TypeName is a legal type name

String and Number are literal values

There will be some built-in primitive types Int, Boolean, Void

Statements are terminated by end of line, but can be extended across lines with a \-escaped end of line.


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

FreeStatement ::= (VarDec | ClassDec | TypeDec | FunctionDec) ‘\n’

ClassStatement ::= (DataDec | MethodDec) ‘\n’

BlockStatement ::= (VarDec | TypeDec | Assignment | If) ‘\n’

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
Function Overloading
