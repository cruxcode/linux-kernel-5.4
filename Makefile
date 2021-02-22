# DO NOT TOUCH THIS
ifeq ($(platform), emulator)
	CC := $(HOME)/./Android/Sdk/ndk/20.0.5594570/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android29-clang
endif

# NOTE:
#   Use $(CC) instead of compiler name (e.g. gcc, clang).
#   Since your user-space program is simple, $(CC) is able to handle
#   everything. Do not use other executables for compiling/linking.

# ---------- Write the rest of Makefile below ----------

#make file - this is a comment section
TARGET2=six_states #target file name
TARGET3=test_with_arg #target file name
TARGET5 = test_chronology #target file name
all:
	$(CC) -Wall six_states.c -g -o $(TARGET2)
	$(CC) -Wall test_with_arg.c -g -o $(TARGET3)
	$(CC) -Wall test_chronology.c -g -o $(TARGET5)

clean:
	rm $(TARGET)
