# Pop

This is a language that I want to create to practice C and assembler.
The idea of this language isn't create new things, as a test language
I will steal pieces that i love from other languages like _C_, _Java_, _JS_ or _Python_.
The name of the laguage comes from this mix of things, i.e., a "Popurrí" (medley) of stolen features.

To achieve this languages I know that I'll search a lot of references for the code, structure and lang ideas. In order to recognize that, in the resources section there are all the references I remember to write.

My greatest reference and inspiration to begin this travel has been [Tsoding](https://www.youtube.com/@TsodingDaily).

### Steps

By the moment the steps I've followed to reach the compiler has been:

1. _Lexical Analysis:_  
    Create the list of tokens where that laguage receives. A pop token is the next structure:

   ```C
   typedef struct _token_t {
        TokenType type;
        TokenLocation location;
        int length;
        char *lexeme;
        void *value;
   } Token;
   ```

   It's so useful to save types, locations and values of tokens.

2. _Syntactic Analysis:_
   This parses the list of tokens into a AST following the defined language grammar.

3. _Semantic Analysis:_
   The semantic analysis verify that operands, assigments, so on and so forth
   are being consistent operating types with compatible types in a determined scope of the application.
   I can analyze this through scopes that saves types and symbols of each code block inside a symbol table.
   Symbol table used in this attempt of language is inspired on python one.

4. _Intermediate Representation:_
   Now, In this step I convert the verified AST into three address code or "TAC". This tac is easier to generate ASM with.
   In my code the Intermediate Representation is a list of operations and the operations is represented by the next structure:

   ```C
   typedef struct _operation_t {
        IrOperationType type;
        IrValue arg1;
        IrValue arg2;
        IrValue result;
   } IrOperation;
   ```

   these `arg1` and `arg2` can be a number or the index into another Operation, this is to connect easily when the IR transforms into assembly.
   The idea is to create a table-like Three Address Code representation where the indexes says where do you saves things.
   For example, `(2*3 + 1) - (4*5 / 6*7)` is represented by the next table:

   | Index | Op  | Arg1 | Arg2 |
   | ----: | :-: | :--: | :--: |
   |     0 | \*  |  2   |  3   |
   |     1 |  +  |  1   | (0)  |
   |     2 | \*  |  4   |  5   |
   |     3 |  +  |  6   |  7   |
   |     4 |  /  | (2)  | (3)  |
   |     5 |  -  | (1)  | (4)  |

5. _Generation of code:_
   Actually this step is so simple when you already has and intermediate representation it's just translate the IR into the asm code of your predilection.
   Pop is using nasm, but in the future it's also planned implement ARM, MIPS and stuff like that.

## Compile

### Linux

This project uses make as building system.
So it's just needed to execute the next command:

```sh
make
```

it'll create a build dir with the executable inside.

### Windows

I think I'm not going to implement this, at least not soon.
It could be in a future. But the idea of mine is to enjoy this project
and I know that center of deploy to windows will make me have headaches.

## Examples

If you want to explore what this language can do, you can compile the examples.
There are some examples:

- 01-math_expr
- 02-if_else
- 03-variables
- 04-while
- 05-types
- 06-basic-funct

To compile and explore these examples use the next command:

```bash
make ex<example_name>
```

for example `make ex03-variables`

## Milestones

- [x] Compiled to native instructions
- [x] Variables
- [x] Loops
- [x] Statically typed (Like Typescript)
- [x] Chars
- [x] Functions
- [ ] Arrays
- [ ] Strings
- [ ] Turing-complete (Can emulate itself)
- [ ] Read and write files
- [ ] python-like import system
- [ ] Structs
- [ ] Classes
- [ ] Self-hosted (when the classes are done)
- [ ] Optimized
- [ ] First class functions
- [ ] Arrow functions
- [ ] Map, filter, foreach, ..., etc (I will see things of haskell)
- [ ] List comprehesion-like system

## Resources

These are some of the resources I used:

- [Compilers _Principles, Techniques & Tools_](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)
- [Porth](https://gitlab.com/tsoding/porth)
  > The inspiration of this project. Is like a complete different language, but gave me the strength to begin this.
- [Crafting Interpreters](https://craftinginterpreters.com/)
  > From here you can take the idea of the Lexer, the grammar and a more useful things. But it's thought by using with an OOP language. It difficults to translate it into C
- [Semantic Analysis](https://www.youtube.com/watch?v=cC8YRnDGMwI&ab_channel=Dr.SagarkumarBadhiye)
  > Why and how to make the semantic analysis.
- [Another example of compiler](https://www.youtube.com/watch?v=-4RmhDy0A2s&list=PLRnI_2_ZWhtA_ZAzEa8uJF8wgGF0HjjEz&ab_channel=CobbCoding)
- [Intermediate Representation](https://www.youtube.com/watch?v=u2qLQep_Wzw&ab_channel=AnitaR)
  > A little explanation about how works the three address code for the intermediate representation.
- [Compiler in C of DoctorWkt](https://github.com/DoctorWkt/acwj)
  > This is a great discoverage, The project is basically the same. When I don't know how to implement something I go to this repository to stole ideas and learn how to do things.
- [Assembler Directives](https://docs.oracle.com/cd/E19253-01/817-5477/eoiyg/index.html)
  > Used basically to know how does work the `.comm` directive.
- [Color terminal](https://askubuntu.com/questions/558280/changing-colour-of-text-and-background-of-terminal)

## Thanks

- **Sagarkumar Badhiye:**
  Thanks to _Sagarkumar_ the hindi man that saves this project without knowing.
  He has a video where he explains what is the semantic analysis and gave me the idea of manage that using the table.
  It was hard to reach that Idea even if the examples of _Crafting Interpreters_
  because they explain that using Java classes and patterns of OOP programming.
  Now i know that the symboltable is the right aproach. A link to the video that helped me to reach this is in the references.

- **AnitaR:**
  Oh thanks god to have found her video explaining three address code. Literally i had no idea how to implement that and
  it was just watch that video to shine my mind into something functional. I love U Anita.

- **DoctorWkt:** Thanks to show me `r8`, `r9`, `r10` and `r11` registers. I was having a lot of problems using `R*X`'s type ones.
  Also thanks for your register managing way, I thought it before seen your way, but I really chose it because of your code.
