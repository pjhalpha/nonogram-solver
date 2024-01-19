# Build
```
em++ clue.cpp solver.cpp linesolver.cpp wasm.cpp -sEXPORTED_FUNCTIONS=['_getInputPtr','_getOutputPtr','_setWrap','_setInspect','_setFail','_getRemain','_getCount','_getTime','_solve'] -sEXPORTED_RUNTIME_METHODS=['addFunction'] -sEXPORT_ES6=1 -sMODULARIZE -sALLOW_TABLE_GROWTH -sEXPORT_NAME='createModule' -O3 -o nonogram-solver.js
```

# Usage
Click `solve` to solve the nonogram and immediately draw the output.

Click `inspect` to show the solving process.

`interval` input specifys the time interval (milliseconds) beteween the solving processes.

`row`, `columnns`, `row clues`, and `column clues` inputs specify the clue of the nonogram.

Only one solution will be printed if the solver found more than one solutions.

# Example
## Nonogram
```
    │   1 1   1
    │ 3 1 1 3 1
─ ─ ┼ ─ ─ ─ ─ ─
  0 │ □ □ □ □ □
  0 │ □ □ □ □ □
2 1 │ □ ■ ■ □ ■
1 1 │ ■ □ □ ■ □
1 1 │ ■ □ □ ■ □
1 1 │ ■ □ □ ■ □
2 1 │ □ ■ ■ □ ■
  0 │ □ □ □ □ □
```

## Input
row
```
8
```

column
```
5
```

row clues
```
0
0
2 1
1 1
1 1
1 1
2 1
0
```

column clues
```
3
1 1
1 1
3
1 1
```
