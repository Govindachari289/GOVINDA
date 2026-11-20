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

## Download and Use (Windows, no setup needed)

You do not need to install GCC or any other tool. You only need Windows and VS Code (or any terminal).

### Step 1: Download GOVINDA

1. Open the [Releases page](https://github.com/Govindachari289/GOVINDA/releases).
2. Under **GOVINDA v0.1**, click **govinda.exe** to download it.

**Note:** Windows may show a "protected your PC" warning because the file is new. Click **More info**, then **Run anyway**.

### Step 2: Make a folder

1. Create a new folder, for example `MyGovinda`, on your Desktop.
2. Move `govinda.exe` into that folder.

### Step 3: Open the folder in VS Code

1. Open VS Code.
2. Click **File → Open Folder**.
3. Choose the `MyGovinda` folder and click **Select Folder**.

### Step 4: Create your first program

1. In the left panel of VS Code, click the **New File** icon.
2. Name the file `hello.gov`. GOVINDA programs use the `.gov` extension.
3. Type this code and save with **Ctrl + S**:

```text
PRINT "Hello, World!"
```

### Step 5: Run it

1. In VS Code, click **Terminal → New Terminal**.
2. Type this command and press Enter:

```powershell
.\govinda.exe hello.gov
```

You will see:

```text
Hello, World!
```

**Troubleshooting:** If the terminal cannot find `govinda.exe`, make sure the terminal is open in the same folder as the file. Type `dir` to see the files in the current folder.

**Linux and macOS users:** build GOVINDA from source using the steps in the "Build from Source" section below.

---

## Example Programs

### Hello World

```text
PRINT "Hello, World!"
PRINT "Welcome to GOVINDA"
```

Output:

```text
Hello, World!
Welcome to GOVINDA
```

### Variables and Arithmetic

```text
CREATE a = 10
CREATE b = 20
CREATE result = a + b

PRINT result
```

Output:

```text
30
```

> **Note:** `CREATE` is used both to make a new variable and to change the value of an existing one.

### Conditional Statements

```text
CREATE age = 20

IF age >= 18
    PRINT "You are an adult"
ELSE
    PRINT "You are not an adult"
END
```

Output:

```text
You are an adult
```

### Loops

```text
CREATE count = 1

WHILE count <= 5
    PRINT count
    CREATE count = count + 1
END
```

Output:

```text
1
2
3
4
5
```

### Functions

```text
FUNCTION add(a, b)
    RETURN a + b
END

CREATE result = add(10, 20)
PRINT result
```

Output:

```text
30
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

Output:

```text
120
```

### Arrays

```text
CREATE numbers = [10, 20, 30, 40]

PRINT numbers[0]
PRINT numbers[2]
```

Output:

```text
10
30
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
├── LICENSE
├── README.md
└── .gitignore
```

---

## Build from Source

If you want to build GOVINDA yourself, you need GCC.

- **Windows:** GCC (MinGW-w64)
- **Linux:** GCC
- **macOS:** GCC or Clang (not yet tested)

Visual Studio Code is recommended but optional.

### 1. Clone the repository

```bash
git clone https://github.com/Govindachari289/GOVINDA.git
cd GOVINDA
```

### 2. Windows (PowerShell / CMD)

Install GCC using MinGW-w64 and make sure GCC is added to PATH.

Compile:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/lexer.c src/ast.c src/parser.c src/interpreter.c -lm -o govinda.exe
```

Run:

```powershell
.\govinda.exe programs\hello.gov
```

### 3. Linux / Ubuntu / WSL

Install GCC if required:

```bash
sudo apt update
sudo apt install build-essential
```

Compile:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/main.c src/lexer.c src/ast.c src/parser.c src/interpreter.c -lm -o govinda
```

Run:

```bash
./govinda programs/hello.gov
```

### 4. macOS

Install Xcode Command Line Tools:

```bash
xcode-select --install
```

Build using GCC or Clang with the same source files and appropriate compiler command. macOS compatibility has not yet been tested.

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
- [x] Comprehensive Testing and Stability
- [x] First Release (v0.1)
- [ ] Version 1.0 Release
- [ ] Future Compiler Development

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

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
