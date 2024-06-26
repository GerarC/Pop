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

## Milestones

- [ ] Compiled to native instructions
- [ ] Variables
- [ ] Arrays
- [ ] Chars
- [ ] Strings
- [ ] Functions
- [ ] Statically typed (Like Typescript)
- [ ] Turing-complete (Can emulate itself)
- [ ] Read and write files
- [ ] Structs
- [ ] Classes
- [ ] Self-hosted (when the classes are done)
- [ ] Optimized
- [ ] List comprehesion-like system

## Resources

These are some of the resources I used:

- [Compilers _Principles, Techniques & Tools_](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)
- [Porth](https://gitlab.com/tsoding/porth)
- [Crafting Interpreters](https://craftinginterpreters.com/)
- [Semantic Analysis](https://www.youtube.com/watch?v=cC8YRnDGMwI&ab_channel=Dr.SagarkumarBadhiye)

## Thanks

- **Sagarkumar Badhiye:**
  Thanks to _Sagarkumar_ the hindi man that saves this project without knowing.
  He has a video where he explains what is the semantic analysis and gave me the idea of manage that using the table.
  It was hard to reach that Idea even if the examples of _Crafting Interpreters_
  because they explain that using Java classes and patterns of OOP programming.
  Now i know that the symboltable is the right aproach. A link to the video that helped me to reach this is in the references.
