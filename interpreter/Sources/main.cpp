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
    std::ifstream file(argv[1]);
    std::vector<std::pair<AssemblyInstruction,int>>full_contents;
    if(!file.is_open()) {
        std::cout<<"File \""<<argv[1]<<"\" not found!\n";
        return -1;
    }
    InstructionController instruction_controller(&full_contents);
    int offset = 0;
    while(1) {
        std::string temp_str;
        if(!getline(file , temp_str)) break;
        // copy string
        char *c_string = new char[temp_str.size()];
        strcpy(c_string , temp_str.c_str());

        std::cout<<"preprocessing string : "<<temp_str<<"\n";
        int instr_type = InstructionController::preprocessing(c_string);
        if(instr_type == -1) {
            std::cout<<"error in line "<<current_line<<"\n";
            return -1;
        }
        
        AssemblyInstruction instruction;
        int instr_size = InstructionController::process_instruction(&instruction , c_string , instr_type);
        
        full_contents.push_back(std::make_pair(instruction , offset));
        offset += instr_size; 
        current_line++;
        
    }
    file.close();
    for(int i = 0; i < current_line; i++) {
        std::cout<<"procesing line "<<i<<"\n";
        instruction_controller.process_line(i);
    }
}