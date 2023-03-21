# Linux shell guesser

Dumb thing I wrote which tries to guess if a program 
on Linux is running in a terminal.

The usual isatty check works well enough for most cases, but I wanted to
see if I could handle some edge cases (i.e. if i/o is being redirected, 
if running from a script, etc). There are some edge cases I probably 
haven't handled.

## Dependencies

Needs GTK to display the guess in non-terminals. 

## Building

* mkdir build
* cd build 
* cmake ..
