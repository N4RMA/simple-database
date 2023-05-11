#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;


// Dynamically allocates memory for a new InputBuffer struct and initializes its fields to default values
InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}


// reads stdin and stores it in the InputBuffer struct
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = 
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    //ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}


// displays the db prompt
void print_prompt() { printf("db > "); }


// prototype for getline function
ssize_t getline(char **lineptr, size_t *n, FILE *stream);


// frees the allocated memory for the InputBuffer struct
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}


// main function
int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {
        switch (do_meta_command(input_buffer)) {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND): 
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
        }
    }
    switch (prepare_statement(input_buffer, &statement)) {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNCRECOGNIZED_STATEMENT):
            printf("Unrecognized keywork at start of '%s'.\n", input_buffer->buffer);
            continue;
    }

    execute_statement(&statement);
    printf("Executed.\n");
  }
}

