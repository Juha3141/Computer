#include <iostream>
#include <vector>
#include <process_asm.hpp>
#include <instruction.hpp>
#include <string>
#include <fstream>

int main(int argc , char **argv) {
    if(argc == 1) {
        std::cout<<"Usage : "<<argv[0]<<" [file name] [options]"<<"\n";
        return -1;
    }

    // process entire file and store contents to vector
    int current_line = 0;
    std::ifstream file;
    file.open(argv[1] , std::ifstream::in);
    std::vector<std::pair<AssemblyInstruction,int>>full_contents;
    if(!file.is_open()) {
        std::cout<<"File \""<<argv[1]<<"\" not found!\n";
        return -1;
    }
    InstructionController instruction_controller(&full_contents);
    int offset = 0;
    std::string temp_str;
    while(1) {
        if(!getline(file , temp_str)) break;
        // copy string
        char *c_string = new char[temp_str.size()];
        strcpy(c_string , temp_str.c_str());

        std::cout<<"preprocessing string : \""<<temp_str<<"\"\n";
        int instr_type = InstructionController::preprocessing(c_string);
        std::cout<<"preprocessed         : \""<<c_string<<"\"\n";
        if(ignore(c_string) == true) {
            delete c_string;
            std::cout<<"ignoring\n";
            AssemblyInstruction none;
            full_contents.push_back(std::make_pair(none , -1));
            current_line++;
            continue;
        }
        if(instr_type == -1) {
            std::cout<<"error in line "<<current_line<<"\n";
            return -1;
        }
        
        AssemblyInstruction instruction;

        int instr_size = instruction_controller.process_instruction(&instruction , c_string , instr_type);
        int second = offset;
        printf("instr_size = %d\n" , instr_size);
        if(instruction.instruction_type == INSTRUCTION_TYPE_LABEL) {
            // if it's control -> 
            instruction_controller.marker_store->store_mark(instruction.instruction , offset);
            second = -1;
        }
        if(instruction.instruction_type == INSTRUCTION_TYPE_STRING) {
            offset += strlen(instruction.arguments[0].string);
        }
        full_contents.push_back(std::make_pair(instruction , second));
        offset += instr_size; 
        current_line++;
        delete c_string;
    }
    printf("--------------------- converting lines into machine codes --------------------- \n");
    for(int i = 0; i < current_line; i++) {
        std::cout<<"procesing line "<<i+1<<"\n";
        if(instruction_controller.process_line(i) == false) {
            printf("Error at line %d\n" , i+1);
            return 0;
        }
    }
    std::cout<<"done processing ------------ \n";
    for(auto i = instruction_controller.interpreted_codes.begin(); i != instruction_controller.interpreted_codes.end(); i++) {
        printf("%04X " , *i);
    }
    std::cout<<"\n";
    return 0;
}