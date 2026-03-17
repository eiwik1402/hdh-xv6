#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/param.h"

void print_indent(int depth) {
    for(int i = 0; i < depth; i++)
        printf("  ");
}

void tree(char *path, int depth) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "tree: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if(st.type != T_DIR){
        print_indent(depth);
        printf("%s\n", path);
        close(fd);
        return;
    }

  // Print current directory name with '/'
    print_indent(depth);
    char *name = path;
    for(char *q = path; *q; q++)
        if(*q == '/') name = q + 1;
    printf("%s/\n", name);

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("tree: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;

    // Copy name
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    // Skip "." and ".."
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

    if(stat(buf, &st) < 0){
        fprintf(2, "tree: cannot stat %s\n", buf);
        continue;
    }

    if(st.type == T_DIR){
        tree(buf, depth + 1);
        p = buf + strlen(path) + 1;
    } else {
        print_indent(depth + 1);
        printf("%s\n", de.name);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc > 2){
        fprintf(2, "Usage: tree [directory]\n");
        exit(1);
    }
    if(argc == 1)
        tree(".", 0);
    else
        tree(argv[1], 0);
  exit(0);
}