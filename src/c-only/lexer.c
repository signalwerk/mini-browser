#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

void Lexer_init(Lexer *lexer, const char *input)
{
    lexer->input = input;
    lexer->position = 0;
    lexer->blockCount = 0;
    lexer->insideBlock = 0;
}

static char Lexer_peek(Lexer *lexer)
{
    return lexer->input[lexer->position];
}

static char Lexer_advance(Lexer *lexer)
{
    return lexer->input[lexer->position++];
}

static Token Lexer_createToken(TokenType type, const char *value)
{
    Token token;
    token.type = type;
    token.value = strdup(value);
    return token;
}

static Token Lexer_handleSelector(Lexer *lexer)
{
    char currentChar = Lexer_peek(lexer);
    char buffer[1024];
    int bufIndex = 0;

    while (isalnum(currentChar) || currentChar == '.' || currentChar == '#' || currentChar == '*' || currentChar == '>' || currentChar == ':' || currentChar == '-' || currentChar == '_')
    {
        buffer[bufIndex++] = Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }
    buffer[bufIndex] = '\0';

    return Lexer_createToken(TOKEN_SELECTOR, buffer);
}

static Token Lexer_handleDeclaration(Lexer *lexer)
{
    char buffer[1024];
    int bufIndex = 0;
    char currentChar = Lexer_peek(lexer);

    while (currentChar != ';' && currentChar != '\0')
    {
        buffer[bufIndex++] = Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }
    buffer[bufIndex] = '\0';

    return Lexer_createToken(TOKEN_DECLARATION, buffer);
}

static Token Lexer_handleAtRule(Lexer *lexer)
{
    char buffer[1024];
    int bufIndex = 0;
    char currentChar = Lexer_advance(lexer); // advance the '@' character

    while (isalpha(currentChar) || currentChar == '-')
    {
        buffer[bufIndex++] = Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }
    buffer[bufIndex] = '\0';

    // Check if the next character is '{', indicating a block begins.
    // We don't consume the '{' here; let Lexer_nextToken handle that.
    if (Lexer_peek(lexer) == '{')
    {
        lexer->blockCount++;
    }

    return Lexer_createToken(TOKEN_AT_RULE, buffer);
}

static Token Lexer_skipComment(Lexer *lexer)
{
    while (!(Lexer_peek(lexer) == '*' && lexer->input[lexer->position + 1] == '/') && Lexer_peek(lexer) != '\0')
    {
        Lexer_advance(lexer);
    }
    Lexer_advance(lexer); // advance '*'
    Lexer_advance(lexer); // advance '/'

    return Lexer_nextToken(lexer); // call Lexer_nextToken to get the next valid token
}

static Token Lexer_handleStartBlock(Lexer *lexer)
{
    Lexer_advance(lexer); // advance the '{' character
    lexer->blockCount++;  // Increase block count
    return Lexer_createToken(TOKEN_START_BLOCK, "{");
}

static Token Lexer_handleEndBlock(Lexer *lexer)
{
    Lexer_advance(lexer); // advance the '}' character
    lexer->blockCount--;  // Decrease block count
    return Lexer_createToken(TOKEN_END_BLOCK, "}");
}

static Token Lexer_handlePropertyName(Lexer *lexer)
{
    char buffer[1024];
    int bufIndex = 0;
    char currentChar = Lexer_peek(lexer);

    while (isalpha(currentChar) || currentChar == '-')
    {
        buffer[bufIndex++] = Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }
    buffer[bufIndex] = '\0';

    return Lexer_createToken(TOKEN_PROPERTY_NAME, buffer);
}

static Token Lexer_handlePropertyValue(Lexer *lexer)
{
    char buffer[1024];
    int bufIndex = 0;
    char currentChar = Lexer_peek(lexer);

    // skip whitespace after the colon
    while (isspace(currentChar))
    {
        Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }

    while (!isspace(currentChar) && currentChar != ';' && currentChar != '}')
    {
        buffer[bufIndex++] = Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }
    buffer[bufIndex] = '\0';

    return Lexer_createToken(TOKEN_PROPERTY_VALUE, buffer);
}

Token Lexer_nextToken(Lexer *lexer)
{
    char currentChar = Lexer_peek(lexer);

    while (isspace(currentChar))
    {
        Lexer_advance(lexer);
        currentChar = Lexer_peek(lexer);
    }

    // Handle comments
    if (currentChar == '/' && lexer->input[lexer->position + 1] == '*')
    {
        return Lexer_skipComment(lexer);
    }

    if (currentChar == '\0')
    {
        return Lexer_createToken(TOKEN_EOF, "");
    }

    if (lexer->insideBlock)
    {
        if (isalpha(currentChar))
        {
            Token propertyNameToken = Lexer_handlePropertyName(lexer);
            if (Lexer_peek(lexer) == ':') // If next token is colon, we return the property name
            {
                return propertyNameToken;
            }
            return Lexer_createToken(TOKEN_UNKNOWN, propertyNameToken.value);
        }

        if (currentChar == ':')
        {
            Lexer_advance(lexer); // Consume the colon
            return Lexer_handlePropertyValue(lexer);
        }

        if (currentChar == ';')
        {
            Lexer_advance(lexer);
            return Lexer_createToken(TOKEN_SEMICOLON, ";");
        }

        if (currentChar == '}')
        {
            lexer->blockCount--;
            if (lexer->blockCount == 0)
                lexer->insideBlock = 0;
            Lexer_advance(lexer);
            return Lexer_createToken(TOKEN_END_BLOCK, "}");
        }
    }
    else
    {
        if (isalpha(currentChar) || currentChar == '.' || currentChar == '#' || currentChar == '*' || currentChar == '>' || currentChar == ':')
        {
            return Lexer_handleSelector(lexer);
        }

        if (currentChar == ',')
        {
            Lexer_advance(lexer);
            return Lexer_createToken(TOKEN_COMMA, ",");
        }

        if (currentChar == '@')
        {
            return Lexer_handleAtRule(lexer);
        }

        if (currentChar == '{')
        {
            lexer->blockCount++;
            lexer->insideBlock = 1;
            Lexer_advance(lexer);
            return Lexer_createToken(TOKEN_START_BLOCK, "{");
        }
    }

    // If the current character isn't recognized, treat it as unknown and advance
    char buffer[2] = {Lexer_advance(lexer), '\0'};
    return Lexer_createToken(TOKEN_UNKNOWN, buffer);
}

void Lexer_printToken(Token token)
{
    const char *typeName = NULL;
    bool printDebug = false;

    switch (token.type)
    {
    case TOKEN_IDENTIFIER:
        typeName = "IDENTIFIER";
        break;
    case TOKEN_SEMICOLON:
        typeName = "SEMICOLON";
        break;
    case TOKEN_COLON:
        typeName = "COLON";
        break;
    case TOKEN_EOF:
        typeName = "EOF";
        break;
    case TOKEN_UNKNOWN:
        typeName = "UNKNOWN";
        break;
    case TOKEN_SELECTOR:
        typeName = "SELECTOR";
        break;
    case TOKEN_DECLARATION:
        typeName = "DECLARATION";
        break;
    case TOKEN_AT_RULE:
        typeName = "AT_RULE";
        break;
    case TOKEN_HASH:
        typeName = "HASH";
        break;
    case TOKEN_START_BLOCK:
        typeName = "START_BLOCK";
        break;
    case TOKEN_END_BLOCK:
        typeName = "END_BLOCK";
        break;
    case TOKEN_PROPERTY_NAME:
        typeName = "PROPERTY_NAME";
        break;
    case TOKEN_PROPERTY_VALUE:
        typeName = "PROPERTY_VALUE";
        break;
    case TOKEN_COMMA:
        typeName = "COMMA";
        break;

    default:
        printDebug = true;
        typeName = "TYPE_UNKNOWN";
        break;
    }

    if (printDebug)
    {
        printf("Type: %-20s (%d) '%s'\n", typeName, token.type, token.value);
    }
    else
    {
        printf("%-20s '%s'\n", typeName, token.value);
    }
}
