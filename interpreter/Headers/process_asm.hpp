#ifndef _PROCESS_ASM_HPP_
#define _PROCESS_ASM_HPP_

#include <cstring>

void parse_instruction(const char *assembly , char *instruction);
void remove_first_end_spaces(char *string);
void remove_spaces(char *string);
int parse_from_asm(const char *assembly , char *instruction , char **argument , int instruction_type);

int get_instruction_type(const char *string);

#endif