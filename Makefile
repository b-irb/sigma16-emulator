# compiler flags
CFLAGS := -O2 -flto -DDUMP_CPU

# object files
OBJ := src/main.o src/vm.o src/cpu.o src/tracing.o

.PHONY: all
all: sigma16

sigma16: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	find -name "*.o" -delete
	rm -f sigma16
