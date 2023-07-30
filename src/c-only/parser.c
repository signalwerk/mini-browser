#include "parser.h"
#include "lexer.h" // Include the lexer.h header file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper functions
static void Parser_error(const char *message)
{
    fprintf(stderr, "Parser Error: %s\n", message);
    exit(1);
}

// Helper function to advance the lexer and get the next token
static Token Parser_nextToken(struct Parser *parser)
{
    return Lexer_nextToken(&parser->lexer);
}

static struct selector Parser_parseSelector(struct Parser *parser)
{
    // Implementation of selector parsing logic
    // This is a placeholder. You need to implement the actual selector parsing.
    struct selector sel;
    sel.tag_name = NULL;
    sel.id = NULL;
    sel._class = NULL;
    sel.class_count = 0;
    sel.descendants = NULL;
    sel.descendant_count = 0;
    return sel;
}

static struct declaration Parser_parseDeclaration(struct Parser *parser)
{
    // Implementation of declaration parsing logic
    // This is a placeholder. You need to implement the actual declaration parsing.
    struct declaration decl;
    decl.name = NULL;
    decl.value = NULL;
    return decl;
}

static struct rule Parser_parseRule(struct Parser *parser)
{
    struct rule rule;
    // Parse selectors
    rule.selectors = NULL;
    rule.selector_count = 0;
    // Parse declarations
    rule.declarations = NULL;
    rule.declaration_count = 0;
    rule.entity.type = 0; // Rule type

    // Example implementation (parse at least one selector and one declaration)
    rule.selectors = (struct selector *)malloc(sizeof(struct selector));
    rule.selectors[0] = Parser_parseSelector(parser);
    rule.selector_count = 1;

    rule.declarations = (struct declaration *)malloc(sizeof(struct declaration));
    rule.declarations[0] = Parser_parseDeclaration(parser);
    rule.declaration_count = 1;

    return rule;
}

static struct atrule Parser_parseAtRule(struct Parser *parser)
{
    struct atrule atrule;
    // Parse keyword and value
    atrule.keyword = NULL;
    atrule.value = NULL;
    atrule.entity.type = 1; // AtRule type

    // Example implementation (parse at least one keyword and one value)
    // This is a placeholder. You need to implement the actual AtRule parsing.
    atrule.keyword = strdup("media");
    atrule.value = strdup("(max-width: 600px)");

    return atrule;
}

static struct CSSEntity *Parser_parseRuleOrAtRule(struct Parser *parser);

struct stylesheet *Parser_parseStylesheet(struct Parser *parser)
{
    struct stylesheet *sheet = (struct stylesheet *)malloc(sizeof(struct stylesheet));
    sheet->entities = NULL;
    sheet->entity_count = 0;

    // Parse rules and at-rules
    Token currentToken = Parser_nextToken(parser); // Get the first token
    while (currentToken.type != TOKEN_EOF)
    {
        parser->currentToken = currentToken; // Update the current token in the parser
        struct CSSEntity *entity = Parser_parseRuleOrAtRule(parser);
        // Append the parsed entity to the entities list
        sheet->entities = (struct CSSEntity *)realloc(sheet->entities, (sheet->entity_count + 1) * sizeof(struct CSSEntity));
        sheet->entities[sheet->entity_count++] = *entity;
        currentToken = Parser_nextToken(parser); // Get the next token
    }

    return sheet;
}

struct CSSEntity *Parser_parseRuleOrAtRule(struct Parser *parser)
{
    struct CSSEntity *entity = (struct CSSEntity *)malloc(sizeof(struct CSSEntity));
    entity->type = -1; // Invalid type, for error handling

    // Check the type of the next token
    if (parser->currentToken.type == TOKEN_IDENTIFIER)
    {
        // It's a rule (since it starts with an identifier)
        struct rule *parsedRule = (struct rule *)malloc(sizeof(struct rule));
        parsedRule->selectors = NULL;
        parsedRule->selector_count = 0;
        parsedRule->declarations = NULL;
        parsedRule->declaration_count = 0;
        parsedRule->entity.type = 0; // Rule type

        // Implement the actual rule parsing logic here

        entity = (struct CSSEntity *)parsedRule;
    }
    else if (parser->currentToken.type == TOKEN_HASH || parser->currentToken.type == TOKEN_IDENTIFIER)
    {
        // It's an at-rule (since it starts with a hash or an identifier)
        struct atrule *parsedAtRule = (struct atrule *)malloc(sizeof(struct atrule));
        parsedAtRule->keyword = NULL;
        parsedAtRule->value = NULL;
        parsedAtRule->innerEntities = NULL;
        parsedAtRule->innerEntity_count = 0;
        parsedAtRule->entity.type = 1; // AtRule type

        // Implement the actual at-rule parsing logic here

        entity = (struct CSSEntity *)parsedAtRule;
    }
    else
    {
        // Invalid token, report an error or handle it according to your parser's error-handling strategy
        Lexer_printToken(parser->currentToken);
        Parser_error("Unexpected token encountered");
    }

    // Move to the next token
    parser->currentToken = Parser_nextToken(parser);
    return entity;
}

// Helper function to advance the lexer
void Parser_advance(struct Parser *parser)
{
    // Changed Lexer_advance to Parser_advance
    Parser_advance(parser);
}

void Parser_init(struct Parser *parser, const char *input)
{
    Lexer_init(&parser->lexer, input);
    parser->currentToken = Lexer_nextToken(&parser->lexer); // Initialize current token
}

void Parser_cleanupStylesheet(struct stylesheet *sheet)
{
    if (sheet)
    {
        for (int i = 0; i < sheet->entity_count; ++i)
        {
            // Free memory based on the type of the entity (rule or at-rule)
            struct CSSEntity *entity = &sheet->entities[i];
            if (entity->type == 0)
            {
                // Rule
                struct rule *r = (struct rule *)entity;
                // TODO: Free memory for selectors and declarations
                // ...

                free(r->selectors);
                free(r->declarations);
            }
            else if (entity->type == 1)
            {
                // AtRule
                struct atrule *ar = (struct atrule *)entity;
                // TODO: Free memory for keyword, value, and innerEntities
                // ...

                free(ar->keyword);
                free(ar->value);
                free(ar->innerEntities);
            }
        }

        free(sheet->entities);
        free(sheet);
    }
}

void Parser_printTokens(struct Parser *parser) {
    while (parser->currentToken.type != TOKEN_EOF) {
        Lexer_printToken(parser->currentToken);
        if (parser->currentToken.type != TOKEN_EOF && parser->currentToken.type != TOKEN_UNKNOWN) {
            free(parser->currentToken.value); // Make sure you free the memory allocated to the token value
        }
        parser->currentToken = Lexer_nextToken(&parser->lexer);
    }
}
