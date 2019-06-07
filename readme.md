# Nx 2

The original Nx emulator started getting bloated and some architectural mistakes were made.  I have
decided to write the emulator from scratch, copying code over as I go.  This will be leaner and meaner
than the previous one.  All configuration will be done via 3 places:

* The command line
* A `nx.cfg` file placed right next to the `nx.exe` executable.
* A telnet server

Eventually, I will add other elements that the original had, such as a debugger, editor, assembler and disassembler
but not before I make sure I nail down the emulation perfectly.

Also this emulator will focus solely on the ZX Spectrum Next.

## Building

To build this requires Visual Studio 2017 to be installed.  Just run `b.bat` to build the debug version or `br.bat` for the
release one.  The executables will be found in the newly generated `_bin` directory.

