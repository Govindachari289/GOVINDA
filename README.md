# GOVINDA

### A Custom Programming Language Built from Scratch in C

GOVINDA is an experimental programming language developed from scratch using the C programming language. It is designed to explore the fundamentals of language design, lexical analysis, parsing, abstract syntax trees, and interpretation.

The project is being developed as a learning journey into compiler and interpreter design.

---

## Features

GOVINDA currently supports:

- Variables using `CREATE`
- Output using `PRINT`
- User input using `INPUT`
- Arithmetic operations (`+`, `-`, `*`, `/`)
- Comparison operators (`>`, `<`, `>=`, `<=`, `==`, `!=`)
- Logical operators (`AND`, `OR`, `NOT`)
- Conditional statements (`IF`, `ELSE`, `END`)
- Loops (`REPEAT`, `WHILE`)
- User-defined functions
- Function parameters and return values
- Recursive function calls
- String handling and concatenation
- Arrays and array indexing
- Array operations (`APPEND`, `SET`, `DELETE`)
- Built-in functions (`LENGTH`, `LEN`, `NUMBER`, `STRING`, `ABS`, `ROUND`, `FLOOR`, `CEIL`)
- Runtime error handling

---

## Example Programs

### Hello World

```text
PRINT "Hello, World!"
PRINT "Welcome to GOVINDA"
```

### Variables and Arithmetic

```text
CREATE a = 10
CREATE b = 20
CREATE result = a + b

PRINT result
```

### Conditional Statements

```text
CREATE age = 20

IF age >= 18
    PRINT "You are an adult"
ELSE
    PRINT "You are not an adult"
END
```

### Loops

```text
CREATE count = 1

WHILE count <= 5
    PRINT count
    CREATE count = count + 1
END
```

### Functions

```text
FUNCTION add(a, b)
    RETURN a + b
END

CREATE result = add(10, 20)
PRINT result
```

### Recursion

```text
FUNCTION factorial(n)
    IF n <= 1
        RETURN 1
    END

    RETURN n * factorial(n - 1)
END

PRINT factorial(5)
```

### Arrays

```text
CREATE numbers = [10, 20, 30, 40]

PRINT numbers[0]
PRINT numbers[2]
```

---

## Project Structure

```text
GOVINDA/
│
├── include/
│   ├── lexer.h
│   ├── ast.h
│   ├── parser.h
│   └── interpreter.h
│
├── src/
│   ├── main.c
│   ├── lexer.c
│   ├── ast.c
│   ├── parser.c
│   └── interpreter.c
│
├── programs/
├── tests/
├── README.md
└── .gitignore
```

---

## Requirements

- GCC Compiler
- Linux / Ubuntu / WSL
- Visual Studio Code (recommended)

## Installation and Execution

### 1. Clone the repository

```bash
git clone https://github.com/Govindachari289/GOVINDA.git
```

### 2. Enter the project directory

```bash
cd GOVINDA
```

### 3. Compile GOVINDA

```bash
cd src

gcc -Wall -Wextra -std=c11 -I../include main.c lexer.c ast.c parser.c interpreter.c -lm -o govinda
```

### 4. Run a GOVINDA program

```bash
./govinda ../programs/your_program.gov
```

---

## Development Roadmap

- [x] Lexer and Tokenizer
- [x] Abstract Syntax Tree (AST)
- [x] Parser
- [x] Interpreter
- [x] Variables and Expressions
- [x] Conditional Statements
- [x] Loops
- [x] Functions and Parameters
- [x] Recursion
- [x] Input and Output
- [x] String Handling
- [x] Arrays
- [x] Logical Operators
- [x] Built-in Functions
- [x] Initial Runtime Error Handling
- [ ] Comprehensive Testing and Stability
- [ ] Version 1.0 Release
- [ ] Future Compiler Development

---

## Author

**Govindachari289**

B.Tech Computer Science Engineering Student  
Gurukula Kangri University

GitHub: https://github.com/Govindachari289

---

## Project Status

GOVINDA is an actively developed personal programming language project.

The current implementation is an interpreter written in C. New features, improvements, and stability enhancements are planned.

**Built with C, curiosity, and a passion for programming language development.**

---

⭐ If you find this project interesting, consider giving it a star!
