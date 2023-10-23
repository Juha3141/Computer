#include <iostream>
#include <process_asm.hpp>
#include <regex>
#include <vector>

/// @brief Remove spaces from first part and end part of string
/// @param string target string
void remove_first_end_spaces(char *string) {
    int i;
    for(int i = 0; (string[i] != 0x00 && string[i] == ' '); i++) {
        memcpy(string+i , string+i+1 , strlen(string)-i);
        i--;
    }
    for(int i = strlen(string)-1; (i >= 0 && string[i] == ' '); i--) {
        memcpy(string+i , string+i+1 , strlen(string)-i);
    }
}

/// @brief Remove space from string
/// @param string target string
void remove_spaces(char *string) {
    int i;
    for(int i = 0; string[i] != 0x00; i++) {
        if(string[i] == ' ') {
            memcpy(string+i , string+i+1 , strlen(string)-i);
            i--;
        }
    }
}

int get_instruction_type(const char *string) {
    std::regex *regs[] = {
        new std::regex("\\w+[ ]*:[ ]*") , // [control]:
        new std::regex("\\w+[ ]*") ,  // [instruction]
        new std::regex("\\w+[ ]+[A-Za-z\\!-9]+[ ]*,[ ]*[A-Za-z\\!-9]+") , // [instruction] [argument 1],[argument 2]
        new std::regex("\\w+[ ]+[A-Za-z\\!-9]+[ ]*") , // [instruction] [argument 1]
        new std::regex("\\w+\\([A-Za-z\\!-9]+\\)[ ]+[A-Za-z\\!-9]+[ ]*") , // [instruction](argument 1) [argument 2]
        new std::regex("\\w+\\([A-Za-z\\!-9]+\\)[ ]*") , // [instruction](argument 1)
    };
    for(int i = 0; i < sizeof(regs)/sizeof(std::regex*); i++) {
        if(std::regex_match(string , *regs[i]) == true) {
            std::cout<<"get_instruction_type:type matched,i:"<<i<<"\n";
            return i;
        }
    }
    std::cout<<"get_instruction_type:type not matched\n";
    return -1;
}

/// @brief Get the argument in assembly code
/// @param assembly string assembly code
/// @param argument return
/// @return number of argument
int parse_from_asm(const char *assembly , char *instruction , char **argument , int instruction_type) {
    int argument_count = 0;
    std::smatch match;
    std::regex *regs[] = {
        new std::regex("([\\w]+)[ ]*\\:[ ]*") , // [control]:
        new std::regex("([\\w]+)[ ]*") ,        // [instruction]
        new std::regex("([\\w]+)[ ]+([A-Za-z!-9]+)[ ]*\\,[ ]*([A-Za-z!-9]+)[ ]*") , // [instruction] [argument 1],[argument 2]
        new std::regex("([\\w]+)[ ]+([A-Za-z!-9]+)[ ]*") , // [instruction] [argument 1]
        new std::regex("([\\w]+)\\(([A-Za-z!-9]+)\\)[ ]+([A-Za-z!-9]+)[ ]*") , // [instruction](argument 1) [argument 2]
        new std::regex("([\\w]+)\\(([A-Za-z!-9]+)\\)[ ]*") , // [instruction](argument 1)
    };
    if(instruction_type > 5||instruction_type < 0) {
        return 0;
    }
    std::cout<<"instruction_type : "<<instruction_type<<"\n";
    std::string newstr = assembly;
    if(std::regex_search(newstr , match , *regs[instruction_type]) == false) {
        return 0;
    }
    strcpy(instruction , match[1].str().c_str());
    for(int i = 0; i < match.size()-2; i++) { // skip first and second
        std::cout<<"arg "<<i<<":"<<match[i+2].str()<<"\n";
        strcpy(argument[argument_count++] , match[i+2].str().c_str());
    }
    return argument_count;
}

bool ignore(const char *oneline_asm) {
    if(strcmp(oneline_asm , "") == 0) {
        return true;
    }
    return false;
}