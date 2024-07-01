flags=-Wall -Wextra
libs=-lbsd
out=gcc -o 
name=pop
src=./src/
icd=./include/
build=./build/
target=$(build)$(name)

objs=$(src)ir.o \
	 $(src)log.o \
	 $(src)file.o \
	 $(src)main.o  \
	 $(src)token.o  \
	 $(src)lexer.o	 \
	 $(src)parser.o   \
	 $(src)semantic.o  \
	 $(src)asmlines.o   \
	 $(src)symboltable.o \
	 $(src)generation/nasm_x86_64.o \

all: compile clean

compile: $(objs)
	@[ -d "$(build)" ] || { mkdir "$(build)"; }
	$(out)$(target) $(objs) $(flags) $(libs)

ex_%:
	$(target) -c ./examples/$@.pop -o $@.asm && cat -n $@.asm && nasm $@.asm -felf64 -o $@.o && ld $@.o -o $@
	echo "EXECUTION: "
	./$@
	rm ./$@*


clean:
		rm $(objs)

debug:
	gcc -g $(src)* -o $(target)_d


run: 
	$(target)
