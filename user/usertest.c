#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int run(char **args) {
  int pid = fork();
  if (pid < 0) return -1;
  if (pid == 0) {
    exec(args[0], args);
    exit(1);
  }
  int status;
  wait(&status);
  return status;
}

void mkfile(char *path) {
  int fd = open(path, O_CREATE | O_RDWR);
  if (fd >= 0) close(fd);
}

int test_xargs() {
  int all_pass = 1;
  printf("\n[1.1] echo hello too | xargs echo bye\n");
  {
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
      close(0); dup(p[0]);
      close(p[0]); close(p[1]);
      char *args[] = {"xargs", "echo", "bye", 0};
      exec("xargs", args);
      exit(1);
    }
    close(p[0]);
    write(p[1], "hello too\n", 10);
    close(p[1]);
    int st; wait(&st);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  printf("\n[1.2] echo README | xargs cat\n");
  {
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
      close(0); dup(p[0]);
      close(p[0]); close(p[1]);
      char *args[] = {"xargs", "cat", 0};
      exec("xargs", args);
      exit(1);
    }
    close(p[0]);
    write(p[1], "README\n", 7);
    close(p[1]);
    int st; wait(&st);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  return all_pass;
}

int test_tree() {
  int all_pass = 1;
  mkdir("a");
  mkfile("a/file1.txt");
  mkfile("a/file2.txt");
  mkdir("a/sub");
  mkfile("a/sub/file3.txt");

  printf("\n[2.1] tree .\n");
  {
    char *args[] = {"tree", ".", 0};
    int st = run(args);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  printf("\n[2.2] tree a\n");
  {
    char *args[] = {"tree", "a", 0};
    int st = run(args);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  return all_pass;
}

int test_trace() {
  int all_pass = 1;
  printf("\n[3.1] trace 32 grep hello README\n");
  {
    char *args[] = {"trace", "32", "grep", "hello", "README", 0};
    int st = run(args);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  printf("\n[3.2] trace 2147483647 grep hello README\n");
  {
    char *args[] = {"trace", "2147483647", "grep", "hello", "README", 0};
    int st = run(args);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  return all_pass;
}

int test_procinfo() {
  int all_pass = 1;

  printf("\n[4.1] procinfo 1  (init process)\n");
  {
    char *args[] = {"procinfo", "1", 0};
    int st = run(args);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  printf("\n[4.2] procinfo <current pid>\n");
  {
    int pid = fork();
    if (pid == 0) {
      int mypid = getpid();
      char buf[16];
      int i = 0, tmp = mypid;
      if (tmp == 0) { buf[i++] = '0'; }
      else { int rev = 0, digits = 0;
             while(tmp){ rev = rev*10 + tmp%10; tmp/=10; digits++; }
             for(int d=0;d<digits;d++){ buf[i++]='0'+rev%10; rev/=10; } }
      buf[i] = 0;
      char *args[] = {"procinfo", buf, 0};
      exec("procinfo", args);
      exit(1);
    }
    int st; wait(&st);
    if (st == 0) printf("  => PASS\n");
    else       { printf("  => FAIL\n"); all_pass = 0; }
  }

  return all_pass;
}

int main(int argc, char *argv[]) {
  int pass = 1;
  printf("================================================================================\n");
  printf("                              [1] XARGS TEST\n");
  printf("================================================================================\n");
  pass &= test_xargs();

  printf("================================================================================\n");
  printf("                              [2] TREE TEST\n");
  printf("================================================================================\n");
  pass &= test_tree();

  printf("================================================================================\n");
  printf("                              [3] TRACE TEST\n");
  printf("================================================================================\n");
  pass &= test_trace();

  printf("================================================================================\n");
  printf("                              [4] PROCINFO TEST \n");
  printf("================================================================================\n");
  pass &= test_procinfo();

  if (pass) printf("  ALL TESTS PASSED\n");
  else      printf("  SOME TESTS FAILED\n");
  exit(0);
}