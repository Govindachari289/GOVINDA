#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char *argv[])
{
    printf("\n");
    printf("============================\n");
    printf("      GOVINDA INTERPRETER\n");
    printf("============================\n");

    if (argc != 2) {
        printf("Usage: govinda <filename>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open source file '%s'!\n",
                argv[1]);
        return EXIT_FAILURE;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Cannot seek source file!\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    long file_size = ftell(file);

    if (file_size < 0) {
        fprintf(stderr, "Error: Cannot determine file size!\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    rewind(file);

    char *source = malloc((size_t)file_size + 1);

    if (source == NULL) {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(source, 1, (size_t)file_size, file);

    if (ferror(file)) {
        fprintf(stderr, "Error: Failed to read source file!\n");
        free(source);
        fclose(file);
        return EXIT_FAILURE;
    }

    source[bytes_read] = '\0';
    fclose(file);

    ASTNode *tree = parse_program(source);

    if (tree != NULL) {
        interpret(tree);
        ast_free(tree);
    }

    free(source);

    printf("\nProgram execution completed!\n");

    return EXIT_SUCCESS;
}