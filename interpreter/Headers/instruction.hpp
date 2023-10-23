#ifndef _INSTRUCTION_HPP_
#define _INSTRUCTION_HPP_

#include <iostream>
#include <vector>
#include <process_asm.hpp>
#include <string>
#include <cstring>

#include <types.hpp>

#define INSTRUCTION_MAININSTRUCTION_SIZE 6
#define INSTRUCTION_ARGUMENT_SIZE        3

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
     * 7 : label-register
    */
    int register_number;
    int ext_reg_number;
    // char *label_name;
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

    int arg_index;
};

struct DataArgument {
    int argument_type;
    default_t data;
    char *label;

    int arg_index;
};

struct AssemblyCode {
    char asmcode[32];
    int arg_count;

    // subcodes
    /*
     ****** arg_count : 0 ******
     * 0  : Nothing
     * 1  : D
     * 2  : A
     ****** arg_count : 1 ******
     * 3  : R
     * 4  : R  <- D
     * 5  : R  <- A
     * 
     * 6  : AR
     * 7  : AR <- D
     * 
     * 8  : A  <- R
     ****** arg_count : 2 ******
     * 9  : R <- R
     * 10 : R <- AR
     * 
     * 11 : AR <- R
     ***************************
    */
    unsigned short type_instructions[12]; // corresponding instruction per type
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
                {"mov"     , 2 , {0xFF , 0xFF , 0xFF , 0xFF , 0x02 , 0x03 , 0xFF , 0x07 , 0x04 , 0x01 , 0x05 , 0x06}} , 
                {"ldr"     , 2 , {0xFF , 0x09 , 0x0A , 0x08 , 0xFF , 0xFF , 0x0B , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}} , 
                {"aluout"  , 1 , {0xFF , 0xFF , 0x0D , 0x0C , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}} , 
                {"alutest" , 0 , {0x0E , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}} , 
                {"jmp"     , 1 , {0xFF , 0xFF , 0x0F , 0x10 , 0xFF , 0xFF , 0x11 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}} , 
                {"stpc"    , 1 , {0xFF , 0xFF , 0xFF , 0x1B , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}} , 
                {"hlt"     , 0 , {0x1F , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF}}
            };
            for(auto i : codes) {
                asm_codes.push_back(i);
            }
        }

        bool process_line(int line); // error or not

        default_t convert_instruction(AssemblyInstruction instruction);
        
        // static
        static int preprocessing(char *oneline_assembly);
        int process_instruction(AssemblyInstruction *instruction , const char *oneline_assembly , int instruction_type);
        bool process_data_argument(DataArgument &arg_info , const char *asm_arg);
        bool process_argument(Argument &arg_info , const char *asm_arg);
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
            /* <real instruction type from spreadsheet>
             * 0 : No arguments
             * 1 : One argument, 1 : RC
             * 2 : Two argument  1 : RC, 2 : RC
             * 3 : Two argument, 1 : RC, 2 : ALU Oper.
             * 4 : Two argument, 1 : RC, 2 : Conditional Jump Oper.
             * 5 : Two Argument, 1 : RC, 2 : ERC
             * 6 : One Argument, 1 : ERC
            */
            if(instruction.argument_count == 0) {
                if(instruction.data_count == 0) { // 0 : nothing
                    return 0;
                }
                else if(instruction.data_count == 1) {
                    if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_DATA) return 1;
                    if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_ADDRESS) return 2;
                }
                return -1;
            }
            if(instruction.argument_count == 1) {
                if(instruction.arguments[0].argument_type == ARGUMENT_TYPE_REGISTER) return 1;
                if(instruction.arguments[0].argument_type ==  ARGUMENT_TYPE_ADDR_REG) return 1;
                if(instruction.arguments[0].argument_type == ARGUMENT_TYPE_EXT_REG) return 6;
                if(instruction.arguments[0].argument_type == ARGUMENT_TYPE_COND_JUMP) return 7;
                if(instruction.arguments[0].argument_type == ARGUMENT_TYPE_ALU_OPER) return 8;
            }
            if(instruction.argument_count == 2) {
                int arg_t1 = instruction.arguments[0].argument_type;
                int arg_t2 = instruction.arguments[1].argument_type;
                if(arg_t1 == ARGUMENT_TYPE_REGISTER && arg_t2 == ARGUMENT_TYPE_REGISTER)    return 2;
                if((arg_t1 == ARGUMENT_TYPE_REGISTER && arg_t2 == ARGUMENT_TYPE_ALU_OPER)
                || (arg_t1 == ARGUMENT_TYPE_ALU_OPER && arg_t2 == ARGUMENT_TYPE_REGISTER))  return 3;
                if((arg_t1 == ARGUMENT_TYPE_REGISTER && arg_t2 == ARGUMENT_TYPE_COND_JUMP)
                || (arg_t1 == ARGUMENT_TYPE_COND_JUMP && arg_t2 == ARGUMENT_TYPE_REGISTER)) return 4;
                if((arg_t1 == ARGUMENT_TYPE_REGISTER && arg_t2 == ARGUMENT_TYPE_EXT_REG)
                || (arg_t1 == ARGUMENT_TYPE_EXT_REG && arg_t2 == ARGUMENT_TYPE_REGISTER))   return 5;
                return -1;
            }
            return -1;
        }
        static int get_data_type(const AssemblyInstruction instruction) {
            if(instruction.data_count == 0) {
                return 0;
            }
            if(instruction.data_count == 1) {
                if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_ADDRESS) return 1;
                if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_DATA) return 3;
            }
            if(instruction.data_count == 2) {
                if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_ADDRESS 
                && instruction.data_arguments[1].argument_type == DATAARGUMENT_TYPE_DATA) return 2;
            }
            return 0;
        }
        static int convert_type(const AssemblyInstruction instruction) {
            Argument only_reg[instruction.argument_count];
            int reg_arg_count = 0;
            for(int i = 0; i < instruction.argument_count; i++) {
                if(instruction.arguments[i].argument_type == ARGUMENT_TYPE_REGISTER||instruction.arguments[i].argument_type == ARGUMENT_TYPE_ADDR_REG) {
                    memcpy(&only_reg[reg_arg_count++] , &instruction.arguments[i] , sizeof(Argument));
                }
            }
            
            if(reg_arg_count == 0) {
                if(instruction.data_count == 0)                                              return 0;
                if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_DATA)    return 1;
                if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_ADDRESS) return 2;
            }
            if(reg_arg_count == 1) {
                if(only_reg[0].argument_type == ARGUMENT_TYPE_REGISTER) {
                    if(instruction.data_count == 0)                                              return 3;
                    if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_DATA)    return 4;
                    if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_ADDRESS) return (instruction.data_arguments[0].arg_index > instruction.arguments[0].arg_index) ? 5 : 8;
                }
                else if(only_reg[0].argument_type == ARGUMENT_TYPE_ADDR_REG) {
                    if(instruction.data_count == 0)                                              return 6;
                    if(instruction.data_arguments[0].argument_type == DATAARGUMENT_TYPE_DATA)    return 7;
                }
            }
            if(reg_arg_count == 2) {
                if(only_reg[0].argument_type == ARGUMENT_TYPE_REGISTER) {
                    if(only_reg[1].argument_type == ARGUMENT_TYPE_REGISTER)  return 9;
                    if(only_reg[1].argument_type == ARGUMENT_TYPE_ADDR_REG)  return 10;
                }
                if(only_reg[0].argument_type == ARGUMENT_TYPE_ADDR_REG) {
                    if(only_reg[1].argument_type == ARGUMENT_TYPE_REGISTER)  return 11;
                }
            }
            return -1;
        }

        inline default_t convert_instruction_code(int main_instruction , int argument0 , int argument1) {
            return ((main_instruction & 0b111111)|((argument0 & 0b111) << INSTRUCTION_MAININSTRUCTION_SIZE)|((argument1 & 0b111) << (INSTRUCTION_MAININSTRUCTION_SIZE+INSTRUCTION_ARGUMENT_SIZE)));
        }

        VariableMarker *marker_store;
        std::vector<default_t>interpreted_codes;
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
        std::vector<AssemblyCode>asm_codes;
};

#endif