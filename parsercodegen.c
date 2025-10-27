/*
Assignment:
HW3 - Parser and Code Generator for PL/0
Author : < Dean Walker, Mark Wlodawski >
Language: C (only)
To Compile:
Scanner:
gcc -O2 -std=c11 -o lex lex.c
Parser/Code Generator:
gcc -O2 -std=c11 -o parsercodegen parsercodegen.c
To Execute (on Eustis):
./lex <input_file.txt>
./parsercodegen
where:
<input_file.txt> is the path to the PL/0 source program
Notes:
- lex.c accepts ONE command-line argument (input PL/0 source file)
- parsercodegen.c accepts NO command-line arguments
- Input filename is hard-coded in parsercodegen.c
- Implements recursive-descent parser for PL/0 grammar
- Generates PM/0 assembly code (see Appendix A for ISA)
- All development and testing performed on Eustis
Class: COP3402 - System Software - Fall 2025
Instructor: Dr. Jie Lin
Due Date: Friday, October 31, 2025 at 11:59 PM ET
*/

// STEP 1: PREPARATION AND SETUP

// INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// CONSTANTS
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500
#define MAX_TOKENS 500

// ENUMERATIONS - FROM lex.c FILE
typedef enum 
{
    skipsym = 1,
    identsym, 
    numbersym,
    plussym, 
    minussym, 
    multsym, 
    slashsym,
    eqsym, 
    neqsym, 
    lessym, 
    leqsym, 
    gtrsym, 
    geqsym,
    lparentsym, 
    rparentsym, 
    commasym, 
    semicolonsym,
    periodsym, 
    becomessym,
    beginsym, 
    endsym, 
    ifsym, 
    fisym, 
    thensym,
    whilesym, 
    dosym, 
    callsym, 
    constsym, 
    varsym,
    procsym, 
    writesym,
    readsym, 
    elsesym, 
    evensym
} TokenType;

// STRUCTS

typedef struct 
{
    TokenType type;
    char lexeme[12];
} Token;

typedef struct {
    int mark;
    int addr;
    int level;
    int kind;
    char name[12];
    int val;
} symbol;

typedef struct {
    int l;
    int m;
    int op;
} instruction;

// IMPORTANT ARRAYS
Token tokens[MAX_TOKENS];
int tokenIndex = 0;
int tokenCount = 0;
Token currentToken;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbolCount = 0;

instruction code[MAX_CODE_LENGTH];
int codeIndex = 0;

// NAMES OF FILES
const char *OUTPUT_FILE = "elf.txt";
const char *TOKEN_FILE = "tokensPrint.txt"; // Matches lex.c output file

// The Error Messages
const char *ERRORS[] = {
    "Error: program must end with period", // 1
    "Error: const, var, and read keywords must be followed by identifier", // 2
    "Error: symbol name has already been declared", // 3
    "Error: constants must be assigned with =", // 4
    "Error: constants must be assigned an integer value", // 5
    "Error: constant and variable declarations must be followed by a semicolon", // 6
    "Error: undeclared identifier", // 7
    "Error: only variable values may be altered", // 8
    "Error: assignment statements must use :=", // 9
    "Error: begin must be followed by end", // 10
    "Error: if must be followed by then", // 11
    "Error: while must be followed by do", // 12
    "Error: condition must contain comparison operator", // 13
    "Error: right parenthesis must follow left parenthesis", // 14
    "Error: arithmetic equations must contain operands, parentheses, numbers, or symbols", // 15
};

// Function Prototypes
void factor(void);
void term(void);
void expression(void);
void statement(void);
void condition(void);
void block(void);
void const_declaration(void);
void program(void);
int var_declaration(void);

void writeOutput(void);
void emit(int op, int l, int m);
void error(numError);
void nextToken(void);
int symbol_table_check(char *name);
void nextToken(void);
void loadTokens(void);

// STEP 2: TOKEN FILE READER
void loadTokens()
{
    FILE *fp = fopen(TOKEN_FILE, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: cannot open token file '%s'\n", TOKEN_FILE);
        exit(1);
    }

    tokenCount = 0;
    while (!feof(fp))
    {
        int type;
        if (fscanf(fp, "%d", &type) !=1)
        break; // When the end of the file or malformed input comes, stop

        tokens[tokenCount].type = (TokenType)type;
        strcpy(tokens[tokenCount].lexeme, ""); // lexeme with nothing inside

        // Identifier or number token, check the number/lexeme
        if (type == identsym || type == numbersym)
        {
            if (fscanf(fp, "%11s", tokens[tokenCount].lexeme) != 1)
            {
                fprintf(stderr, "Error: token is malformed at the index %d\n", tokenCount);
                fclose(fp);
                exit(1);
            }
        }

        tokenCount++;
        if (tokenCount >= MAX_TOKENS)
        {
            fprintf(stderr, "Error: there are too many tokens (limit %d)\n", MAX_TOKENS);
            fclose(fp);
            exit(1);
        }
    }

    fclose(fp);
}

// Proceed to next token
void nextToken(void)
{
    if (tokenIndex < tokenCount)
        currentToken = tokens[tokenIndex++];
    else
    {
        currentToken.type = periodsym; // End of program at end of the file
        strcpy(currentToken.lexeme, ".");
    }
}

// Reporting Errors
void error(int numError) {
    const char *msg = ERRORS[numError];
    printf("%s\n", msg); // Terminal shows error message

    FILE *fp = fopen(OUTPUT_FILE, "w"); // elf.txt receives output
    if (fp) {
        fprintf(fp, "%s\n", msg);
        fclose(fp);
    }
    exit(1);
}

// STEP 3: SYMBOL TABLE IMPLEMENTATION
int symbol_table_check(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (symbol_table[i].mark == 0 && strcmp(symbol_table[i].name, name) == 0) {
            return i; // Symbol is found
        }
    }
    return -1; // Symbol is not found
}

int get_symbol_index(char *name) {
    int index = symbol_table_check(name);
    if (index == -1) {
        error(7); // Error number 7
    }
    return index;
}

void symbol_table_add(int kind, char *name, int val, int level, int addr) {
    if (symbol_table_check(name) != -1) {
        error(3); // Error number 3
    }
    symbol_table[symbolCount].kind = kind;
    strcpy(symbol_table[symbolCount].name, name);
    symbol_table[symbolCount].val = val;
    symbol_table[symbolCount].level = level;
    symbol_table[symbolCount].addr = addr;
    symbol_table[symbolCount].mark = 0; // An available mark
    symbolCount++;
}

// STEP 4: CODE GENERATION HELPER

// STEP 5: RECURSIVE-DESCENT PARSER IMPLEMENTATION

// STEP 6: FINAL OUTPUT