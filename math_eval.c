#include "math_eval.h"

/* ***MY MATH FUNCTIONS*** */
int my_pow(int x, int y) {
    int result = x; 
    for (int i = 1; i < y; i++) {
        result *= x;
    }

    return result;
}

/* ***LEXER FUNCTIONS*** */
void print_token(const token_t* token) {
    printf("%s", token->val);
}

lexer_t create_lexer(eval_ctx* context, const char* expr) {
    lexer_t lexer = { context, expr, 0, expr[0] };

    return lexer;
}

void advance_lexer(lexer_t* lexer) {
    lexer->pos++;
    if (lexer->pos >= strlen(lexer->expr)) {
        lexer->current_char = '\0';
    } else {
        lexer->current_char = lexer->expr[lexer->pos];
    }
}

char peek(lexer_t* lexer) {
    return lexer->current_char;
}

int is_defined_function(const char* str) {
    if (strcmp(str, "sin") == 0) return 1;
    if (strcmp(str, "cos") == 0) return 1;
    if (strcmp(str, "tan") == 0) return 1;
    if (strcmp(str, "sqrt") == 0) return 1;
    else return 0;
}

int is_defined_variable(lexer_t* lexer, const char* str) {
    if (int_get_index(&lexer->context->int_vars, str) != -1) {
        return 1;
    } else if (double_get_index(&lexer->context->double_vars, str) != -1) {
        return 1;
    }

    return 0;
}

token_t look_for_function_or_var(eval_ctx* context, lexer_t* lexer) {
    int size = 10; 
    char* val = malloc(sizeof(char) * size);
    int str_pos = 0;
    
    while (isalpha(lexer->current_char)) {
        if (str_pos >= size - 1) {
            val = realloc(val, sizeof(char) * size * 2);
            size *= 2;
        }

        val[str_pos] = lexer->current_char;
        str_pos++;
        advance_lexer(lexer);
    }

    if (str_pos >= size - 1) {
        val = realloc(val, sizeof(char) * size * 2);
        size *= 2;
    }

    val[str_pos] = '\0';
     
    if (is_defined_function(val)) {
        return (token_t){ FUNCTION, val };
    } else if (is_defined_variable(lexer, val)) {
        return (token_t){ VARIABLE, val };
    } else {
        context->error = UnknownToken;
        printf("Could not match token %s to a function or variable\n", val);
        return (token_t){ UNKNOWN, val };
    }

}

token_t get_next_token(eval_ctx* context, lexer_t* lexer) {
    token_t token;

    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)){     
            advance_lexer(lexer); 
        } else if (isdigit(lexer->current_char)) {
            char c = lexer->current_char;

            //initialize val string
            int size = 10;
            char* val = malloc(sizeof(char) * size);
            val[0] = c;
            int len = 1;

            advance_lexer(lexer);
            while (isdigit(lexer->current_char)) {
                //resizing if needed 
                if (len >= size - 1) {
                    val = realloc(val, sizeof(char) * size * 2);
                    size *= 2;
                }

                val[len] = lexer->current_char;
                len++;
                advance_lexer(lexer);
            }
            
            if (lexer->current_char == '.') {
                //resizing if needed 
                if (len >= size - 1) {
                    val = realloc(val, sizeof(char) * size * 2);
                    size *= 2;
                }

                val[len] = '.';
                len++;

                advance_lexer(lexer);
                while (isdigit(lexer->current_char)) {
                    //resizing if needed 
                    if (len >= size - 1) {
                        val = realloc(val, size * 2);
                        size *= 2;
                    }

                    val[len] = lexer->current_char;
                    len++;
                    advance_lexer(lexer);
                }

                val[len] = '\0';
                token = (token_t){ FLOAT, val };
                return token;
            }

            val[len] = '\0';
            token = (token_t){ INTEGER, val };
            return token;
        } else if (lexer->current_char == '+') {
            token = (token_t){ ADD, "+" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == '-') {
            token = (token_t){ SUB, "-" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == '*') {
            token = (token_t){ MUL, "*" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == '/') {
            token = (token_t){ DIV, "/" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == '^') {
            token = (token_t){ POW, "^" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == '(') {
            token = (token_t){ LPAREN, "(" };
            advance_lexer(lexer);
            return token;
        } else if (lexer->current_char == ')') {
            token = (token_t){ RPAREN, ")" };
            advance_lexer(lexer);
            return token;
        } else {
            //normal char, look for built in functions or variables
            return look_for_function_or_var(context, lexer);
        }
    }

    token = (token_t){ NONE, "\0" };
    return token;
}

/* ***STACK FUNCTIONS*** */
token_stack_t create_stack() {
    token_stack_t stack;
    stack.tokens = malloc(sizeof(token_t) * 10);
    stack.capacity = 10;
    stack.length = 0;

    return stack;
}

void push_back(token_stack_t* stack, token_t token) {
    //resizing if needed
    if (stack->length >= stack->capacity) {
        stack->tokens = realloc(stack->tokens, sizeof(token_t) * stack->capacity*2);
        stack->capacity *= 2;
    }
    
    stack->tokens[stack->length] = token;
    stack->length++;
}
token_t pop(token_stack_t* stack) {
    //empty
    if (stack->length == 0) {
        return (token_t){ NONE, "\0" };
    } 

    token_t token = stack->tokens[stack->length - 1];
    stack->length--;
    
    //we are at a quater capacity so downsize
    if (stack->length < (int)(stack->capacity / 4)) {
        stack->tokens = realloc(stack->tokens, sizeof(token_t) * (int)(stack->capacity / 4));
        stack->capacity = (int)(stack->capacity / 4);
    }

    return token;
}

token_t top(token_stack_t* stack) {
    if (stack->length == 0) {
        return (token_t){ NONE, "\0" };
    }

    return stack->tokens[stack->length - 1];
}

token_stack_t reverse_stack(token_stack_t* stack){
    token_stack_t new_stack = create_stack(); 
    while (stack->length != 0) {
        push_back(&new_stack, pop(stack));
    }

    return new_stack;
}

void print_stack(token_stack_t* stack) {
    printf("[");
    for (int i = 0; i < stack->length - 1; i++) {
        print_token(&stack->tokens[i]); 
        printf(", ");
    }
    print_token(&stack->tokens[stack->length - 1]);
    printf("]");
}

/* ***PARSER FUNCTIONS*** */
int operator_precedence(token_t token) {
    if (token.type == FUNCTION) {
        return 5;
    } else if (token.type == POW) {
        return 4;
    } else if (token.type == NEG) {
        return 3;
    } else if (token.type == MUL || token.type == DIV) {
        return 2;
    } else if (token.type == ADD || token.type == SUB) {
        return 1;
    } else {
        return -1;
    }
}

int is_operator(token_t token) {
    if (token.type == ADD || token.type == SUB || token.type == MUL || token.type == DIV || token.type == POW || token.type == NEG || token.type == FUNCTION) {
        return 1;
    } else {
        return 0;
    }
}

int is_right_associative(token_t token) {
    if (token.type == POW || token.type == NEG) {
        return 1;
    } else {
        return 0;
    }
}

int is_unary_minus(token_stack_t* stack, int i) {
    //the minus is the first character 
    if (i == 0) {
        return 1;
    }

    token_t left = stack->tokens[i - 1];
    if (left.type == LPAREN || is_operator(left)) {
        return 1;
    }

    return 0;
}

token_t* infix_to_prefix(token_stack_t* stack, int num_tokens) {
    //guarantess no resizing as no new tokens will be added;
    token_t* arr = malloc(sizeof(token_t) * num_tokens);
    int arr_len = 0;
    token_stack_t operators = create_stack();
    int n = stack->length;
    
    for (int i = n - 1; i >= 0; i--) {
        token_t token = stack->tokens[i];
        
        if (token.type == INTEGER || token.type == FLOAT || token.type == VARIABLE) {
            arr[arr_len] = token;
            arr_len++;
        } else if (token.type == RPAREN) {
            push_back(&operators, token);
        } else if (token.type == LPAREN) {
            while (top(&operators).type != NONE && top(&operators).type != RPAREN) {
                arr[arr_len] = pop(&operators);
                arr_len++;
            }
            
            //pop RPAREN
            if (top(&operators).type != NONE) {
                pop(&operators); 
            }
            
            //checks if a function is to the left of the parenthesis
            if (i > 0 && stack->tokens[i - 1].type == FUNCTION) {
                //add the function and skip the next token 
                arr[arr_len] = stack->tokens[i - 1];
                arr_len++;
                i--;
            }

        } else if (is_operator(token)) {
            //checking for unary NEG 
            if (token.type == SUB && is_unary_minus(stack, i)) {
                token.type = NEG;
                token.val = "NEG";
            } 

            while (top(&operators).type != NONE && is_operator(top(&operators)) && (operator_precedence(top(&operators)) > operator_precedence(token) || (operator_precedence(top(&operators))) == operator_precedence(token) && is_right_associative(token))) {
                arr[arr_len] = pop(&operators);
                arr_len++;
            }

            push_back(&operators, token);
        }
    }
    
    while (operators.length > 0) {
        arr[arr_len] = pop(&operators);
        arr_len++;
    }

    return arr;
}

token_t make_int_token_from_int(int x) {
    int needed = snprintf(NULL, 0, "%d", x);
    
    char* s = malloc((size_t)needed + 1);
    snprintf(s, (size_t)needed + 1, "%d", x);
    
    return (token_t){ INTEGER, s };
}

token_t make_float_token_from_float(double x) {
    int needed = snprintf(NULL, 0, "%f", x);

    char* s = malloc((size_t)needed + 1);
    snprintf(s, (size_t)needed + 1, "%f", x);

    return (token_t){ FLOAT, s };
}

token_t handle_integer(eval_ctx* context, token_t current_token, token_t operand1, token_t operand2) {
    int val1 = atoi(operand1.val);
    int val2 = atoi(operand2.val);
    token_t new_operand;
    
    switch (current_token.type) {
        case ADD:
            new_operand = make_int_token_from_int(val1 + val2); 
            break;
        case SUB:
            new_operand = make_int_token_from_int(val1 - val2); 
            break;
        case MUL:
            new_operand = make_int_token_from_int(val1 * val2); 
            break;
        case POW:
            new_operand = make_int_token_from_int(my_pow(val1, val2)); 
            break;
        default:
            context->error = UnknownOperator;
            printf("Could not match token %s to an operator\n", current_token.val);
            new_operand = (token_t){ UNKNOWN, "UNKNOWN" };
            break;
    }
    
    return new_operand;
}

token_t handle_double(eval_ctx* context, token_t current_token, token_t operand1, token_t operand2) {
    double val1 = atof(operand1.val);
    double val2 = atof(operand2.val);
    token_t new_operand;

    switch (current_token.type) {
        case ADD:
            new_operand = make_float_token_from_float(val1 + val2); 
            break;
        case SUB:
            new_operand = make_float_token_from_float(val1 - val2); 
            break;
        case DIV:
            new_operand = make_float_token_from_float(val1 / val2); 
            break;
        case MUL:
            new_operand = make_float_token_from_float(val1 * val2); 
            break;
        case POW:
            new_operand = make_float_token_from_float(pow(val1, val2)); 
            break;
        default:
            context->error = UnknownOperator;
            printf("Could not match token %s to an operator\n", current_token.val);
            new_operand = (token_t){ UNKNOWN, "UNKNOWN" };
            break;
    }

    return new_operand;
}

token_t handle_negation(token_t operand) {
    token_t new_operand;

    if (operand.type == INTEGER) {
        int val = atoi(operand.val); 
        new_operand = make_int_token_from_int(-val);
    } else {
        double val = atof(operand.val); 
        new_operand = make_float_token_from_float(-val);
    }

    return new_operand;
}

token_t handle_function(eval_ctx* context, token_t func, token_t operand) {
    double val = atof(operand.val);
    double result;
    if (strcmp(func.val, "sin") == 0) {
        result = sin(val);
    } else if (strcmp(func.val, "cos") == 0) {
        result = cos(val);
    } else if (strcmp(func.val, "tan") == 0) {
        result = tan(val);
    } else if (strcmp(func.val, "sqrt") == 0) {
        result = sqrt(val);
    } else {
        context->error = UnknownToken;
        printf("Could not find function of name: %s\n", func.val);
        return (token_t){ UNKNOWN, "UNKNOWN FUNCTION" };
    }

    return make_float_token_from_float(result);
}

token_t evaluate(eval_ctx* context, token_stack_t* prefix) {
    token_stack_t operands = create_stack();

    for (int i = prefix->length - 1; i >= 0; i--) {
        token_t current_token = prefix->tokens[i];
        if (is_operator(current_token)) {
            if (current_token.type == NEG) {
                token_t operand = pop(&operands);
                token_t new_operand = handle_negation(operand);
                push_back(&operands, new_operand);
            } else if (current_token.type == FUNCTION) {
                token_t operand = pop(&operands);
                token_t new_operand = handle_function(context, current_token, operand);
                push_back(&operands, new_operand);
            } else {
                token_t operand1 = pop(&operands);
                token_t operand2 = pop(&operands);

                if (operand1.type == INTEGER && operand2.type == INTEGER && current_token.type != DIV) {
                    token_t result = handle_integer(context, current_token, operand1, operand2);
                    push_back(&operands, result);
                } else {
                    token_t result = handle_double(context, current_token, operand1, operand2);
                    push_back(&operands, result);
                }
            }
        } else if (current_token.type == VARIABLE) {
            token_t operand;
            if (int_get_index(&context->int_vars, current_token.val) != -1) {
                 
                int val = int_get(&context->int_vars, current_token.val);
                operand = make_int_token_from_int(val);
            } else if (double_get_index(&context->double_vars, current_token.val) != -1) {
                double val = int_get(&context->int_vars, current_token.val);
                operand = make_float_token_from_float(val);
            } else {
                context->error = UnknownToken;
                printf("Could not find variable of name: %s\n", current_token.val);
                operand = (token_t){ UNKNOWN, "UNKNOWN VARIABLE" };
            }
            push_back(&operands, operand); 
        } else {
            push_back(&operands, current_token);
        }
    }
    token_t result = pop(&operands);
    return result;
}

token_stack_t parse(eval_ctx* context, const char* expr) {
    lexer_t lexer = create_lexer(context, expr);
     
    token_t token;
    token_stack_t token_stack = create_stack();
    int num_tokens = 0;

    do {
        token = get_next_token(context, &lexer);
        
        if (token.type == NONE) {
            break;
        } else if (token.type != LPAREN && token.type != RPAREN) {
            num_tokens++;
        }

        push_back(&token_stack, token);
         
    } while (token.type != NONE);
    
    token_t* prefix_arr = infix_to_prefix(&token_stack, num_tokens);
    token_stack_t prefix = create_stack();

    for (int i = num_tokens - 1; i >= 0; i--) {
        push_back(&prefix, prefix_arr[i]); 
    }

    return prefix;
}

/* ***HASHMAP FUNCTIONS *** */
int_hash_map_t create_int_hash_map() {
    int_hash_map_t map;
    map.size = 0;

    return map;
}

double_hash_map_t create_double_hash_map() {
    double_hash_map_t map;
    map.size = 0;

    return map;
}

int int_get_index(int_hash_map_t* map, const char key[]) {
    for (int i = 0; i < MAX_DICT_SIZE; i++) {
        if (strcmp(map->keys[i], key) == 0) {
            return i;
        }
    }

    return -1;
}

int double_get_index(double_hash_map_t* map, const char key[]) {
    for (int i = 0; i < MAX_DICT_SIZE; i++) {
        if (strcmp(map->keys[i], key) == 0) {
            return i;
        }
    }

    return -1;
}

void int_set(int_hash_map_t* map, const char key[], int new_val) {
    int idx = int_get_index(map, key);

    if (idx == -1) {
        return;
    }

    map->values[idx] = new_val;
}

void double_set(double_hash_map_t* map, const char key[], double new_val) {
    int idx = double_get_index(map, key);

    if (idx == -1) {
        return;
    }

    map->values[idx] = new_val;
}

void int_insert(eval_ctx* context, const char key[], int value) {
    int_hash_map_t* map = &context->int_vars; 

    if (map->size > MAX_DICT_SIZE) {
        context->error = TooManyVariables;
        printf("Could not insert another variable, max number of variables is %d", MAX_DICT_SIZE);
        return;
    } 

    strncpy(map->keys[map->size], key, 100); 
    map->values[map->size] = value;
    map->size++;
}

void double_insert(eval_ctx* context, const char key[], double value) {
    double_hash_map_t* map = &context->double_vars; 

    if (map->size > MAX_DICT_SIZE) {
        context->error = TooManyVariables;
        printf("Could not insert another variable, max number of variables is %d", MAX_DICT_SIZE);
        return;
    }

    strncpy(map->keys[map->size], key, 100);
    map->values[map->size] = value;
    map->size++;
}

int int_get(int_hash_map_t* map, const char key[]) {
    int idx = int_get_index(map, key);
    
    if (idx == -1) {
        return -1;
    }

    return map->values[idx];
}

double double_get(double_hash_map_t* map, const char key[]) {
    int idx = double_get_index(map, key);
    
    if (idx == -1) {
        return -1;
    }

    return map->values[idx];
}


/* ***AVAILIABLE FUNCTIONS*** */
eval_ctx eval_create_context() {
    eval_ctx context;
    context.int_vars = create_int_hash_map();
    context.double_vars = create_double_hash_map();
    context.equation = (equation_t){ create_stack() };
    context.error = NoError;

    return context;
}

void eval_create_int_var(eval_ctx* context, const char* name, int val) {
    int_insert(context, name, val);  
}

void eval_create_double_var(eval_ctx* context, const char* name, double val) {
    double_insert(context, name, val); 
}

void eval_set_var_int(eval_ctx* context, const char* name, int val) {
    int_set(&context->int_vars, name, val); 
}
void set_var_double(eval_ctx* context, const char* name, double val) {
    double_set(&context->double_vars, name, val);
}

void eval_create_equation(eval_ctx* context, const char* equation) {
    token_stack_t prefix = parse(context, equation);
    equation_t new_equation = (equation_t) { prefix };
    context->equation = new_equation; 
}

double eval_solve(eval_ctx* context) {
    return atof(evaluate(context, &context->equation.prefix).val);
}

int main() {
    eval_ctx ctx = eval_create_context();

    eval_create_var(&ctx, "x", 0.5f);
    eval_create_var(&ctx, "y", 2.5f);
    eval_create_equation(&ctx, "sin(x)^y");
    
    double result = eval_solve(&ctx);

    printf("Result: %f\n", result);
    return 0;  
}
