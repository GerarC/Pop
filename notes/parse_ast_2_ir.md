# Parsing Ast to Intermediate Representation

So... I have the next Ast:

```bash
.
└─ -
   ├─ +
   │  ├─ 1
   │  └─ *
   │     ├─ 2
   │     └─ 3
   └─ /
      ├─ *
      │  ├─ 4
      │  └─ 5
      └─ +
         ├─ 6
         └─ 7
```

And I need the next intermediate representation

```bash
t1 = 2 * 3
t2 = 1 + t1
t3 = 4 * 5
t4 = 6 + 7
t5 = t3 + t4
t6 = t2 + t5
```

How can i reach that?

1. the token just has the Lexeme and the token type.
   ```c
   typedef struct toke_t {
   	TokenType type;
   	char* lexeme;
   }
   ```
2. My AST is represented by an struct like
   ```c
   typedef struct node_t {
   	Token tok;
   	node_t left;
   	node_t right;
   } Node;
   ```
3. My IR is represented by the next struct
   ```c
   typedef struct node_t {
   	IrOpType type; // ADD, MUL, SUB, DIV
   	IrValue arg1;
   	IrValue arg2;
   	IrValue arg3;
   } Node;
   ```

Returning to this. In the Anita's video she shows something that i liked so much and it is a really interesting form.

The idea is to create a table-like Three Address Code representation where the indexes says where do you saves things.
For example, my code is represented by the next table:

|Index|Op|Arg1|Arg2|
|--:|:-:|:-:|:-:|
|0|*|2|3|
|1|+|1|(0)|
|2|*|4|5|
|3|+|6|7|
|4|/|(2)|(3)|
|5|-|(1)|(4)|
