#include <stdio.h>
#include <string.h>
#include "list_mode.h"
#include "script_mode.h"

char *const SCRIPT_MODE = "script";
char *const MOUNTS_LIST_MODE = "list";

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Please, specify program mode (%s/%s)\n", SCRIPT_MODE, MOUNTS_LIST_MODE);
        return 0;
    } else {
        char *mode = argv[1];
        if (!strcmp(SCRIPT_MODE, mode)) {
            if (argc < 3) {
                printf("You have to specify target device with FAT32 fs\n");
                printf("usage: ./lab1 script sda1\n");
                return 0;
            }

            printf("Starting program in script mode...\n");
            run_script_mode(argv[2]);
        } else if (!strcmp(MOUNTS_LIST_MODE, mode)) {
            printf("Starting program in mounts mode...\n");
            run_list_mode();
        } else {
            printf("Unknown mode, terminating program!\n");
        }
        printf("Finish working!");
    }
    return 0;
}