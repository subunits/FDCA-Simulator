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
