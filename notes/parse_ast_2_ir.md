#  Parsing Ast to Intermediate Representation
So... I have the next Ast:
~~~ bash
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
~~~

And I need the next intermediate representation
~~~ bash
t1 = 2 * 3
t2 = 1 + t1
t3 = 4 * 5
t4 = 6 + 7
t5 = t3 + t4
t6 = t2 + t5
~~~

How can i reach that?
1. the token just has the Lexeme and the token type.
	~~~ c
	typedef struct toke_t {
		TokenType type;
		char* lexeme;
	}
	~~~
2. My AST is represented by an struct like
	~~~ c
	typedef struct node_t {
		Token tok;
		node_t left;
		node_t right;
	} Node;
	~~~
2. My IR is represented by the next struct
	~~~ c
	typedef struct node_t {
		IrOpType type; // ADD, MUL, SUB, DIV
		IrValue arg1;
		IrValue arg2;
		IrValue arg3;
	} Node;
	~~~


We can call those operations binary ops and the number a literal;
