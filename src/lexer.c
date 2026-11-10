#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

static const char *token_name(TokenType type)
{
    switch (type) {
        case TOKEN_CREATE: return "CREATE";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_INPUT: return "INPUT";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_END: return "END";
        case TOKEN_REPEAT: return "REPEAT";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_CALL: return "CALL";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_APPEND: return "APPEND";
        case TOKEN_DELETE: return "DELETE";
        case TOKEN_SET: return "SET";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        default: return "UNKNOWN";
    }
}

static TokenType get_keyword_type(const char *word)
{
    if (strcmp(word, "CREATE") == 0) return TOKEN_CREATE;
    if (strcmp(word, "PRINT") == 0) return TOKEN_PRINT;
    if (strcmp(word, "INPUT") == 0) return TOKEN_INPUT;
    if (strcmp(word, "IF") == 0) return TOKEN_IF;
    if (strcmp(word, "ELSE") == 0) return TOKEN_ELSE;
    if (strcmp(word, "END") == 0) return TOKEN_END;
    if (strcmp(word, "REPEAT") == 0) return TOKEN_REPEAT;
    if (strcmp(word, "WHILE") == 0) return TOKEN_WHILE;
    if (strcmp(word, "FUNCTION") == 0) return TOKEN_FUNCTION;
    if (strcmp(word, "CALL") == 0) return TOKEN_CALL;
    if (strcmp(word, "RETURN") == 0) return TOKEN_RETURN;
    if (strcmp(word, "TRUE") == 0) return TOKEN_TRUE;
    if (strcmp(word, "FALSE") == 0) return TOKEN_FALSE;
    if (strcmp(word, "AND") == 0) return TOKEN_AND;
    if (strcmp(word, "OR") == 0) return TOKEN_OR;
    if (strcmp(word, "NOT") == 0) return TOKEN_NOT;
    if (strcmp(word, "APPEND") == 0) return TOKEN_APPEND;
    if (strcmp(word, "DELETE") == 0) return TOKEN_DELETE;
    if (strcmp(word, "SET") == 0) return TOKEN_SET;

    return TOKEN_IDENTIFIER;
}

static void print_token(TokenType type, const char *value, int line)
{
    printf("Line %d: %-15s %s\n", line, token_name(type), value);
}

void tokenize(const char *source)
{
    if (source == NULL) {
        fprintf(stderr, "Lexer error: source is NULL.\n");
        return;
    }

    int i = 0;
    int line = 1;

    while (source[i] != '\0') {
        if (source[i] == '\n') {
            line++;
            i++;
            continue;
        }

        if (isspace((unsigned char)source[i])) {
            i++;
            continue;
        }

        /* Identifier or keyword */
        if (isalpha((unsigned char)source[i]) || source[i] == '_') {
            char word[100];
            int j = 0;

            while (isalnum((unsigned char)source[i]) ||
                   source[i] == '_') {
                if (j < 99) {
                    word[j++] = source[i];
                }
                i++;
            }

            word[j] = '\0';

            TokenType type = get_keyword_type(word);
            print_token(type, word, line);
            continue;
        }

        /* Number */
        if (isdigit((unsigned char)source[i])) {
            char number[100];
            int j = 0;
            int decimal_seen = 0;

            while (isdigit((unsigned char)source[i]) ||
                   (source[i] == '.' && !decimal_seen)) {
                if (source[i] == '.') {
                    decimal_seen = 1;
                }

                if (j < 99) {
                    number[j++] = source[i];
                }

                i++;
            }

            number[j] = '\0';
            print_token(TOKEN_NUMBER, number, line);
            continue;
        }

        /* String */
        if (source[i] == '"') {
            char string[100];
            int j = 0;

            i++;

            while (source[i] != '\0' && source[i] != '"') {
                if (source[i] == '\\' && source[i + 1] != '\0') {
                    i++;

                    if (source[i] == 'n') {
                        if (j < 99) string[j++] = '\n';
                    } else if (source[i] == 't') {
                        if (j < 99) string[j++] = '\t';
                    } else {
                        if (j < 99) string[j++] = source[i];
                    }
                } else {
                    if (source[i] == '\n') {
                        line++;
                    }

                    if (j < 99) {
                        string[j++] = source[i];
                    }
                }

                i++;
            }

            if (source[i] == '"') {
                i++;
            } else {
                fprintf(stderr,
                        "Lexer error: unterminated string at line %d.\n",
                        line);
            }

            string[j] = '\0';
            print_token(TOKEN_STRING, string, line);
            continue;
        }

        /* Two-character operators */
        if (source[i] == '>' && source[i + 1] == '=') {
            print_token(TOKEN_GREATER_EQUAL, ">=", line);
            i += 2;
            continue;
        }

        if (source[i] == '<' && source[i + 1] == '=') {
            print_token(TOKEN_LESS_EQUAL, "<=", line);
            i += 2;
            continue;
        }

        if (source[i] == '=' && source[i + 1] == '=') {
            print_token(TOKEN_EQUAL, "==", line);
            i += 2;
            continue;
        }

        if (source[i] == '!' && source[i + 1] == '=') {
            print_token(TOKEN_NOT_EQUAL, "!=", line);
            i += 2;
            continue;
        }

        /* Single-character operators and symbols */
        char symbol[2] = { source[i], '\0' };

        switch (source[i]) {
            case '=':
                print_token(TOKEN_ASSIGN, symbol, line);
                break;

            case '+':
                print_token(TOKEN_PLUS, symbol, line);
                break;

            case '-':
                print_token(TOKEN_MINUS, symbol, line);
                break;

            case '*':
                print_token(TOKEN_MULTIPLY, symbol, line);
                break;

            case '/':
                print_token(TOKEN_DIVIDE, symbol, line);
                break;

            case '%':
                print_token(TOKEN_MODULO, symbol, line);
                break;

            case '(':
                print_token(TOKEN_LEFT_PAREN, symbol, line);
                break;

            case ')':
                print_token(TOKEN_RIGHT_PAREN, symbol, line);
                break;

            case '[':
                print_token(TOKEN_LEFT_BRACKET, symbol, line);
                break;

            case ']':
                print_token(TOKEN_RIGHT_BRACKET, symbol, line);
                break;

            case ',':
                print_token(TOKEN_COMMA, symbol, line);
                break;

            case '>':
                print_token(TOKEN_GREATER, symbol, line);
                break;

            case '<':
                print_token(TOKEN_LESS, symbol, line);
                break;

            default:
                print_token(TOKEN_UNKNOWN, symbol, line);
                break;
        }

        i++;
    }
}