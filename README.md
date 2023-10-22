# Computer
Basic 16bit computer(for fun)
### 1. Goal of this project
The goal of this project is to make a stable 16bit computer architecture. Currently there is not much of things going on, but I will add more things(like interrupt, MMU) in future. 

### 2. Specification
```
Architecture    : 16bit
ISA             : RISC(+ CISC), Hybrid I guess..?
Registers Count : 8 General Purpose Reg(R0-R7), 2 ALU Input Reg(A,B), 2 Stack Reg(BP,SP), Flag Reg(Flag)
                   = Total 13 registers
```
### 3. How is project organized
* 'Computer.circ' : [Logisim-ITA](https://logisim.altervista.org/) file of entire circuit
* interpreter/    : Interpreter for architecture, convert assembly codes to hex instructions
* documents/      : Specific architecture description
* Spreadsheet (that contains lots of specific architecture descriptions..) : [Google Spreadsheet](https://docs.google.com/spreadsheets/d/1p-d54ITsBcRd8YfNupwiGrvMGoG832bEcXw2OzB0Sjg/edit?usp=sharing)
