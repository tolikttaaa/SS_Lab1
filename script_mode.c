//
// Created by ttaaa on 4/22/21.
//

#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "fat32_lib.h"
#include "script_mode.h"
#include "utils.h"

void print_dir(struct dir_value *pValue) {
    while (pValue != NULL) {
        if (pValue->type == 'd') {
            printf("DIR %s %d \n", pValue->filename, pValue->first_cluster);
        } else {
            printf("FILE %s (%d bytes)\n", pValue->filename, pValue->size);
        }

        pValue = pValue->next;
    }
}

void copy_file(struct partition_value *part, char *destination, struct dir_value *file) {
    if (file->type != 'f') {
        printf("Not a file\n");
        return;
    }

    char *buf = malloc(part->cluster_size);
    unsigned int fat_record = file->first_cluster;
    int fd = open(destination, O_RDWR | O_APPEND | O_CREAT, 0777);
    unsigned int size = file->size < part->cluster_size ? file->size : part->cluster_size;

    while (fat_record < 0x0FFFFFF7) {
        fat_record = read_file_cluster(part, fat_record, buf);
        write(fd, buf, size);
    }

    free(buf);
    close(fd);
}

void copy_dir(struct partition_value *part, char *destination, struct dir_value *file) {
    if (file->type != 'd') {
        printf("Not a dir\n");
        return;
    }

    struct stat dir = {0};
    if (stat(destination, &dir) == -1) {
        mkdir(destination, 0777);
    }

    struct dir_value *dir_val = read_dir(file->first_cluster, part);
    while (dir_val != NULL) {
        if (strcmp((char*)  dir_val->filename, ".") != 0 && strcmp((char*) dir_val->filename, "..") != 0) {
            char *path = calloc(1, 512);
            strcat(path, destination);
            append_path_part(path, (char*) dir_val->filename);

            if (dir_val->type == 'd') {
                copy_dir(part, path, dir_val);
            } else {
                copy_file(part, path, dir_val);
            }

            free(path);
        }

        dir_val = dir_val->next;
    }
    destroy_dir_value(dir_val);
}

void print_help() {
    printf("cd [arg] - change working directory\n");
    printf("pwd - print working directory full name\n");
    printf("cp [arg] - copy dir or file to mounted device\n");
    printf("ls - show working directory elements\n");
    printf("exit - terminate program\n");
    printf("help - print help\n");
}

void ls_command(struct partition_value *partition) {
    struct dir_value *dir_value = read_dir(partition->active_cluster, partition);
    print_dir(dir_value);
    destroy_dir_value(dir_value);
}

void cd_command(struct partition_value *partition, char *dir, char *to) {
    if (change_dir(partition, (unsigned char*) to)) {
        if (!strcmp(to, "..")) {
            remove_until(dir, '/');
        } else if (!strcmp(".", to)) {
            // Do nothing
        } else {
            strcat(dir, "/");
            strcat(dir, to);
        }

        printf("%s\n", to);
    } else {
        printf("Dir doesn't exist.\n");
    }
}

int cp_command(struct partition_value *partition, char* source, char* to) {
    struct dir_value *dir_value = read_dir(partition->active_cluster, partition);
    char copied = 0;

    while (dir_value != NULL) {
        if (!strcmp((char*) dir_value->filename, source)) {
            if (check_directory(to)) {
                char filename[256] = {0};
                strcpy(filename, to);
                size_t str_len = strlen(to);

                if (filename[str_len - 1] != '/') {
                    strcat(filename, "/");
                }

                strcat(filename, (char *) dir_value->filename);

                if (dir_value->type == 'd') {
                    copy_dir(partition, filename, dir_value);
                } else {
                    copy_file(partition, filename, dir_value);
                }

                return 1;
            } else {
                printf("Directory doesn't exists\n");
            }
        }

        dir_value = dir_value->next;
    }

    destroy_dir_value(dir_value);
    return 0;
}

void run_script_mode(const char *part) {
    struct partition_value *partition = open_partition(part);

    if (partition) {
        printf("FAT32 supported.\n");

        char *current_dir = calloc(1, 512);
        strcat(current_dir, "/");
        strcat(current_dir, part);

        char *line;
        int exit = 0;

        while (!exit) {
            char *args[3];
            args[0] = calloc(1, 256);
            args[1] = calloc(1, 256);
            args[2] = calloc(1, 256);

            printf("%s$ ", current_dir);

            line = get_line();
            remove_ending_symbol(line, '\n');
            parse(line, args);

            if (!strcmp(args[0], "ls")) {
                ls_command(partition);
            } else if (!strcmp(args[0], "cd")) {
                cd_command(partition, current_dir, args[1]);
            } else if (!strcmp(args[0], "exit")) {
                exit = 1;
            } else if (!strcmp(args[0], "pwd")) {
                printf("%s\n", current_dir);
            } else if (!strcmp(args[0], "cp")) {
                if (cp_command(partition, args[1], args[2])) {
                    printf("Copied\n");
                } else {
                    printf("Dir/file not found\n");
                }
            } else if (!strcmp(args[0], "help")) {
                print_help();
            } else {
                fprintf(stderr, "Unknown command\n");
                print_help();
            }

            free(args[0]);
            free(args[1]);
            free(args[2]);
        }

        close_partition(partition);
    } else {
        printf("FAT32 not supported.\n");
    }
}
