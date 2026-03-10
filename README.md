# Transpiler

A lightweight transpiler that converts arithmetic expressions into GAS (GNU Assembler) syntax for x86-64 architecture, using the AT&T syntax. Built from scratch to understand how compilers work under the hood — from parsing to code generation.

How it works?
  1. Lexical Analysis — Converts input string into tokens (numbers, operators, parentheses)
  2. Parsing — Uses Shunting Yard algorithm to build an AST with correct operator precedence
  3. AST Representation — Tree structure with Num (leaf) and Op (internal) nodes
  4. Code Generation — Recursively traverses AST and emits GAS assembly in AT&T syntax
