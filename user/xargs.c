#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG];
    int i;

    if (argc < 2) {
        fprintf(2, "Usage: xargs [command] [initial-args...]\n");
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    while (1) {
        int m = 0;
        int n;
        
        while (1) {
            n = read(0, &buf[m], 1);
            if (n <= 0 || buf[m] == '\n') break;
            m++;
        }

        if (n <= 0 && m == 0) break;

        buf[m] = 0;

        args[argc - 1] = buf;
        args[argc] = 0; 

        if (fork() == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec failed\n");
            exit(1);
        }
        
        wait(0);
    }
    exit(0);
}
