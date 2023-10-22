#ifndef _INSTRUCTION_HPP_
#define _INSTRUCTION_HPP_

#include <iostream>
#include <vector>
#include <process_asm.hpp>
#include <string>
#include <cstring>

#include <types.hpp>

typedef unsigned short default_t;
typedef char bit_one;

struct Argument {
    int argument_type;
    /* 1 : register
     * 2 : address-register
     * 3 : label
     * 4 : special register
     * special types : 
     * 5 : Conditional Jump
     * 6 : ALU Operation
    */
    int register_number;
    char *ext_reg;
    char *label_name;
    int conditional_jump;
    /* 0 : JMP
     * 1 : JZ
     * 2 : JU
     * 3 : JS
     * 4 : JC
     * 5 : JO
     */
    int alu_operation;
    /* 0 : ADD
     * 1 : SUB
     * 2 : MUL
     * 3 : DIV
     * 4 : AND
     * 5 : OR
     * 6 : XOR
     * 7 : NOT
    */
};

struct AssemblyCode {
    char asmcode[32];
    int arg_count;

    // subcodes
    /*
     ****** arg_count : 1 ******
     * 0 : R  <- R
     * 1 : R  <- D
     * 2 : R  <- A
     * 3 : A  <- R
     * 4 : R  <- AR
     * 5 : AR <- R
     * 6 : AR <- D
     ****** arg_count : 2 ******
     * 0 : R
     * 1 : D
     * 2 : A
     * 3 : AR
     ***************************
    */
    unsigned short type_instructions[7]; // corresponding instruction per type
};

struct DataArgument {
    int argument_type;
    default_t data;
};

struct AssemblyInstruction {
    /* 1 : control
     * 2 : only instruction
     * 3 : instruction, argument
     * 4 : instruction, argument1 , argument2
    */
    int instruction_type;

    // for instruction
    char *instruction;
    int argument_count;
    Argument *arguments;

    int data_count;
    DataArgument *data_arguments;
};

class VariableMarker {
    public:
        bool store_mark(char *marker_name , unsigned int memory_addr);
        unsigned int load_mark(const char *marker_name);
        
    private:
        std::vector<std::pair<char*,unsigned int>>markers;
};

class InstructionController {
    public:
        InstructionController(std::vector<std::pair<AssemblyInstruction,int>>*full_contents) : asm_source(full_contents) {
            current_position = 0;
            marker_store = new VariableMarker();
            AssemblyCode codes[] = {
                {"mov"     , 2 , {0x01 , 0x02 , 0x03 , 0x04 , 0x05 , 0x06 , 0x07}} , 
                {"ldr"     , 2 , {0x08 , 0x09 , 0x0A , 0x0B , 0x00 , 0x00 , 0x00}} , 
                {"aluout"  , 1 , {0x0C , 0x0D , 0x00 , 0x00 , 0x00 , 0x00 , 0x00}} , 
                {"alutest" , 0 , {0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00}} , 
                {"jmp"     , 1 , {0x10 , 0x00 , 0x0F , 0x11 , 0x00 , 0x00 , 0x00}} , 
            };
            for(auto i : codes) {
                asm_codes.push_back(i);
            }
        }

        bool process_line(int line); // error or not

        default_t convert_instruction(AssemblyInstruction instruction);
        void save_loop(AssemblyInstruction instruction);
        
        // static
        static int preprocessing(char *oneline_assembly);
        static int process_instruction(AssemblyInstruction *instruction , const char *oneline_assembly , int instruction_type);
        static bool process_data_argument(DataArgument &arg_info , const char *asm_arg);
        static bool process_argument(Argument &arg_info , const char *asm_arg);
        static int get_argument_count(int instruction_type) {
            if(instruction_type > 4||instruction_type < 0) {
                return 0;
            }
            const int ac[] = {0,0,2,1,2};
            return ac[instruction_type];
        }
        /// @brief Get real instruction type
        /// @param instruction 
        /// @return 
        static int get_real_instruction_type(const AssemblyInstruction instruction) {
            if(instruction.argument_count == 0) {
                return 0; // type zero : No arguments
            }
            // one register, one argument
            if(instruction.argument_count == 2
            && (instruction.arguments[1].argument_type == 3||instruction.arguments[1].argument_type == 4)) {
                switch(instruction.arguments[1].argument_type) {
                    case ARGUMENT_TYPE_REGISTER:
                        return 1; // type one : only register
                    case ARGUMENT_TYPE_EXT_REG:
                        return 6; // type six : only ext. register
                };
                // two register, both 'Register'
                if(instruction.arguments[0].argument_type == 3 && instruction.arguments[1].argument_type == 3) {
                    return 2;
                }
            }
        }
    private:
        AssemblyCode *get_assembly_code_info(const char *instruction) {
            int sz = asm_codes.size();
            for(int i = 0; i < sz; i++) {
                if(strcmp(asm_codes[i].asmcode , instruction) == 0) {
                    return &asm_codes[i];
                }
            }
            return 0x00;
        }

        std::vector<std::pair<AssemblyInstruction,int>>*asm_source;
        unsigned int current_position = 0;
        VariableMarker *marker_store;
        std::vector<AssemblyCode>asm_codes;
};

#endif