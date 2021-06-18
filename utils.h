//
// Created by ttaaa on 4/22/21.
//

#ifndef SYSTEM_SOFTWARE_UTILS_H
#define SYSTEM_SOFTWARE_UTILS_H

int parse(const char *cmd, char **args);
void append_path_part(char *path, const char *part);
char *get_line(void);
void remove_ending_symbol(char *str, char sym);
void remove_until(char *str, char sym);
int check_directory(const char *path);
int startsWith(const char *str, const char *pre);

#endif //SYSTEM_SOFTWARE_UTILS_H
