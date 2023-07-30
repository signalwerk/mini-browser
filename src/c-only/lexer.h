#ifndef LEXER_H
#define LEXER_H

typedef enum
{
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_SELECTOR,
    TOKEN_DECLARATION,
    TOKEN_AT_RULE,
    TOKEN_HASH,
    TOKEN_START_BLOCK,
    TOKEN_END_BLOCK,
    TOKEN_PROPERTY_NAME,
    TOKEN_PROPERTY_VALUE,
    TOKEN_COMMA
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

typedef struct
{
    const char *input;
    int position;
    int blockCount;  // 0 if not inside any block, increments for nested blocks
    int insideBlock; // 1 if inside, 0 otherwise

} Lexer;

void Lexer_init(Lexer *lexer, const char *input);
Token Lexer_nextToken(Lexer *lexer);
void Lexer_printToken(Token token);

#endif // LEXER_H
