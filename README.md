# Computer
Basic 16bit computer(for fun)
![alt text](https://github.com/Juha3141/Computer/blob/master/documents/circuit.png?raw=true)
### 1. Goal of this project
The project is just making a computer architecture, nothing too abstract or something.. My initial goal was to design this architecture and actually making it (irl), but I found that pretty much impossible, since the project has gone to the point that I cannot make this in a breadboard. However, I am still planning to make this via PCB design and some breadboards(like, breadboard and PCB hybrid computer ..) 

### 2. Specification
```
Architecture    : 16bit
ISA             : CISC
Registers Count : 8 General Purpose Reg(R0-R7), 2 ALU Input Reg(A,B)
                  2 Stack Reg(BP,SP), Flag Reg(Flag)
                  = Total 13 registers
List of "extra registers":
 1. Register A     5. Div. Out
 2. Register B     6. Port Address Register
 3. BP             7. Port Read Write Register
 4. SP             8. Port Enable Register
```

### 3. How is project organized
* 'Computer.circ' : [Logisim-ITA](https://logisim.altervista.org/) file of entire circuit
* interpreter/    : Interpreter for architecture, convert assembly codes to hex instructions
* documents/      : Specific architecture description (not implemented yet)
* Spreadsheet (that contains lots of specific architecture descriptions..) : [Google Spreadsheet](https://docs.google.com/spreadsheets/d/1p-d54ITsBcRd8YfNupwiGrvMGoG832bEcXw2OzB0Sjg/edit?usp=sharing)

### 4. Assembly Interpreter(Assembler)
  More about assembly interpreter in this document : 

### 5. Future plan?
Currently there is not much of things going on, but I will add more things(like interrupt, MMU) in future. I'm planning to research how to improve original systems of current PC/etc architectures, and implement those research results to this project. (But no one knows how this project will end up with..)

