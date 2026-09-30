# SimpCalc Scanner and Parser

## Authors
- Keith Ayeras
- Elijem Timothy Jaso
- Dave Predigua

## Description
This project implements a lexical analyzer (scanner) and a recursive-descent parser for the SimpCalc language.
The `main.c` driver program processes all `_input*.txt` files in its directory, producing both scanner and parser outputs.

## Build Instructions
1. Ensure you have `gcc` installed on your system.
2. Open a terminal and navigate to the project directory.
3. Run the following command to compile the project:
   ```bash
   gcc -Wall -o simpcalc main.c scanner.c parser.c
   ```

## Run Instructions
To run the scanner and parser on all input files:
1. Ensure your `sample_input_*.txt` files are in the same directory as the `simpcalc` executable.
2. Run the program:
   - On Windows: `.\simpcalc.exe`
   - On Linux/Mac: `./simpcalc`
3. The program will automatically read each `_input` file and generate the corresponding `_output_scan` and `_output_parse` files.

## Testing
We have included a `test.ps1` script for Windows PowerShell users to automate building and verifying the outputs against expected outputs.
To run tests:
```powershell
.\test.ps1
```
