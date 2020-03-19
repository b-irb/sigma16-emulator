# compiler flags
CFLAGS := -O2 -flto -DENABLE_TRACE

# object files
OBJ := src/emulator/main.o src/emulator/tracing.o src/emulator/vm.o

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
