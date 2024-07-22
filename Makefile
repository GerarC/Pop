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
	 $(src)common.o   \
	 $(src)parser.o    \
	 $(src)semantic.o   \
	 $(src)symboltable.o \
	 $(src)generation/nasm_x86_64.o \

all: compile clean

compile: $(objs)
	@[ -d "$(build)" ] || { mkdir "$(build)"; }
	$(out)$(target) $(objs) $(flags) $(libs)

ex%:
	@$(eval example := $(shell echo "$@""*" | sed 's/ex//g'  | xargs -ro find examples/ -name | sed 's#.*/\([^/]*\)\..*#\1#'))
	@printf "\n\e[48;5;137m\e[38;5;0m    COMPILATION:    \e[0m \n"
	@$(target) -c ./examples/$(example).pop -o $(example).asm --debug
	@printf "\n\e[48;5;88m    ASSEMBLER CODE:    \e[0m\n"
	@cat -n $(example).asm
	@nasm $(example).asm -felf64 -o $(example).o && ld $(example).o -o $(example)
	@printf "\n\e[48;5;129m\e[38;5;0m    ORIGINAL CODE:    \e[0m \n"
	@cat -n ./examples/$(example).pop
	@printf "\n\e[48;5;136m\e[38;5;0m    EXECUTION:    \e[0m\n"
	@./$(example)
	@rm ./$(example)*

test%:
	@$(eval test := $(shell echo "$@""*" | sed 's/test//g' | xargs -I {} find test/ -name "{}.pop" | sed 's#.*/\([^/]*\)\..*#\1#'))
	@printf "\n\e[48;5;137m\e[38;5;0m    COMPILATION:    \e[0m \n"
	@$(target) -c ./test/$(test).pop -o $(test).asm --debug
	@printf "\n\e[48;5;88m    ASSEMBLER CODE:    \e[0m\n"
	@cat -n $(test).asm
	@nasm $(test).asm -felf64 -o $(test).o && ld $(test).o -o $(test)
	@printf "\n\e[48;5;129m\e[38;5;0m    ORIGINAL CODE:    \e[0m \n"
	@cat -n ./test/$(test).pop
	@printf "\n\e[48;5;136m\e[38;5;0m    EXECUTION:    \e[0m\n"
	@./$(test)
	@rm ./$(test)*

record_tests:
	@$(eval tests := $(shell find test/ -name "*.pop" | sed 's#.*/\([^/]*\)\..*#\1#'))
	echo $(tests)
	for tst in $(tests); do\
		echo $${tst} ;\
		$(target) -c ./test/$${tst}.pop -o $${tst}.asm >> /dev/null; \
		nasm $${tst}.asm -felf64 -o $${tst}.o && ld $${tst}.o -o $${tst}; \
		echo "OUTPUT:" > test/$${tst}.out; \
		./$${tst} >> test/$${tst}.out; \
		rm ./$${tst}*; \
	done;

clean:
		@rm $(objs)

debug:
	gcc -g $(src)* -o $(target)_d

run: 
	$(target)
