
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

ASTNode *ast_new(ASTNodeType type)
{
    ASTNode *node = calloc(1, sizeof(ASTNode));

    if (node == NULL) {
        fprintf(stderr, "Error: AST memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    node->type = type;

    return node;
}

void ast_free(ASTNode *node)
{
    if (node == NULL) {
        return;
    }

    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->next);
    ast_free(node->else_branch);

    free(node);
}
