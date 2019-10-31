# TinyLex
A small, lightweight and cross language lexer code generator

## Syntax
To define a token, simple put the name, followed by white space and then the regular expression encoding that token. Each token is separated by a new line
	
	<Name>  <Expression>
	...
	
The definitions of tokens follow standard regular expressions
| Operation | Description                                                   |
|-----------|---------------------------------------------------------------|
|(...)      |- Sub expression                                               |
|[...]      |- Any of the chars are accepted including ranges, e.g. 'a-z'   |
|?          |- One or none                                                  |
|+          |- One or more                                                  |
|*          |- Zero or more                                                 |
|\|         |- Or                                                           |

