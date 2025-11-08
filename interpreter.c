#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024
#define STACK_SIZE 32

typedef struct {
    int arr[32];  
    int matches[32];  
    int top;        
} Stack;

char *funcs[256];
char memory[MEMORY_SIZE];
int memory_ptr = 0;
int defining_func = 0;

Stack* call_stack;
Stack* func_def_stack;

int peek (char req){
    for(int i=memory_ptr;i<MEMORY_SIZE;i++){
        if(memory[i] == req){
            return i;
        }
    }
    return memory_ptr;
}


int read_file(char* path, char* res){
    FILE* fd = fopen(path, "r+");

    if (!fd) {
        perror("Opening file failed");
        return 0;
    }    

    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    fread(res, 1, fsize, fd);
    fclose(fd);
    return fsize;
}

int execute_library_function (int id){
    switch (id)
    {
        case 0:
        // add 16 to current memory adress. This is useful to define functions
        memory[memory_ptr] += 16;
        break;
        case 1:
        //Print out entire memory starting from the pointer
        puts(&memory[memory_ptr+1]);
        break;
        case 2:
        //Read a file
        char* path = &memory[memory_ptr+1];
        read_file(path, &(memory[memory_ptr+1]));        
        break;
    default:
        break;
    }

    return 0;
}


int interpret (char* code, long fsize){
    for(int i=0;i<fsize;i++){
        char com = code[i];
        
        if (com == '}' || defining_func) 
        {
            if(com != '}') continue;
        
            defining_func = 0;
            int start_pos = func_def_stack->arr[func_def_stack->top--];
            int subsegment_size = i-start_pos;
            char* subsegment = malloc(subsegment_size);

            memcpy(subsegment, &(code[start_pos+1]), subsegment_size-1);
            
            subsegment[subsegment_size-1] = '\0';
            
            funcs[memory[memory_ptr]] = subsegment;
        }
        else if (com == '>') memory_ptr++;
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
            int start_pos = call_stack->arr[call_stack->top];
                        
            if(memory[memory_ptr]){
                i = start_pos;
            }else{
                call_stack->top--;
            }
        }
        else if (com == '#') {
            if(memory[memory_ptr] < 16){
                execute_library_function(memory[memory_ptr]);
            }else{
                interpret(funcs[memory[memory_ptr]], strlen(funcs[memory[memory_ptr]]));
            }
        }
        else if (com == '{') {
            func_def_stack->arr[++func_def_stack->top] = i;
            defining_func = 1;
        }
        
        if (memory_ptr < 0) memory_ptr = 0;
        if (memory_ptr >= MEMORY_SIZE) memory_ptr = MEMORY_SIZE - 1;
        
    }
    return 1;
}



int main (int argc, char** argv){
    char code[30000];

    int fsize = read_file("./test.brainfuck", code);
    code[fsize] = '\n';
    code[fsize+1] = '\0';

    call_stack = malloc(sizeof(Stack));
    func_def_stack = malloc(sizeof(Stack));

    call_stack->top = -1;
    func_def_stack->top = -1;

    interpret(code, fsize);
    puts("\n");

}
