# Assembler Syntax
### 1. Forms of assembly syntax
The basic syntax of assembly is constructed with *Instruction* and *Argument*.

* *Instruction* is the main opcode, and *Argument* is the Operand of the instruction.
* *Argument* can be any kind of data. Currently, an argument can be Register, Hex Data or Memory Address.

*(In this assembler, the types of argument is determined by the formatting of argument.)*

There are 4 forms of assembly syntax : 
* ***Type 1 syntax*** is the declaration of a label.
* ***Type 2 syntax*** is an instruction with two arguments.
* ***Type 3 syntax*** is an instruction with one argument.
* ***Type 4 syntax*** is an instruction with one *"extra register"* and one argument.

Each syntax are implemented by this syntax : 
```
Type 1. "label*": 
Type 2. [instruction] [argument 1] , [argument 2] 
Type 3. [instruction] [argument 1]
Type 4. [instruction](argument 1) [argument 2]

* label : Indicates memory address in current context(= PC[Program Counter]).
If used, it acts as a numerical data with value of memory address of the label.
```

## 2. Instruction list
Lists of instruction are shown specifically in the source code : [instruction.hpp: Line 123](https://github.com/Juha3141/Computer/blob/master/assembler/Headers/instruction.hpp#L123)

## 3. Types of Argument
