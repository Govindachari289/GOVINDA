
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "interpreter.h"

#define MAX_VARIABLES 100
#define MAX_FUNCTIONS 100
#define MAX_ARRAY_SIZE 100
#define MAX_CALL_DEPTH 100
#define MAX_LOOP_COUNT 1000000

typedef struct {
    ASTNodeType type;
    double number;
    char text[100];
} Value;

typedef struct {
    char name[100];
    Value value;
    Value items[MAX_ARRAY_SIZE];
    int array_size;
} Variable;

typedef struct {
    char name[100];
    ASTNode *parameters;
    ASTNode *body;
} Function;

static Variable variables[MAX_VARIABLES];
static int variable_count = 0;

static Function functions[MAX_FUNCTIONS];
static int function_count = 0;

static int call_depth = 0;
static int runtime_error = 0;

#define REPORT_ERROR(...) do { \
    runtime_error = 1; \
    fprintf(stderr, __VA_ARGS__); \
} while (0)

static Variable *find_variable(const char *name)
{
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0)
            return &variables[i];
    }
    return NULL;
}

static Function *find_function(const char *name)
{
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0)
            return &functions[i];
    }
    return NULL;
}

static Value make_number(double number)
{
    Value value = {0};
    value.type = AST_NUMBER;
    value.number = number;
    return value;
}

static Value make_boolean(int boolean)
{
    Value value = {0};
    value.type = AST_BOOLEAN;
    value.number = boolean != 0;
    return value;
}

static Value make_string(const char *string)
{
    Value value = {0};
    value.type = AST_STRING;
    snprintf(value.text, sizeof(value.text), "%s", string);
    return value;
}

static int truthy(Value value)
{
    if (value.type == AST_STRING)
        return value.text[0] != '\0';

    return value.number != 0;
}

static int evaluate(ASTNode *node, Value *result);
static int execute_block(ASTNode *program, Value *return_value);

static int call_function(
    const char *name,
    ASTNode *arguments,
    Value *result
);

static void register_functions(ASTNode *program)
{
    for (ASTNode *node = program; node != NULL; node = node->next) {
        if (node->type == AST_FUNCTION) {
            Function *function = find_function(node->text);

            if (function == NULL) {
                if (function_count >= MAX_FUNCTIONS) {
                    REPORT_ERROR("Error: Too many functions!\n");
                    return;
                }

                function = &functions[function_count++];
            }

            snprintf(function->name, sizeof(function->name),
                     "%s", node->text);

            function->parameters = node->left;
            function->body = node->right;
        }

        if (node->type == AST_IF ||
            node->type == AST_REPEAT ||
            node->type == AST_WHILE) {
            register_functions(node->right);
            register_functions(node->else_branch);
        }

        if (runtime_error)
            return;
    }
}

static int evaluate_builtin(ASTNode *node, Value *result)
{
    Value argument;

    if (!evaluate(node->left, &argument))
        return 0;

    if (strcmp(node->text, "LENGTH") == 0 ||
        strcmp(node->text, "LEN") == 0) {
        if (argument.type != AST_STRING) {
            REPORT_ERROR("Error: LENGTH expects a string!\n");
            return 0;
        }

        *result = make_number(strlen(argument.text));
        return 1;
    }

    if (strcmp(node->text, "NUMBER") == 0) {
        if (argument.type == AST_NUMBER ||
            argument.type == AST_BOOLEAN) {
            *result = make_number(argument.number);
            return 1;
        }

        if (argument.type == AST_STRING) {
            char *end;
            double number = strtod(argument.text, &end);

            if (argument.text[0] != '\0' && *end == '\0') {
                *result = make_number(number);
                return 1;
            }
        }

        REPORT_ERROR("Error: Cannot convert value to NUMBER!\n");
        return 0;
    }

    if (strcmp(node->text, "STRING") == 0) {
        if (argument.type == AST_STRING) {
            *result = argument;
        } else if (argument.type == AST_BOOLEAN) {
            *result = make_string(
                argument.number ? "TRUE" : "FALSE"
            );
        } else {
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "%g", argument.number);
            *result = make_string(buffer);
        }

        return 1;
    }

    if (argument.type != AST_NUMBER) {
        REPORT_ERROR("Error: Built-in function '%s' expects a number!\n",
                     node->text);
        return 0;
    }

    if (strcmp(node->text, "ABS") == 0) {
        *result = make_number(fabs(argument.number));
        return 1;
    }

    if (strcmp(node->text, "ROUND") == 0) {
        *result = make_number(round(argument.number));
        return 1;
    }

    if (strcmp(node->text, "FLOOR") == 0) {
        *result = make_number(floor(argument.number));
        return 1;
    }

    if (strcmp(node->text, "CEIL") == 0) {
        *result = make_number(ceil(argument.number));
        return 1;
    }

    REPORT_ERROR("Error: Unknown built-in function '%s'!\n", node->text);
    return 0;
}

static int evaluate(ASTNode *node, Value *result)
{
    if (node == NULL || runtime_error)
        return 0;

    if (node->type == AST_NUMBER) {
        *result = make_number(node->number);
        return 1;
    }

    if (node->type == AST_BOOLEAN) {
        *result = make_boolean((int)node->number);
        return 1;
    }

    if (node->type == AST_STRING) {
        *result = make_string(node->text);
        return 1;
    }

    if (node->type == AST_VARIABLE) {
        Variable *var = find_variable(node->text);

        if (var == NULL) {
            REPORT_ERROR("Error: Undefined variable '%s'!\n", node->text);
            return 0;
        }

        *result = var->value;
        return 1;
    }

    if (node->type == AST_INDEX) {
        Variable *var = find_variable(node->text);
        Value index;

        if (var == NULL) {
            REPORT_ERROR("Error: Undefined array '%s'!\n", node->text);
            return 0;
        }

        if (!evaluate(node->left, &index))
            return 0;

        if (var->value.type != AST_ARRAY ||
            index.type != AST_NUMBER ||
            index.number < 0 ||
            index.number >= var->array_size ||
            floor(index.number) != index.number) {
            REPORT_ERROR("Error: Invalid array index!\n");
            return 0;
        }

        *result = var->items[(int)index.number];
        return 1;
    }

    if (node->type == AST_CALL)
        return call_function(node->text, node->left, result);

    if (node->type == AST_BUILTIN)
        return evaluate_builtin(node, result);

    if (node->type == AST_UNARY) {
        Value operand;

        if (!evaluate(node->left, &operand))
            return 0;

        if (node->operator == '!') {
            *result = make_boolean(!truthy(operand));
            return 1;
        }

        if (node->operator == '-') {
            if (operand.type != AST_NUMBER) {
                REPORT_ERROR("Error: Unary minus expects a number!\n");
                return 0;
            }

            *result = make_number(-operand.number);
            return 1;
        }

        return 0;
    }

    if (node->type == AST_BINARY) {
        Value left, right;

        if (!evaluate(node->left, &left) ||
            !evaluate(node->right, &right))
            return 0;

        if (node->operator == '+' &&
            left.type == AST_STRING &&
            right.type == AST_STRING) {
            char combined[100];

            size_t left_len = strlen(left.text);
            size_t right_len = strlen(right.text);

            if (left_len + right_len >= sizeof(combined)) {
                REPORT_ERROR("Error: Combined string is too long!\n");
                return 0;
            }

            memcpy(combined, left.text, left_len);
            memcpy(combined + left_len, right.text, right_len + 1);

            *result = make_string(combined);
            return 1;
        }

        if (left.type != AST_NUMBER || right.type != AST_NUMBER) {
            REPORT_ERROR("Error: Arithmetic requires numeric values!\n");
            return 0;
        }

        switch (node->operator) {
            case '+':
                *result = make_number(left.number + right.number);
                return 1;

            case '-':
                *result = make_number(left.number - right.number);
                return 1;

            case '*':
                *result = make_number(left.number * right.number);
                return 1;

            case '/':
                if (right.number == 0) {
                    REPORT_ERROR("Error: Division by zero!\n");
                    return 0;
                }

                *result = make_number(left.number / right.number);
                return 1;

            case '%':
                if (right.number == 0) {
                    REPORT_ERROR("Error: Modulo by zero!\n");
                    return 0;
                }

                *result = make_number(fmod(left.number, right.number));
                return 1;
        }
    }

    if (node->type == AST_COMPARE) {
        Value left, right;

        if (!evaluate(node->left, &left) ||
            !evaluate(node->right, &right))
            return 0;

        if (left.type == AST_STRING && right.type == AST_STRING) {
            int comparison = strcmp(left.text, right.text);

            switch (node->operator) {
                case '=':
                    *result = make_boolean(comparison == 0);
                    return 1;
                case '!':
                    *result = make_boolean(comparison != 0);
                    return 1;
                case '>':
                    *result = make_boolean(comparison > 0);
                    return 1;
                case '<':
                    *result = make_boolean(comparison < 0);
                    return 1;
                case 'G':
                    *result = make_boolean(comparison >= 0);
                    return 1;
                case 'L':
                    *result = make_boolean(comparison <= 0);
                    return 1;
            }
        }

        if ((left.type != AST_NUMBER && left.type != AST_BOOLEAN) ||
            (right.type != AST_NUMBER && right.type != AST_BOOLEAN)) {
            REPORT_ERROR("Error: Cannot compare these data types!\n");
            return 0;
        }

        switch (node->operator) {
            case '>':
                *result = make_boolean(left.number > right.number);
                return 1;
            case '<':
                *result = make_boolean(left.number < right.number);
                return 1;
            case 'G':
                *result = make_boolean(left.number >= right.number);
                return 1;
            case 'L':
                *result = make_boolean(left.number <= right.number);
                return 1;
            case '=':
                *result = make_boolean(left.number == right.number);
                return 1;
            case '!':
                *result = make_boolean(left.number != right.number);
                return 1;
        }
    }

    if (node->type == AST_LOGICAL) {
        Value left;

        if (!evaluate(node->left, &left))
            return 0;

        if (node->operator == '&' && !truthy(left)) {
            *result = make_boolean(0);
            return 1;
        }

        if (node->operator == '|' && truthy(left)) {
            *result = make_boolean(1);
            return 1;
        }

        Value right;

        if (!evaluate(node->right, &right))
            return 0;

        *result = make_boolean(truthy(right));
        return 1;
    }

    return 0;
}

static int evaluate_condition(ASTNode *node, int *result)
{
    Value value;

    if (!evaluate(node, &value)) {
        if (!runtime_error)
            REPORT_ERROR("Error: Invalid condition!\n");
        return 0;
    }

    *result = truthy(value);
    return 1;
}

/* FIXED CREATE: Evaluate first, then replace the variable. */

static int create_variable(ASTNode *node)
{
    if (!node->left || !node->right ||
        node->left->type != AST_VARIABLE) {
        REPORT_ERROR("Error: Invalid CREATE statement!\n");
        return 0;
    }

    char name[100];
    snprintf(name, sizeof(name), "%s", node->left->text);

    Value new_value = {0};
    Value array_items[MAX_ARRAY_SIZE];
    int array_size = 0;

    if (node->right->type == AST_ARRAY) {
        for (ASTNode *item = node->right->left;
             item; item = item->next) {

            if (array_size >= MAX_ARRAY_SIZE) {
                REPORT_ERROR("Error: Array is too large!\n");
                return 0;
            }

            if (!evaluate(item, &array_items[array_size]))
                return 0;

            array_size++;
        }

        new_value.type = AST_ARRAY;
        new_value.number = array_size;
    } else {
        if (!evaluate(node->right, &new_value))
            return 0;
    }

    Variable *var = find_variable(name);

    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES) {
            REPORT_ERROR("Error: Too many variables!\n");
            return 0;
        }

        var = &variables[variable_count++];
    }

    memset(var, 0, sizeof(*var));

    snprintf(var->name, sizeof(var->name), "%s", name);
    var->value = new_value;

    if (new_value.type == AST_ARRAY) {
        var->array_size = array_size;

        for (int i = 0; i < array_size; i++)
            var->items[i] = array_items[i];
    }

    return 1;
}

static void print_value(Value value)
{
    switch (value.type) {
        case AST_STRING:
            printf("%s\n", value.text);
            break;

        case AST_BOOLEAN:
            printf("%s\n", truthy(value) ? "TRUE" : "FALSE");
            break;

        case AST_NUMBER:
            printf("%g\n", value.number);
            break;

        case AST_ARRAY:
            printf("Array with %d elements\n", (int)value.number);
            break;

        default:
            printf("<value>\n");
            break;
    }
}

/* FUNCTION CALLING: HEAP MEMORY FIX */

static int call_function(
    const char *name,
    ASTNode *arguments,
    Value *result
)
{
    Function *function = find_function(name);

    if (function == NULL) {
        REPORT_ERROR("Error: Undefined function '%s'!\n", name);
        return 0;
    }

    if (call_depth >= MAX_CALL_DEPTH) {
        REPORT_ERROR("Error: Maximum function call depth exceeded!\n");
        return 0;
    }

    Value values[MAX_VARIABLES];
    int argument_count = 0;

    ASTNode *parameter = function->parameters;
    ASTNode *argument = arguments;

    while (parameter && argument) {
        if (argument_count >= MAX_VARIABLES ||
            !evaluate(argument, &values[argument_count]))
            return 0;

        argument_count++;
        parameter = parameter->next;
        argument = argument->next;
    }

    if (parameter || argument) {
        REPORT_ERROR("Error: Function '%s' argument count mismatch!\n",
                     name);
        return 0;
    }

    int saved_count = variable_count;

    Variable *saved_variables = malloc(sizeof(variables));

    if (saved_variables == NULL) {
        REPORT_ERROR("Error: Memory allocation failed!\n");
        return 0;
    }

    memcpy(saved_variables, variables, sizeof(variables));

    variable_count = 0;
    parameter = function->parameters;

    for (int i = 0; i < argument_count; i++) {
        Variable *var = &variables[variable_count++];

        memset(var, 0, sizeof(*var));

        snprintf(var->name, sizeof(var->name),
                 "%s", parameter->text);

        var->value = values[i];
        parameter = parameter->next;
    }

    Value returned_value = make_number(0);

    call_depth++;

    int returned = execute_block(function->body, &returned_value);

    call_depth--;

    memcpy(variables, saved_variables, sizeof(variables));
    variable_count = saved_count;

    free(saved_variables);

    if (runtime_error)
        return 0;

    if (!returned) {
        REPORT_ERROR("Error: Function '%s' did not return a value!\n",
                     name);
        return 0;
    }

    *result = returned_value;
    return 1;
}

static int execute_block(ASTNode *program, Value *return_value)
{
    for (ASTNode *node = program; node; node = node->next) {
        if (runtime_error)
            return 0;

        if (node->type == AST_FUNCTION)
            continue;

        if (node->type == AST_CREATE) {
            if (!create_variable(node)) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Could not create variable!\n");
                return 0;
            }
        }

        else if (node->type == AST_INPUT) {
            char input[100];

            printf("%s: ", node->text);
            fflush(stdout);

            if (!fgets(input, sizeof(input), stdin)) {
                REPORT_ERROR("Error: Failed to read INPUT value!\n");
                return 0;
            }

            input[strcspn(input, "\n")] = '\0';

            Variable *var = find_variable(node->text);

            if (!var) {
                if (variable_count >= MAX_VARIABLES) {
                    REPORT_ERROR("Error: Too many variables!\n");
                    return 0;
                }

                var = &variables[variable_count++];
            }

            memset(var, 0, sizeof(*var));
            snprintf(var->name, sizeof(var->name), "%s", node->text);

            char *end;
            double number = strtod(input, &end);

            if (input[0] != '\0' && *end == '\0')
                var->value = make_number(number);
            else
                var->value = make_string(input);
        }

        else if (node->type == AST_PRINT) {
            Value value;

            if (!evaluate(node->left, &value)) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Could not evaluate PRINT value!\n");
                return 0;
            }

            print_value(value);
        }

        else if (node->type == AST_CALL) {
            Value ignored;

            if (!call_function(node->text, node->left, &ignored)) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Function call failed!\n");
                return 0;
            }
        }

        else if (node->type == AST_RETURN) {
            if (!node->left || !evaluate(node->left, return_value)) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Invalid RETURN value!\n");
                return 0;
            }

            return 1;
        }

        else if (node->type == AST_IF) {
            int condition;

            if (!evaluate_condition(node->left, &condition))
                return 0;

            int returned = condition
                ? execute_block(node->right, return_value)
                : execute_block(node->else_branch, return_value);

            if (runtime_error)
                return 0;

            if (returned)
                return 1;
        }

        else if (node->type == AST_REPEAT) {
            Value count;

            if (!evaluate(node->left, &count) ||
                count.type != AST_NUMBER ||
                count.number < 0 ||
                count.number > MAX_LOOP_COUNT ||
                floor(count.number) != count.number) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Invalid REPEAT count!\n");
                return 0;
            }

            for (int i = 0; i < (int)count.number; i++) {
                int returned = execute_block(node->right, return_value);

                if (runtime_error)
                    return 0;

                if (returned)
                    return 1;
            }
        }

        else if (node->type == AST_WHILE) {
            int condition;
            int iterations = 0;

            while (!runtime_error) {
                if (!evaluate_condition(node->left, &condition))
                    return 0;

                if (!condition)
                    break;

                if (iterations++ >= MAX_LOOP_COUNT) {
                    REPORT_ERROR(
                        "Error: WHILE loop exceeded maximum iterations!\n"
                    );
                    return 0;
                }

                int returned = execute_block(node->right, return_value);

                if (runtime_error)
                    return 0;

                if (returned)
                    return 1;
            }
        }

        else if (node->type == AST_APPEND) {
            Variable *var = find_variable(node->text);
            Value value;

            if (!var || var->value.type != AST_ARRAY ||
                !evaluate(node->left, &value)) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Invalid APPEND operation!\n");
                return 0;
            }

            if (var->array_size >= MAX_ARRAY_SIZE) {
                REPORT_ERROR("Error: Array is full!\n");
                return 0;
            }

            var->items[var->array_size++] = value;
            var->value.number = var->array_size;
        }

        else if (node->type == AST_DELETE) {
            Variable *var = find_variable(node->text);
            Value index;

            if (!var || var->value.type != AST_ARRAY ||
                !evaluate(node->left, &index) ||
                index.type != AST_NUMBER ||
                index.number < 0 ||
                index.number >= var->array_size ||
                floor(index.number) != index.number) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Invalid DELETE index!\n");
                return 0;
            }

            int position = (int)index.number;

            for (int i = position; i < var->array_size - 1; i++)
                var->items[i] = var->items[i + 1];

            var->array_size--;
            var->value.number = var->array_size;
        }

        else if (node->type == AST_SET_INDEX) {
            Variable *var = find_variable(node->text);
            Value index, value;

            if (!var || var->value.type != AST_ARRAY ||
                !evaluate(node->left, &index) ||
                !evaluate(node->right, &value) ||
                index.type != AST_NUMBER ||
                index.number < 0 ||
                index.number >= var->array_size ||
                floor(index.number) != index.number) {
                if (!runtime_error)
                    REPORT_ERROR("Error: Invalid SET index!\n");
                return 0;
            }

            var->items[(int)index.number] = value;
        }
    }

    return 0;
}

void interpret(ASTNode *program)
{
    variable_count = 0;
    function_count = 0;
    call_depth = 0;
    runtime_error = 0;

    register_functions(program);

    if (runtime_error)
        return;

    Value ignored = make_number(0);
    execute_block(program, &ignored);
}
