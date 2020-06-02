# V5-MIPS-Assembler-Emulator
A custom MIPS Emulator, Assembler, and Linker. Support for about half of the instructions. Written in C++, interfaces with Java for the GUI via the java native interface.

## History
This project was written December of 2018 over winter break to prepare me for my MIPS class (ICSI 333) in college. I decided on this project because I figured it would be a good introduction to the MIPS language and over summer 2018 I had played around and written a small x86 emulator in Javascript before following it up with a small x86 assembler in C++. 

## Running The Project
At the time of writing this I was compiling using NppExec scripts in NotePad++, I was able to find my exact script which used gcc as well as Java 10:
```
npp_save
cd $(CURRENT_DIRECTORY)
g++  -O4  -o $(NAME_PART).exe $(FILE_NAME) -I"C:\Program Files\Java\jdk-10.0.2\include" -I"C:\Program Files\Java\jdk-10.0.2\include\win32" -L"C:\Program Files\Java\jdk-10.0.2\lib" -ljvm
cmd /k (start $(CURRENT_DIRECTORY)\$(NAME_PART).exe) & exit 
```
Hopefully someday I’ll get around to cleaning this project up and making a nice CMake script for it. 

To use this project, the main file to look at is Emulator.cpp, the first line of main is 
```cpp 
vector<string> files = {"asm/header.asm", "asm/print.asm","asm/graphics.asm", "asm/gt.asm"};
```
These are the MIPS assembly files that will be assembled, linked, and run on the emulator. If you have any MIPS code you would like to run, you can put it here, But please note again, This project leaves out many of the instructions and should not be used for anything other than a proof of concept. 

## Notes
I added a few custom syscalls for things like setting pixel colors so I would be able to create some graphics programs, everything else conforms to the MARS / SPIM syscall standards, and my custom syscalls were added in syscall slots that aren’t used by either of the other emulators.

