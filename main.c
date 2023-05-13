#include <complex.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>


#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)NULL)->Attribute)


typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;


typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;


typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;


typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNCRECOGNIZED_STATEMENT
} PrepareResult;


typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;


typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;


const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;



MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}


// prepares the statement based on the user input
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement.row_to_insert.email);

        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNCRECOGNIZED_STATEMENT;
}


void execute_statement(Statement* statement) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            puts("This is where we would do an insert.");
            break;
        case (STATEMENT_SELECT):
            puts("This is where we would do a select.");
            break;
    }
}


void serialize_row(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}


void deserialization_row(void* source, Row* destination) {
   memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE); 
   memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE); 
   memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE); 
}


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
    Statement statement;
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

