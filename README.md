# TinyLex
A small, lightweight and cross language lexer code generator

# Syntax

## Projects
Using the 'project' keyword, you can specify the name of the project. The use for this will change for each language

	project <ProjectName>


## Tokens
To define a token, simple put the name, followed by white space and then the regular expression encoding that token. Each token is separated by a new line
	
	<Name>  <Expression>
	...
	
## Regular Expressions
The definitions of tokens follow standard regular expressions

| Operation | Description                                                   |
|-----------|---------------------------------------------------------------|
| (...)     | Sub expression                                                |
| [...]     | Any of the chars are accepted including ranges, e.g. 'a-z'    |
| ?         | One or none                                                   |
| +         | One or more                                                   |
| *         | Zero or more                                                  |
| \|        | Or                                                            |

# Usage
Parse your source files into tinylex using the command line and it'll output a file depending on your language of choice

## C++
A header file will be generates. Place this file into your projects source and include it when needed. On a single source file, define 'TINYLEX_IMPLEMNT' before including the file

	...
	#define TINYLEX_IMPLEMENT
	#include "tinylex.h"
	...

The classes 'Token' and 'Lexer' will be under the namespace of your project name. Creating a new Lexer and parsing in the input file or stream will open the new source. The method 'next' will return the next token in the stream. Documentation on this API is below

### Lexer
| Member | Type | Description |
|--------|------|-------------|
| next() | method | Returns the next token in the stream |
| is_eof() | method | Return if the end of the stream as been reached |

## Token
| Member | Type | Description |
| ------ | ---- | ----------- |
| data | char * | A pointer to the tokens data |
| type | Token::TokenType | The type of the token |
| type_name | const char * | Token's type name as a string |

