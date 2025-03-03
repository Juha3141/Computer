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

    int label = 0;
    // remove unwanted characters
    for(int i = 0; oneline_assembly[i] != 0x00; i++) {
        if(oneline_assembly[i] == ';') {
            oneline_assembly[i] = 0x00;
            break;
        }
    }
    printf("check1\n");
    remove_first_end_spaces(oneline_assembly);
    for(int i = 0; oneline_assembly[i] != 0x00; i++) {
        if(oneline_assembly[i] == ':') {
            label = i;
        }
    }
    printf("check2\n");
    if(!label) {
        for(int i = 0; oneline_assembly[i] != ' ' && oneline_assembly[i] != '(' && oneline_assembly[i] != 0x00; i++) {
            oneline_assembly[i] = tolower(oneline_assembly[i]);
        }
    }
    printf("check3\n");
    for(instruction_end = strlen(oneline_assembly)-1; oneline_assembly[instruction_end] == ' '; instruction_end--) {}
    printf("instruction_end : %d(l=%d)\n" , instruction_end , strlen(oneline_assembly));
    if((instruction_type = get_instruction_type(oneline_assembly)) < 0) {
        return -1;
    }
    printf("check4\n");
    if(instruction_end != strlen(oneline_assembly)-1) {
        remove_spaces(oneline_assembly+instruction_end);
    }
    printf("check5\n");
    std::cout<<"preprocessed : "<<oneline_assembly<<"\n";
    return instruction_type;
}

#define REGEX_DEFINITIONS \
    std::regex *argument_regex[] = {\
        new std::regex("[rR]{1}\\d+") ,                            /* register */\
        new std::regex("\\*[rR]{1}\\d+") ,                         /* register-address */\
        new std::regex("[$]\\w+") ,                                /* special register */\
        new std::regex("[#](J|Z|U|S|C|O){1}") ,                    /* conditional jump */\
        new std::regex("[%](ADD|SUB|MUL|DIV|AND|OR|XOR|NOT){1}") , /* alu operation */\
    };\
    std::regex *data_regex[] = {\
        new std::regex("0[xX]{1}[0-9A-Fa-f]+") ,    /* hex data */\
        new std::regex("0[bB]{1}[01]+") ,           /* binary data */\
        new std::regex("\\d+") ,                    /* data */\
        new std::regex("\\*0[xX]{1}[0-9A-Fa-f]+") , /* hex address */\
        new std::regex("\\*0[bB]{1}[01]+") ,        /* binary address */\
        new std::regex("\\*\\d+") ,                 /* address */\
        new std::regex("\\w+") ,                    /* label */\
        new std::regex("\\*\\w+") ,                 /* label-address */\
    };

bool InstructionController::process_data_argument(DataArgument &arg_info , const char *asm_arg) {
    int type = -1;
    REGEX_DEFINITIONS
    for(int i = 0; i < sizeof(argument_regex)/sizeof(std::regex*); i++) {
        if(std::regex_match(asm_arg , *argument_regex[i]) == true) { // if it's register
            return false; // return false, not considering
        }
    }

    for(int i = 0; i < sizeof(data_regex)/sizeof(std::regex*); i++) {
        if(std::regex_match(asm_arg , *data_regex[i]) == true) {
            type = i;
            break;
        }
    }
    if(type == -1) {
        return false;
    }
    arg_info.data = 0;
    arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
    arg_info.label = 0x00;
    switch(type) {
        case 0: // hex data
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg+2 , 0x00 , 16); // asm_arg+2 : To remove hex prefix "0x"
            break;
        case 1: // binary data
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg+2 , 0x00 , 2);
            break;
        case 2: // decimal
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.data = strtol(asm_arg , 0x00 , 10);
            break;
        case 3: // hex address
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+3 , 0x00 , 16);
            break;
        case 4: // binary address
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+3 , 0x00 , 2);
            break;
        case 5: // address
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.data = strtol(asm_arg+1 , 0x00 , 10);
            break;
        case 6: { // label
            arg_info.argument_type = DATAARGUMENT_TYPE_DATA;
            arg_info.label = new char[strlen(asm_arg)];
            strcpy(arg_info.label , asm_arg);
            break;
        }
        case 7: { // label address (*LABEL)
            arg_info.argument_type = DATAARGUMENT_TYPE_ADDRESS;
            arg_info.label = new char[strlen(asm_arg)];
            strcpy(arg_info.label , asm_arg+1);
            break;
        }
        case 8: { // string
            arg_info.argument_type = DATAARGUMENT_TYPE_STRING;
            arg_info.string = new char[strlen(asm_arg)];
            strncpy(arg_info.string , asm_arg+1 , strlen(asm_arg)-2);
            std::cout<<"     type:string,\""<<arg_info.string<<"\n";
            break;
        }
    }
    return true;
}

static void string_upper(char *target , const char *source) {
    int i = 0;
    for(; source[i] != 0x00; i++) {
        target[i] = toupper(source[i]);
    }
    target[i] = 0x00;
}

bool InstructionController::process_argument(Argument &arg_info , const char *asm_arg) {
    int type = -1;
    REGEX_DEFINITIONS
    char new_higher[strlen(asm_arg)];
    string_upper(new_higher , asm_arg); // uppercase the argument
    std::cout<<"upper-case arg : "<<new_higher<<"\n";

    for(int i = 0; i < sizeof(argument_regex)/sizeof(std::regex*); i++) {
        if(std::regex_match(new_higher , *argument_regex[i]) == true) {
            type = i;
            break;
        }
    }
    if(type == -1) {
        return false;
    }
    arg_info.register_number = 0;
    arg_info.ext_reg_number = 0;
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
        case 2: // Extra register options
            arg_info.argument_type = 4;
            if(!strcmp(new_higher+1 , "A")) arg_info.ext_reg_number = 0;     // Register A
            if(!strcmp(new_higher+1 , "B")) arg_info.ext_reg_number = 1;     // Register B
            if(!strcmp(new_higher+1 , "BP")) arg_info.ext_reg_number = 2;
            if(!strcmp(new_higher+1 , "SP")) arg_info.ext_reg_number = 3;
            if(!strcmp(new_higher+1 , "DIVOUT")) arg_info.ext_reg_number = 4;
            if(!strcmp(new_higher+1 , "PORTADDR")) arg_info.ext_reg_number = 5;  // Port Address
            if(!strcmp(new_higher+1 , "PORTRW")) arg_info.ext_reg_number = 6; // Port R/W Status
            if(!strcmp(new_higher+1 , "PORTE")) arg_info.ext_reg_number = 7; // Port Enable
            break;
        case 3: { // Jump options
            arg_info.argument_type = 5;
            if(new_higher[1] == 'J') arg_info.conditional_jump = 0; // Unconditional Jump
            if(new_higher[1] == 'Z') arg_info.conditional_jump = 1; // Jump if result is zero
            if(new_higher[1] == 'U') arg_info.conditional_jump = 2; // Jump if result is unsigned (+)
            if(new_higher[1] == 'S') arg_info.conditional_jump = 3; // Jump if result is signed (-)
            if(new_higher[1] == 'C') arg_info.conditional_jump = 4; // Jump if carry
            if(new_higher[1] == 'O') arg_info.conditional_jump = 5; // Jump if overflow
            break;
        }
        case 4: { // Arithmetic operation options
            arg_info.argument_type = 6;
            if(!strcmp(new_higher+1 , "ADD")) arg_info.alu_operation = 0;
            if(!strcmp(new_higher+1 , "SUB")) arg_info.alu_operation = 1;
            if(!strcmp(new_higher+1 , "MUL")) arg_info.alu_operation = 2;
            if(!strcmp(new_higher+1 , "DIV")) arg_info.alu_operation = 3;
            if(!strcmp(new_higher+1 , "AND")) arg_info.alu_operation = 4;
            if(!strcmp(new_higher+1 , "OR"))  arg_info.alu_operation = 5;
            if(!strcmp(new_higher+1 , "XOR")) arg_info.alu_operation = 6;
            if(!strcmp(new_higher+1 , "NOT")) arg_info.alu_operation = 7;
            break;
        }
    }
    return true;
}

/// @brief 
/// @param instruction 
/// @param oneline_assembly 
/// @param instruction_type 
/// @return 
int InstructionController::process_instruction(AssemblyInstruction *instruction , const char *oneline_assembly , int instruction_type) {
    int instruction_size = 1;
    int total_argument_count = get_argument_count(instruction_type)+1;
    
    char *arguments[total_argument_count];
    // allocate space for argument and instruction
    for(int i = 0; i < total_argument_count; i++) {
        arguments[i] = new char[strlen(oneline_assembly)];
    }

    instruction->instruction = new char[strlen(oneline_assembly)];
    instruction->instruction_type = instruction_type;

    std::cout<<"total argument count : "<<total_argument_count<<"\n";
    parse_from_asm(oneline_assembly , instruction->instruction , arguments , instruction_type);
    if(instruction_type == INSTRUCTION_TYPE_LABEL) { 
        // control(label), save
        instruction->argument_count = 0;
        instruction->arguments = 0x00;
        instruction->data_count = 0;
        instruction->data_arguments = 0x00;
        return 0;
    }
    if(instruction_type == INSTRUCTION_TYPE_STRING) {
        instruction->data_count = 0;
        instruction->data_arguments = 0x00;
        // string argument
        instruction->argument_count = 1;
        instruction->arguments = new Argument[1];
        std::regex str_regex("\\w+[ ]+([\"|\'].+[\"|\'])"); // [instruction](string)
        
        std::smatch match;
        std::string str = std::string(oneline_assembly);
        regex_search(str , match , str_regex);
        int len = match[1].str().length();
        instruction->arguments[0].string = new char[len+1];
        strncpy(instruction->arguments[0].string , match[1].str().c_str()+1 , len-2); // truncate the quotation marks

        return instruction_size;
    }
    std::cout<<"instruction:"<<instruction->instruction<<"\n";
    for(int i = 0; i < total_argument_count; i++) {
        std::cout<<"arg"<<i<<":"<<arguments[i]<<"\n";
    }

    instruction->arguments = new Argument[total_argument_count];
    instruction->data_arguments = new DataArgument[total_argument_count];
    
    int count = 0;
    for(int i = 0; i < total_argument_count; i++) {
        if(process_data_argument(instruction->data_arguments[count] , arguments[i]) == true) {
            instruction->data_arguments[count].arg_index = i;
            count++;
            strcpy(arguments[i] , ""); // make sure not to process this argument in next routine
        }
    }
    instruction->data_count = count;
    count = 0;
    for(int i = 0; i < total_argument_count; i++) {
        if(process_argument(instruction->arguments[count] , arguments[i]) == true) {
            instruction->arguments[count].arg_index = i;
            printf("processing \"argument\" : %d\n" , count);
            count++;
        }
    }
    instruction->argument_count = count;
    instruction_size += instruction->data_count;
    
    if(instruction->arguments[0].argument_type == ARGUMENT_TYPE_EXT_REG
    ||instruction->arguments[0].argument_type == ARGUMENT_TYPE_COND_JUMP
    ||instruction->arguments[0].argument_type == ARGUMENT_TYPE_ALU_OPER) {
        printf("sorting argument\n");
        std::sort(instruction->arguments , instruction->arguments+instruction->argument_count , [](Argument a,Argument b){
            return a.argument_type<b.argument_type;
        });
    }
    // for debugging
    for(int i = 0; i < instruction->data_count; i++) {
        printf("---- Type : Data(%d) ----\n" , i);
        printf("  data type : %d\n" , instruction->data_arguments[i].argument_type);
        printf("  content   : %d\n" , instruction->data_arguments[i].data);
        printf("  label     : %s\n" , instruction->data_arguments[i].label);
    }
    for(int i = 0; i < instruction->argument_count; i++) {
        printf("---- Type : Arg(%d) ----\n" , i);
        printf("  type    : %d\n" , instruction->arguments[i].argument_type);
        printf("  reg_num : %d\n" , instruction->arguments[i].register_number);
        printf("  ext_reg : %d\n" , instruction->arguments[i].ext_reg_number);
        printf("  c. jmp  : %d\n" , instruction->arguments[i].conditional_jump);
        printf("  alu op. : %d\n" , instruction->arguments[i].alu_operation);
    }
    std::cout<<"--------------"<<"\n";
    std::cout<<"data_count     : "<<instruction->data_count<<"\n";
    std::cout<<"argument_count : "<<instruction->argument_count<<"\n";
    return instruction_size;
}

static int get_argument(AssemblyInstruction instruction , int index) {
    switch(instruction.arguments[index].argument_type) {
        case ARGUMENT_TYPE_REGISTER:
            return instruction.arguments[index].register_number;
        case ARGUMENT_TYPE_ADDR_REG:
            return instruction.arguments[index].register_number;
        case ARGUMENT_TYPE_ALU_OPER:
            return instruction.arguments[index].alu_operation;
        case ARGUMENT_TYPE_COND_JUMP:
            return instruction.arguments[index].conditional_jump;
        case ARGUMENT_TYPE_EXT_REG:
            return instruction.arguments[index].ext_reg_number;
        default:
            return 0;
    }
}

int InstructionController::convert_instruction(AssemblyInstruction instruction) {
    default_t numeric_main_instr = 0x00;
    AssemblyCode *code = get_assembly_code_info(instruction.instruction);
    if(code == 0x00) {
        // error
        return -1;
    }
    // real instruction type : the instruction type from "ISA spreadsheet"
    int real_instruction_type = get_real_instruction_type(instruction);
    int data_type = get_data_type(instruction);
    std::cout<<"real instr. type : "<<real_instruction_type<<"\n";
    std::cout<<"data type        : "<<data_type<<"\n";
    
    int type_instruction = convert_type(instruction);
    std::cout<<"instruction sel. : "<<type_instruction<<"\n";
    numeric_main_instr = code->type_instructions[type_instruction];
    printf("Main Instruction : 0x%X\n" , numeric_main_instr);
    if(numeric_main_instr == 0xFF) {
        printf("Invalid use of instruction\n");
        return -1;
    }
    if(numeric_main_instr == 0xDD) {
        if(instruction.argument_count != 1) {
            return 0;
        }
        return instruction.data_arguments[0].data;
    }
    
    int argument0 = get_argument(instruction , 0);
    int argument1 = get_argument(instruction , 1);
    printf("argument 0 : %d\n" , argument0);
    printf("argument 1 : %d\n" , argument1);
    
    default_t full_instruction = convert_instruction_code(numeric_main_instr , argument0 , argument1);
    return full_instruction;
}

bool InstructionController::process_line(int line) {
    if(asm_source->at(line).second == -1) {
        std::cout<<"skipping line "<<line<<"\n";
        return true;
    }
    AssemblyInstruction *instruction = &(asm_source->at(line).first);
    if(instruction->instruction_type == INSTRUCTION_TYPE_STRING) {
        printf("string! \"%s\"\n" , instruction->arguments[0].string);
        int len = strlen(instruction->arguments[0].string);
        for(int i = 0; i < len; i++) {
            interpreted_codes.push_back((default_t)instruction->arguments[0].string[i]);
        }
        // end of string
        interpreted_codes.push_back(0x00);
        return true;
    }
    default_t full_instruction = convert_instruction(asm_source->at(line).first);
    if(full_instruction == 0xFFFF) {
        return false;
    }

    interpreted_codes.push_back(full_instruction);
    printf("full instruction : 0x%04X\n" , full_instruction);
    printf("full data        : ");
    for(int i = 0; i < instruction->data_count; i++) {
        default_t data = instruction->data_arguments[i].data;
        if(instruction->data_arguments[i].label != 0x00) {
            unsigned int u_data = marker_store->load_mark(instruction->data_arguments[i].label);
            if(u_data == 0xFFFFFFFF) {
                printf("label not found, name : %s\n" , instruction->data_arguments[i].label);
                return false;
            }
            data = u_data;
        }
        interpreted_codes.push_back(data);
    }
    printf("\n");
    return true;
}
