
#Compiler to use for the make
cc=clang++
#Location to store object files
DO=obj
#Directory for main binaries
DB=bin
#Directory where source files are
DS=src
#Directory where docs are stored
DD=doc

#Compiler flags to use for debugging
FD=-Wall -g
#Compiler flags to use for object files
FO=-c 
#Compiler Flags to use for binaries
FB=-framework SDL2 

#Tarball output file
TAR_FILE=chip8.tar.gz

################################################
# Build Commands
################################################

all: prep chip8

#Remove any previously built files
clean:
	#Remove any objects from the object directory
	rm -rf $(DO)

#Removes any files other thatn the source from the directory
purge: clean
	#Remove any binaries from the output directory
	rm -rf $(DB)
	#Remove the source tarball if it exists
	rm -rf $(TAR_FILE)
	#Remove the documentation files
	rm -rf $(DD)

#Create the directory structure required for output
prep:
	#Create the objects directory
	mkdir -p $(DO)
	#Create output directory
	mkdir -p $(DB)

documentation:
	#Generating documentaton
	doxygen Doxyfile

#Tar the project to make it easier to move around
tarball:
	tar -zcvf $(TAR_FILE) Doxyfile Makefile src

################################################
# Executable Binaries
################################################

#Build CHIP8 Emulator executable
chip8: prep driver.o sdl.o font_set.o chip8.o emulator.o
	#Building and linking the Emulator binary
	$(cc) $(FB) -o $(DB)/$@ $(DO)/driver.o $(DO)/sdl.o $(DO)/font_set.o $(DO)/chip8.o $(DO)/emulator.o

################################################
# Object Files
################################################

driver.o: $(DS)/driver.cpp
	#Compiling driver object
	$(cc) $(FO) -o $(DO)/$@ $^

sdl.o: $(DS)/sdl.cpp
	# Compiling sdl input output wrapper object
	$(cc) $(FO) -o $(DO)/$@ $^

font_set.o: $(DS)/font_set.cpp
	# Compiling font set
	$(cc) $(FO) -o $(DO)/$@ $^

chip8.o: $(DS)/chip8.cpp
	# Compiling CPU object
	$(cc) $(FO) -o $(DO)/$@ $^

emulator.o: $(DS)/emulator.cpp
	# Compiling emulator object
	$(cc) $(FO) -o $(DO)/$@ $^

