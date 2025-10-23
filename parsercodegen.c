/*
Assignment:
HW3 - Parser and Code Generator for PL/0
Author(s): Dean Walker, Mark Wlodawski
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

// Function Prototypes



// STEP 2: TOKEN FILE READER

// STEP 3: SYMBOL TABLE IMPLEMENTATION

// STEP 4: CODE GENERATION HELPER

// STEP 5: RECURSIVE-DESCENT PARSER IMPLEMENTATION

// STEP 6: FINAL OUTPUT