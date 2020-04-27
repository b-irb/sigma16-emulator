# compiler flags
CFLAGS := -O2 -flto

# object files
OBJ := src/main.o src/tracing.o src/vm.o

.PHONY: all
all: sigma16-emu

sigma16-emu: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	find -name "*.o" -delete
	rm -f sigma16-emu
