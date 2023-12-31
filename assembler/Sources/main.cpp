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

        // To-do : create conditional loop system
        int instr_size = instruction_controller.process_instruction(&instruction , c_string , instr_type);
        int second = offset;
        if(instruction.instruction_type == 0) {
            // if it's control -> 
            instruction_controller.marker_store->store_mark(instruction.instruction , offset);
            second = -1;
        }
        full_contents.push_back(std::make_pair(instruction , second));
        offset += instr_size; 
        current_line++;
        delete c_string;
    }
    for(int i = 0; i < current_line; i++) {
        std::cout<<"procesing line "<<i<<"\n";
        instruction_controller.process_line(i);
    }
    std::cout<<"done processing ------------ \n";
    for(auto i = instruction_controller.interpreted_codes.begin(); i != instruction_controller.interpreted_codes.end(); i++) {
        printf("%04X " , *i);
    }
    std::cout<<"\n";
    return 0;
}