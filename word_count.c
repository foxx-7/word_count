#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CMD_COUNT 4

typedef struct
{
  const char *cmd;
  void (*count_fn) (FILE *fp);
}exec_context;

void close_file(FILE *fp);
void count_bytes(FILE *fp);
void count_words(FILE *fp);
void count_lines(FILE *fp);
void count_all(FILE *fp);
FILE* open_file(const char *file_name);
void exec(FILE *fp, exec_context *exec_tab, const char *switch_arg);
void print_help();

//MAIN
int main(int argc, char* argv[])
{
  if(argc==2 && (strcmp(argv[1], "--help")==0)){
    print_help();
    exit(EXIT_SUCCESS);
  }
  else if(argc != 3){
    printf("error!\nuse '--help' for help on program usage\n");
    exit(EXIT_FAILURE);
  }
  const char *file_path = argv[1];
  const char *u_switch = argv[2];
  FILE *fp = open_file(file_path);
  exec_context exec_tab[] = {
    {"-b", count_bytes},
    {"-w", count_words},
    {"-l", count_lines},
    {"-a", count_all}
  };
  exec(fp, exec_tab, u_switch);
  close_file(fp);
  return 0;
}

void count_all(FILE *fp){
  size_t words = 0;
  size_t lines = 0;
  size_t bytes = 0;
  size_t in_word = 0;
  int c = 0;
  while((c=fgetc(fp))!= EOF){
    bytes++;
    if(c == '\n')
    {
      lines++;
    }
    if(!isspace(c) && in_word == 0)
    {
      in_word = 1;
      words++;
    }
    else if(isspace(c))
    {
      in_word = 0;
    }
  }
    //increment line counter if file does not end with a new line.
  fseek(fp,-1,SEEK_END);
  if((c=fgetc(fp))!='\n'){
    lines++;
  }
  rewind(fp);
  printf("bytes: %d\nwords: %d\nlines: %d\n", bytes, words, lines);
}

void count_bytes(FILE *fp)
{
  size_t bytes = 0;
  int c = 0;
  while((c = fgetc(fp)) !=EOF)
  {
    bytes++;
  }
  rewind(fp);
  printf("%d\n", bytes);
}

void count_words(FILE *fp)
{
  size_t words = 0;
  int c = 0;
  int in_word = 0;
  while((c = fgetc(fp)) != EOF)
  {
    if(!isspace(c) && in_word == 0)
    {
      in_word = 1;
      words++;
    }
    else if(isspace(c))
    {
      in_word = 0;
    }
  }
  rewind(fp);
  printf("%d\n", words);
}

void count_lines(FILE *fp)
{
  size_t lines = 0;
  int c = 0;
  while((c = fgetc(fp)) != EOF)
  {
    if(c == '\n')
    {
      lines++;
    }
  }
  //increment line counter if file does not end with a new line.
  fseek(fp, -1, SEEK_END);
  if((c = fgetc(fp)) != '\n')
  {
    lines++;
  }
  rewind(fp);
  printf("%d\n", lines);
}

void close_file(FILE *fp){
  //should check if file is open before closing....
  fclose(fp);
}

FILE* open_file(const char *file_path)
{
  FILE *fp = fopen(file_path, "rb");
  if(!fp){
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return fp;
}

void exec(FILE *fp, exec_context *exec_tab, const char *u_switch)
{
  for(size_t i=0;i<CMD_COUNT;i++){
    if(strcmp(exec_tab[i].cmd, u_switch) == 0){
    return exec_tab[i].count_fn(fp);
    }
  }
  fprintf(stderr, "invalid command flag: %s\n", u_switch);
  fprintf(stderr, "use '--help' for help on program usage\n");
  exit(EXIT_FAILURE);
}

void print_help(){
  printf("cw - count word\n"
  "this tool reports properties of a text file using format 'cw <file path> <switch> using one of the following flags as switch: \n"
  "-b : count bytes/characters\n"
  "-l : count lines\n"
  "-w : count words\n"
  "-a : count all(byte, words, lines) respectively\n"
  "please report issues at kevincoretto@gmail.com \n");
}