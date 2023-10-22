#include <iostream>
#include <regex>
#include <instruction.hpp>

bool VariableMarker::store_mark(char *marker_name , unsigned int memory_addr) {
    if(load_mark(marker_name) != 0xFFFFFFFF) {
        return false;
    }
    markers.push_back(std::make_pair(marker_name , memory_addr));
    return true;
}

unsigned int VariableMarker::load_mark(const char *marker_name) {
    for(auto i = markers.begin(); i != markers.end(); i++) {
        if(strcmp(i->first , marker_name) == 0) {
            return i->second;
        }
    }
    return 0xFFFFFFFF;
}


int InstructionController::preprocessing(char *oneline_assembly) {
    int instruction_type;
    int instruction_end;
    // remove unwanted characters
    remove_first_end_spaces(oneline_assembly);
    for(int i = 0; oneline_assembly[i] != ' ' && oneline_assembly[i] != '('; i++) {
        oneline_assembly[i] = tolower(oneline_assembly[i]);
    }
    for(instruction_end = 0; oneline_assembly[instruction_end] != ' '; instruction_end++) {}
    if((instruction_type = get_instruction_type(oneline_assembly)) < 0) {
        return -1;
    }
    if(instruction_end != strlen(oneline_assembly)-1) {
        remove_spaces(oneline_assembly+instruction_end+1);
    }
    std::cout<<"preprocessed : "<<oneline_assembly<<"\n";
    return instruction_type;
}

bool InstructionController::process_data_argument(DataArgument &arg_info , const char *asm_arg) {
    int type = -1;
    std::regex *regs[] = {
        new std::regex("0[xX]{1}[0-9A-Fa-f]+") ,  // hex data
        new std::regex("0[bB]{1}[01]+") ,  // binary data
        new std::regex("\\d+") ,    // data
        new std::regex("\\*0[xX]{1}[0-9A-Fa-f]+") , // hex address
        new std::regex("\\*0[bB]{1}[01]+") ,  // binary address
        new std::regex("\\*\\d+") ,   // address
    };
    for(int i = 0; i < sizeof(regs)/sizeof(std::regex*); i++) {
        if(std::regex_match(asm_arg , *regs[i]) == true) {
            type = i;
            break;
        }
    }
    if(type == -1) {
        return false;
    }
    arg_info.data = 0;
    arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
    switch(type) {
        case 0:
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg+2 , 0x00 , 16);
            break;
        case 1:
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg+2 , 0x00 , 2);
            break;
        case 2:
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg , 0x00 , 10);
            break;
        case 3:
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+3 , 0x00 , 16);
            break;
        case 4:
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+3 , 0x00 , 2);
            break;
        case 5:
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+1 , 0x00 , 10);
            break;
    }
    return true;
}

bool InstructionController::process_argument(Argument &arg_info , const char *asm_arg) {
    int type = -1;
    std::regex *regs[] = {
        new std::regex("[rR]{1}\\d+") ,                            // register
        new std::regex("\\*[rR]{1}\\d+") ,                         // register-address
        new std::regex("[$]\\w+") ,                                // special register
        new std::regex("[#](J|Z|U|S|C|O){1}") ,                    // conditional jump
        new std::regex("[%](ADD|SUB|MUL|DIV|AND|OR|XOR|NOT){1}") , // alu operation
        new std::regex("\\w+") ,                                   // label
    };
    for(int i = 0; i < sizeof(regs)/sizeof(std::regex*); i++) {
        if(std::regex_match(asm_arg , *regs[i]) == true) {
            type = i;
            break;
        }
    }
    if(type == -1) {
        return false;
    }
    arg_info.register_number = 0;
    arg_info.label_name = 0x00;
    arg_info.ext_reg = 0x00;
    arg_info.conditional_jump = 0;
    arg_info.alu_operation = 0;
    switch(type) {
        case 0:
            arg_info.argument_type = 1;
            arg_info.register_number = atoi(asm_arg+1);
            break;
        case 1:
            arg_info.argument_type = 2;
            arg_info.register_number = atoi(asm_arg+2);
            break;
        case 2:
            arg_info.argument_type = 4;
            arg_info.ext_reg = new char[strlen(asm_arg)];
            strcpy(arg_info.ext_reg , asm_arg+1);
            break;
        case 3: {
            arg_info.argument_type = 5;
            if(asm_arg[1] == 'J') arg_info.conditional_jump = 0;
            if(asm_arg[1] == 'Z') arg_info.conditional_jump = 1;
            if(asm_arg[1] == 'U') arg_info.conditional_jump = 2;
            if(asm_arg[1] == 'S') arg_info.conditional_jump = 3;
            if(asm_arg[1] == 'C') arg_info.conditional_jump = 4;
            if(asm_arg[1] == 'O') arg_info.conditional_jump = 5;
            break;
        }
        case 4: {
            arg_info.argument_type = 6;
            if(!strcmp(asm_arg+1 , "ADD")) arg_info.alu_operation = 0;
            if(!strcmp(asm_arg+1 , "SUB")) arg_info.alu_operation = 1;
            if(!strcmp(asm_arg+1 , "MUL")) arg_info.alu_operation = 2;
            if(!strcmp(asm_arg+1 , "DIV")) arg_info.alu_operation = 3;
            if(!strcmp(asm_arg+1 , "AND")) arg_info.alu_operation = 4;
            if(!strcmp(asm_arg+1 , "OR"))  arg_info.alu_operation = 5;
            if(!strcmp(asm_arg+1 , "XOR")) arg_info.alu_operation = 6;
            if(!strcmp(asm_arg+1 , "NOT")) arg_info.alu_operation = 7;
            break;
        case 5:
            arg_info.argument_type = 3;
            arg_info.label_name = new char[strlen(asm_arg)];
            strcpy(arg_info.label_name , asm_arg);
            break;
        }
    }
    return true;
}

/// @brief 
/// @param instruction 
/// @param oneline_assembly 
/// @param instruction_type 
/// @return return true if it is 
int InstructionController::process_instruction(AssemblyInstruction *instruction , const char *oneline_assembly , int instruction_type) {
    int instruction_size = 1;
    int total_argument_count = get_argument_count(instruction_type);
    if(total_argument_count == 0) {
        return 0; // error
    }

    char *arguments[total_argument_count];
    // allocate space for argument and instruction
    for(int i = 0; i < total_argument_count; i++) {
        arguments[i] = new char[strlen(oneline_assembly)];
    }
    instruction->instruction = new char[strlen(oneline_assembly)];
    instruction->instruction_type = instruction_type;

    std::cout<<"total argument count : "<<total_argument_count<<"\n";
    parse_from_asm(oneline_assembly , instruction->instruction , arguments , instruction_type);
    std::cout<<"instruction:"<<instruction->instruction<<"\n";
    for(int i = 0; i < total_argument_count; i++) {
        std::cout<<"arg"<<i<<":"<<arguments[i]<<"\n";
    }

    instruction->arguments = new Argument[total_argument_count];
    instruction->data_arguments = new DataArgument[total_argument_count];

    int count;
    for(int i = 0 , count = 0; i < total_argument_count; i++) {
        if(process_data_argument(instruction->data_arguments[count] , arguments[i]) == true) {
            /*
            printf("---- Type : Data ----\n");
            printf("  data type : %d\n" , instruction->data_arguments[count].argument_type);
            printf("  content   : %d\n" , instruction->data_arguments[count].data);
            */
            count++;
            strcpy(arguments[i] , ""); // make sure not to process this argument in next routine
        }
    }
    instruction->data_count = count;
    
    for(int i = 0 , count = 0; i < total_argument_count; i++) {
        if(process_argument(instruction->arguments[count] , arguments[i]) == true) {
            /*
            printf("---- Type : Arg  ----\n");
            printf("  type    : %d\n" , instruction->arguments[count].argument_type);
            printf("  reg_num : %d\n" , instruction->arguments[count].register_number);
            printf("  label   : %s\n" , instruction->arguments[count].label_name);
            printf("  ext_reg : %s\n" , instruction->arguments[count].ext_reg);
            printf("  c. jmp  : %d\n" , instruction->arguments[count].conditional_jump);
            printf("  alu op. : %d\n" , instruction->arguments[count].alu_operation);
            */
            count++;
        }
    }
    instruction->argument_count = count;
    return instruction_size;
}

default_t InstructionController::convert_instruction(AssemblyInstruction instruction) {
    default_t numeric_main_instr = 0x00;
    AssemblyCode *code = get_assembly_code_info(instruction.instruction);
    if(code == 0x00) {
        // error
        return -1;
    }
    // real instruction type : the instruction type from "ISA spreadsheet"
    int real_instruction_type = get_real_instruction_type(instruction);
    std::cout<<"real instr. type : "<<real_instruction_type<<"\n";
    // todo : compare AssemblyInstruction & AssemblyCode -> determine numeric_main_instr
    // + determine arg1 , arg2 ***
}

void InstructionController::save_loop(AssemblyInstruction instruction) {
    if(instruction.instruction_type != 1) {
        return;
    }
    if(instruction.arguments == 0x00) {
        return;
    }
    marker_store->store_mark(instruction.arguments[0].label_name , this->current_position);
}

bool InstructionController::process_line(int line) {
    convert_instruction(asm_source->at(line).first);
    return true;
}