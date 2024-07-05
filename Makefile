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

ex%:
	@printf "\n\e[48;5;87m\e[38;5;0m    COMPILATION:    \e[0m \n"
	@$(target) -c ./examples/$@.pop -o $@.asm
	@nasm $@.asm -felf64 -o $@.o && ld $@.o -o $@
	@printf "\n\e[48;5;88m    ASSEMBLER CODE:    \e[0m\n"
	@cat -n $@.asm
	@printf "\n\e[48;5;129m\e[38;5;0m    ORIGINAL CODE:    \e[0m \n"
	@cat -n ./examples/$@.pop
	@printf "\n\e[48;5;136m\e[38;5;0m    EXECUTION:    \e[0m\n"
	@./$@
	@rm ./$@*


clean:
		@rm $(objs)

debug:
	gcc -g $(src)* -o $(target)_d


run: 
	$(target)
