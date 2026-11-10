
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

static const char *current;

static void skip_inline_spaces(void)
{
    while (*current == ' ' || *current == '\t' || *current == '\r')
        current++;
}

static void skip_line(void)
{
    while (*current && *current != '\n')
        current++;

    if (*current == '\n')
        current++;
}

static void skip_blank_lines(void)
{
    while (*current) {
        skip_inline_spaces();

        if (*current == '\n' || *current == '\r')
            current++;
        else
            break;
    }
}

static int match_word(const char *word)
{
    size_t length = strlen(word);

    return strncmp(current, word, length) == 0 &&
           !isalnum((unsigned char)current[length]) &&
           current[length] != '_';
}

static void parse_name(char name[100])
{
    skip_inline_spaces();

    int i = 0;

    if (!isalpha((unsigned char)*current) && *current != '_') {
        printf("Error: Missing name!\n");
        name[0] = '\0';
        return;
    }

    while (isalnum((unsigned char)*current) || *current == '_') {
        if (i < 99)
            name[i++] = *current;

        current++;
    }

    name[i] = '\0';
}

static ASTNode *parse_expression(void);
static ASTNode *parse_block(int stop_at_branch);

static void append_node(ASTNode **head, ASTNode **tail, ASTNode *node)
{
    if (!node)
        return;

    if (!*head)
        *head = node;
    else
        (*tail)->next = node;

    *tail = node;
}

static ASTNode *parse_list(int parameters)
{
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    skip_inline_spaces();

    if (*current == ')')
        return NULL;

    while (*current && *current != ')') {
        ASTNode *item;

        if (parameters) {
            item = ast_new(AST_VARIABLE);
            parse_name(item->text);

            if (item->text[0] == '\0') {
                ast_free(item);
                break;
            }
        } else {
            item = parse_expression();

            if (!item) {
                printf("Error: Invalid function argument!\n");
                break;
            }
        }

        append_node(&head, &tail, item);

        skip_inline_spaces();

        if (*current == ',') {
            current++;
            skip_inline_spaces();
        } else {
            break;
        }
    }

    return head;
}

static ASTNode *parse_call_expression(void)
{
    current += 4;

    ASTNode *node = ast_new(AST_CALL);
    parse_name(node->text);

    skip_inline_spaces();

    if (*current == '(') {
        current++;
        node->left = parse_list(0);

        skip_inline_spaces();

        if (*current == ')')
            current++;
        else
            printf("Error: Missing ')' in function call!\n");
    } else {
        printf("Error: Missing '(' in function call!\n");
    }

    return node;
}

static ASTNode *parse_value(void)
{
    skip_inline_spaces();

    if (*current == '"') {
        current++;

        ASTNode *node = ast_new(AST_STRING);
        int i = 0;

        while (*current && *current != '"' && *current != '\n') {
            if (*current == '\\' && current[1]) {
                current++;

                if (*current == 'n' && i < 99)
                    node->text[i++] = '\n';
                else if (*current == 't' && i < 99)
                    node->text[i++] = '\t';
                else if (i < 99)
                    node->text[i++] = *current;
            } else if (i < 99) {
                node->text[i++] = *current;
            }

            current++;
        }

        node->text[i] = '\0';

        if (*current == '"')
            current++;
        else
            printf("Error: Missing closing quote!\n");

        return node;
    }

    if (match_word("TRUE")) {
        current += 4;
        ASTNode *node = ast_new(AST_BOOLEAN);
        node->number = 1;
        return node;
    }

    if (match_word("FALSE")) {
        current += 5;
        ASTNode *node = ast_new(AST_BOOLEAN);
        node->number = 0;
        return node;
    }

    if (isdigit((unsigned char)*current) ||
        (*current == '-' &&
         (isdigit((unsigned char)current[1]) || current[1] == '.')) ||
        (*current == '.' && isdigit((unsigned char)current[1]))) {

        ASTNode *node = ast_new(AST_NUMBER);
        char *end;

        node->number = strtod(current, &end);
        current = end;

        return node;
    }

    if (isalpha((unsigned char)*current) || *current == '_') {
        ASTNode *node = ast_new(AST_VARIABLE);
        parse_name(node->text);
        return node;
    }

    return NULL;
}

static ASTNode *parse_array(void)
{
    current++;

    ASTNode *node = ast_new(AST_ARRAY);
    ASTNode *tail = NULL;

    skip_inline_spaces();

    while (*current && *current != ']') {
        ASTNode *item = parse_expression();

        if (!item) {
            printf("Error: Invalid array element!\n");
            break;
        }

        append_node(&node->left, &tail, item);

        skip_inline_spaces();

        if (*current == ',') {
            current++;
            skip_inline_spaces();
        } else {
            break;
        }
    }

    if (*current == ']')
        current++;
    else
        printf("Error: Missing ']' in array!\n");

    return node;
}

/* BUILT-IN FUNCTION NAME CHECK */

static int is_builtin_name(const char *name)
{
    return strcmp(name, "LENGTH") == 0 ||
           strcmp(name, "LEN") == 0 ||
           strcmp(name, "NUMBER") == 0 ||
           strcmp(name, "STRING") == 0 ||
           strcmp(name, "ABS") == 0 ||
           strcmp(name, "ROUND") == 0 ||
           strcmp(name, "FLOOR") == 0 ||
           strcmp(name, "CEIL") == 0;
}

/* FACTOR PARSER */

static ASTNode *parse_factor(void)
{
    skip_inline_spaces();

    if (match_word("CALL"))
        return parse_call_expression();

    if (match_word("NOT")) {
        current += 3;

        ASTNode *node = ast_new(AST_UNARY);
        node->operator = '!';
        node->left = parse_factor();

        return node;
    }

    if (*current == '-') {
        current++;

        ASTNode *node = ast_new(AST_UNARY);
        node->operator = '-';
        node->left = parse_factor();

        return node;
    }

    if (*current == '[')
        return parse_array();

    if (*current == '(') {
        current++;

        ASTNode *node = parse_expression();

        skip_inline_spaces();

        if (*current == ')')
            current++;
        else
            printf("Error: Missing closing parenthesis!\n");

        return node;
    }

    ASTNode *node = parse_value();

    if (!node)
        return NULL;

    skip_inline_spaces();

    /* BUILT-IN AND USER FUNCTION CALL PARSING */

    if (node->type == AST_VARIABLE && *current == '(') {
        ASTNode *call;

        if (is_builtin_name(node->text))
            call = ast_new(AST_BUILTIN);
        else
            call = ast_new(AST_CALL);

        snprintf(call->text, sizeof(call->text), "%s", node->text);
        ast_free(node);

        current++;

        call->left = parse_list(0);

        skip_inline_spaces();

        if (*current == ')')
            current++;
        else
            printf("Error: Missing ')' in function call!\n");

        return call;
    }

    /* ARRAY INDEX PARSING */

    if (node->type == AST_VARIABLE && *current == '[') {
        ASTNode *index = ast_new(AST_INDEX);

        strcpy(index->text, node->text);
        ast_free(node);

        current++;

        index->left = parse_expression();

        skip_inline_spaces();

        if (*current == ']')
            current++;
        else
            printf("Error: Missing ']' after array index!\n");

        return index;
    }

    return node;
}

/* TERM PARSER */

static ASTNode *parse_term(void)
{
    ASTNode *left = parse_factor();

    while (left) {
        skip_inline_spaces();

        if (*current != '*' && *current != '/' && *current != '%')
            break;

        char op = *current++;

        ASTNode *right = parse_factor();

        if (!right) {
            printf("Error: Missing right operand!\n");
            return left;
        }

        ASTNode *node = ast_new(AST_BINARY);

        node->operator = op;
        node->left = left;
        node->right = right;

        left = node;
    }

    return left;
}

/* EXPRESSION PARSER */

static ASTNode *parse_expression(void)
{
    ASTNode *left = parse_term();

    while (left) {
        skip_inline_spaces();

        if (*current != '+' && *current != '-')
            break;

        char op = *current++;

        ASTNode *right = parse_term();

        if (!right) {
            printf("Error: Missing right operand!\n");
            return left;
        }

        ASTNode *node = ast_new(AST_BINARY);

        node->operator = op;
        node->left = left;
        node->right = right;

        left = node;
    }

    return left;
}

/* COMPARISON PARSER */

static ASTNode *parse_comparison(void)
{
    ASTNode *left = parse_expression();

    skip_inline_spaces();

    char op;

    if (current[0] == '>' && current[1] == '=') {
        op = 'G';
        current += 2;
    } else if (current[0] == '<' && current[1] == '=') {
        op = 'L';
        current += 2;
    } else if (current[0] == '=' && current[1] == '=') {
        op = '=';
        current += 2;
    } else if (current[0] == '!' && current[1] == '=') {
        op = '!';
        current += 2;
    } else if (*current == '>') {
        op = '>';
        current++;
    } else if (*current == '<') {
        op = '<';
        current++;
    } else {
        return left;
    }

    ASTNode *right = parse_expression();

    if (!left || !right) {
        printf("Error: Invalid comparison!\n");
        ast_free(left);
        ast_free(right);
        return NULL;
    }

    ASTNode *node = ast_new(AST_COMPARE);

    node->operator = op;
    node->left = left;
    node->right = right;

    return node;
}

/* LOGICAL CONDITION PARSER */

static ASTNode *parse_condition(void)
{
    ASTNode *left = parse_comparison();

    while (left) {
        skip_inline_spaces();

        char op = '\0';

        if (match_word("AND")) {
            op = '&';
            current += 3;
        } else if (match_word("OR")) {
            op = '|';
            current += 2;
        } else {
            break;
        }

        ASTNode *right = parse_comparison();

        if (!right) {
            ast_free(left);
            return NULL;
        }

        ASTNode *node = ast_new(AST_LOGICAL);

        node->operator = op;
        node->left = left;
        node->right = right;

        left = node;
    }

    return left;
}

/* BLOCK PARSER */

static ASTNode *parse_block(int stop_at_branch)
{
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (*current) {
        skip_blank_lines();

        if (!*current)
            break;

        if (stop_at_branch &&
            (match_word("ELSE") || match_word("END")))
            break;

        ASTNode *statement = NULL;

        if (match_word("FUNCTION")) {
            current += 8;

            statement = ast_new(AST_FUNCTION);

            parse_name(statement->text);

            skip_inline_spaces();

            if (*current == '(') {
                current++;

                statement->left = parse_list(1);

                skip_inline_spaces();

                if (*current == ')')
                    current++;
                else
                    printf("Error: Missing ')' in FUNCTION!\n");
            } else {
                printf("Error: Missing '(' in FUNCTION!\n");
            }

            skip_line();

            statement->right = parse_block(1);

            skip_blank_lines();

            if (match_word("END")) {
                current += 3;
                skip_line();
            } else {
                printf("Error: Missing END for FUNCTION!\n");
            }
        }

        else if (match_word("RETURN")) {
            current += 6;

            statement = ast_new(AST_RETURN);
            statement->left = parse_expression();

            skip_line();
        }

        else if (match_word("INPUT")) {
            current += 5;

            statement = ast_new(AST_INPUT);
            parse_name(statement->text);

            skip_line();
        }

        else if (match_word("CALL")) {
            statement = parse_call_expression();

            skip_line();
        }

        else if (match_word("CREATE")) {
            current += 6;

            statement = ast_new(AST_CREATE);
            statement->left = parse_value();

            skip_inline_spaces();

            if (*current == '=')
                current++;
            else
                printf("Error: Missing '=' in CREATE!\n");

            statement->right = parse_expression();

            skip_line();
        }

        else if (match_word("SET")) {
            current += 3;

            statement = ast_new(AST_SET_INDEX);
            parse_name(statement->text);

            skip_inline_spaces();

            if (*current == '[') {
                current++;

                statement->left = parse_expression();

                skip_inline_spaces();

                if (*current == ']')
                    current++;
                else
                    printf("Error: Missing ']' in SET!\n");
            } else {
                printf("Error: Missing '[' in SET!\n");
            }

            skip_inline_spaces();

            if (*current == '=')
                current++;
            else
                printf("Error: Missing '=' in SET!\n");

            statement->right = parse_expression();

            skip_line();
        }

        else if (match_word("APPEND")) {
            current += 6;

            statement = ast_new(AST_APPEND);

            parse_name(statement->text);

            statement->left = parse_expression();

            skip_line();
        }

        else if (match_word("DELETE")) {
            current += 6;

            statement = ast_new(AST_DELETE);

            parse_name(statement->text);

            statement->left = parse_expression();

            skip_line();
        }

        else if (match_word("PRINT")) {
            current += 5;

            statement = ast_new(AST_PRINT);
            statement->left = parse_expression();

            skip_line();
        }

        else if (match_word("IF")) {
            current += 2;

            statement = ast_new(AST_IF);
            statement->left = parse_condition();

            skip_line();

            statement->right = parse_block(1);

            skip_blank_lines();

            if (match_word("ELSE")) {
                current += 4;
                skip_line();

                statement->else_branch = parse_block(1);

                skip_blank_lines();
            }

            if (match_word("END")) {
                current += 3;
                skip_line();
            } else {
                printf("Error: Missing END for IF block!\n");
            }
        }

        else if (match_word("REPEAT")) {
            current += 6;

            statement = ast_new(AST_REPEAT);
            statement->left = parse_expression();

            skip_line();

            statement->right = parse_block(1);

            skip_blank_lines();

            if (match_word("END")) {
                current += 3;
                skip_line();
            } else {
                printf("Error: Missing END for REPEAT loop!\n");
            }
        }

        else if (match_word("WHILE")) {
            current += 5;

            statement = ast_new(AST_WHILE);
            statement->left = parse_condition();

            skip_line();

            statement->right = parse_block(1);

            skip_blank_lines();

            if (match_word("END")) {
                current += 3;
                skip_line();
            } else {
                printf("Error: Missing END for WHILE loop!\n");
            }
        }

        else {
            skip_line();
            continue;
        }

        append_node(&head, &tail, statement);
    }

    return head;
}

/* PROGRAM ENTRY */

ASTNode *parse_program(const char *source)
{
    if (!source)
        return NULL;

    current = source;

    return parse_block(0);
}
