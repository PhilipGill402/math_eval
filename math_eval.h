#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DICT_SIZE 100

#define eval_create_var(context, name, value) _Generic((value), \
        int : eval_create_int_var,       \
        double : eval_create_double_var,    \
        float:  eval_create_double_var,  \
        long:   eval_create_int_var,     \
        short:  eval_create_int_var,     \
        default: eval_create_double_var  \
        )((context), (name), (value)) 

#define eval_set_var(context, name, value) _Generic((value), \
        int : eval_set_int_var,       \
        double : eval_set_double_var,    \
        float:  eval_set_double_var,  \
        long:   eval_set_int_var,     \
        short:  eval_set_int_var,     \
        default: eval_set_double_var  \
        )((context), (name), (value)) 



typedef enum TokenType {
    INTEGER,
    FLOAT,
    FUNCTION,
    VARIABLE,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    LPAREN,
    RPAREN,
    NEG,
    UNKNOWN,
    NONE
} TokenType;

typedef enum ErrorCode {
    UnknownOperator,
    UnknownToken,
    TooManyVariables,
    NoError
} ErrorCode;

typedef struct token_t {
    TokenType type;
    char* val;
    int needs_to_be_freed;
} token_t;

typedef struct token_stack_t {
    token_t* tokens;
    int capacity;
    int length;
} token_stack_t;

typedef struct equation_t {
    token_stack_t prefix;
} equation_t;

typedef struct int_hash_map_t {
    int size;
    char keys[MAX_DICT_SIZE][100];
    int values[MAX_DICT_SIZE];
} int_hash_map_t;

typedef struct double_hash_map_t {
    int size;
    char keys[MAX_DICT_SIZE][100];
    double values[MAX_DICT_SIZE];
} double_hash_map_t;

typedef struct eval_ctx {
    int_hash_map_t int_vars;
    double_hash_map_t double_vars;
    equation_t equation;
    ErrorCode error;
} eval_ctx;

typedef struct lexer_t {
    eval_ctx* context;
    const char* expr;
    int pos;
    char current_char;
} lexer_t;

//lexer functions
void print_token(const token_t* token);
lexer_t create_lexer(eval_ctx* context, const char* expr);
void advance_lexer(lexer_t* lexer);
char peek(lexer_t* lexer);
int is_defined_function(const char* str);
int is_defined_variable(lexer_t* lexer, const char* str);
token_t look_for_function_or_var(eval_ctx* context, lexer_t* lexer);
token_t get_next_token(eval_ctx* context, lexer_t* lexer);
token_t token_clone(token_t* token);
void token_free(token_t* token);

//stack functions
token_stack_t create_stack();
void push_back(token_stack_t* stack, token_t* token);
token_t pop(token_stack_t* stack);
token_t top(token_stack_t* stack);
token_stack_t reverse_stack(token_stack_t* stack);
void print_stack(token_stack_t* stack);
void stack_free(token_stack_t* stack);

//parser functions
int operator_precedence(token_t token);
int is_operator(token_t token);
int is_right_associative(token_t token);
token_t* infix_to_prefix(token_stack_t* stack, int num_tokens);
token_stack_t parse(eval_ctx* context, const char* expr);
token_t handle_integer(eval_ctx* context, token_t current_token, token_t operand1, token_t operand2);
token_t handle_double(eval_ctx* context, token_t current_token, token_t operand1, token_t operand2);
token_t evaluate(eval_ctx* context, token_stack_t* prefix);

//my hashmap functions
//these run in O(n) time and are not real hashmaps, but considering the number of entries is low, this should be sufficient
int_hash_map_t create_int_hash_map();
double_hash_map_t create_double_hash_map();
int int_get_index(int_hash_map_t* map, const char key[]);
int double_get_index(double_hash_map_t* map, const char key[]);
void int_set(int_hash_map_t* map, const char key[], int new_val);
void double_set(double_hash_map_t* map, const char key[], double new_val);
void int_insert(eval_ctx* context, const char key[], int value);
void double_insert(eval_ctx* context, const char key[], double value);
int int_get(int_hash_map_t* map, const char key[]);
double double_get(double_hash_map_t* map, const char key[]);

//my math functions
int my_pow(int x, int y);

//available to use
eval_ctx eval_create_context();
void eval_create_int_var(eval_ctx* context, const char* name, int val);
void eval_create_double_var(eval_ctx* context, const char* name, double val);
void eval_set_var_int(eval_ctx* context, const char* name, int val);
void set_var_double(eval_ctx* context, const char* name, double val);
void eval_create_equation(eval_ctx* context, const char* equation);
double eval_solve(eval_ctx* context);
void eval_free(eval_ctx* context); 


