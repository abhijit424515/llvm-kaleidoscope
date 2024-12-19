# LLVM Frontend for the Kaleidoscope language

This is a simple LLVM frontend for the Kaleidoscope language, as described in the LLVM tutorial.

## Grammar

```js
top ::= definition | external | expression | ';'
definition ::= 'def' prototype expression
prototype ::= id '(' id* ')'
external ::= 'extern' prototype
expression ::= primary binoprhs
binoprhs ::= (op primary binoprhs)*
primary ::= id | numberexpr | parenexpr | call
parenexpr ::= '(' expression ')'
numberexpr ::= [0-9]+
call ::= id '(' expression* ')'
op ::= '+' | '-' | '*' | '/' | '<'
id ::= [a-zA-Z][a-zA-Z0-9]*
```