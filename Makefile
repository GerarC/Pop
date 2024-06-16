flags=-Wall -Wextra
libs=-lbsd
obj=gcc -c 
out=gcc -o 
name=pop
src=./src/
icd=./include/
build=./build/
target=$(build)$(name)

objs=$(src)main.o \
	 $(src)common.o \
	 $(src)log.o \
	 $(src)file.o \
	 $(src)lexer.o \
	 $(src)parser.o  \

all: compile clean

compile: $(objs)
	@[ -d "$(build)" ] || { mkdir "$(build)"; }
	$(out)$(target) $(objs) $(flags) $(libs)

example%:
	$(target) $@.txt

clean:
		rm $(objs)

run: 
	$(target)
