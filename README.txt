FDCA Simulator README

Overview:
- 1D Finite Deterministic Cellular Automaton (Wolfram rules)
- Console-based, interactive, cross-platform

Features:
- Interactive display with colors (# green, . gray)
- Step numbers
- Dynamic history window: [ and ]
- Speed adjustment: + / -
- Pause/resume: p
- Quit: q
- Export options:
  s → visible text
  c → visible CSV
  f → full CSV
  t → transposed CSV

Compilation:
gcc fdca.c -o fdca
./fdca  (Linux/macOS)
fdca.exe  (Windows)

Controls Quick Reference:
+ : Increase speed
- : Decrease speed
p : Pause / Resume
[ : Decrease window
] : Increase window
s : Save visible text
c : Save visible CSV
f : Save full CSV
t : Save transposed CSV
q : Quit

Output Examples:
Text (s):
 0 ....................
 1 ....................
 2 ..........#.........

CSV (c/f):
0,0,0,0,1,0,...
1,0,0,1,1,1,...

Transposed CSV (t):
Cell\Step,0,1,2,3
0,0,0,0,1
1,0,0,1,1

FDCA Simulator Dependencies

Required:
- Standard C compiler:
    - gcc (Linux/macOS/Windows via MinGW)
    - clang (Linux/macOS)
    - Visual Studio C compiler (Windows)
- Standard C libraries:
    - stdio.h
    - stdlib.h
    - time.h

Optional / Platform-specific:
- Windows:
    - conio.h (for _kbhit() and _getch())
    - windows.h (for Sleep())
- Linux/macOS:
    - unistd.h (for usleep())
    - termios.h and sys/select.h (for non-blocking keyboard input)

Notes:
- The program does not require any external libraries or packages.
- ANSI color output may not display correctly on Windows consoles prior to Windows 10.
- Everything needed to compile and run is included in this package.


