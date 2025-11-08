#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024
#define STACK_SIZE 32

typedef struct {
    int arr[32];  
    int top;        
} Stack;

char memory[MEMORY_SIZE];
int memory_ptr = 0;
Stack* call_stack;

int interpret (char* code, long fsize){
    for(int i=0;i<fsize;i++){
        char com = code[i];
        
        if      (com == '>') memory_ptr++;
        else if (com == '<') memory_ptr--;
        else if (com == '+') memory[memory_ptr]++;
        else if (com == '-') memory[memory_ptr]--;
        else if (com == ',') memory[memory_ptr] = getchar();
        else if (com == '.') {
            putchar(memory[memory_ptr]);
        }
        else if (com == '[') {
            call_stack->arr[++call_stack->top] = i;
        }
        else if (com == ']') {
            int start_pos = call_stack->arr[call_stack->top--];
            int subsegment_size = i-start_pos;
            char* subsegment = malloc(subsegment_size);

            memcpy(subsegment, &(code[start_pos+1]), subsegment_size);
            
            subsegment[subsegment_size-1] = '\0';
            
            while(memory[memory_ptr]){
                interpret(subsegment, subsegment_size);
            }
        }
        
        if (memory_ptr < 0) memory_ptr = 0;
        if (memory_ptr >= MEMORY_SIZE) memory_ptr = MEMORY_SIZE - 1;
        
    }
    return 1;
}


int main (int argc, char** argv){
    FILE* fd = fopen("./test.brainfuck", "r+");
    char code[30000];

    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    fread(code, 1, fsize, fd);
    code[fsize] = '\n';
    code[fsize+1] = '\0';

    call_stack = malloc(sizeof(Stack));
    interpret(code, fsize);
    puts("\n");

    fclose(fd);
}
