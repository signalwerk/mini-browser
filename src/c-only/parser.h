#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Forward declaration of the structs defined in your parser
struct selector;
struct declaration;
struct rule;
struct atrule;
struct stylesheet;

// Define the Parser struct here
struct Parser
{
    Lexer lexer;
    Token currentToken; // Add this field to hold the current token

    // Add any other fields that your parser might need
};

struct selector
{
    char *tag_name;
    char *id;
    char **_class;
    int class_count;
    struct selector *descendants;
    int descendant_count;
};

struct declaration
{
    char *name;
    char *value;
};

struct CSSEntity
{
    int type; // 0: rule, 1: atrule
};

struct rule
{
    struct selector *selectors;
    int selector_count;
    struct declaration *declarations;
    int declaration_count;
    struct CSSEntity entity;
};

struct atrule
{
    char *keyword;
    char *value;
    struct CSSEntity *innerEntities;
    int innerEntity_count;
    struct CSSEntity entity;
};

struct stylesheet
{
    struct CSSEntity *entities;
    int entity_count;
};

// Functions
// Functions
void Parser_init(struct Parser *parser, const char *input);
struct stylesheet *Parser_parseStylesheet(struct Parser *parser);
void Parser_cleanupStylesheet(struct stylesheet *sheet);
void Parser_advance(struct Parser *parser);
void Parser_printTokens(struct Parser *parser);

#endif // PARSER_H
