# Outline
This is test program using the nonogram solver class.

# Usage
## Input
The first line of input contains r and c, where r and c are the number of rows and columns of the table.

The next r and c lines contain the clue of the rows and columns of the nonogram.

All values are separated with spaces and each line does not contain any empty line.

## Output
If the clue is invalid or impossible to solve, nothing is printed.

If the class found solutions, regardless of the number of them, only one solved table is printed with black as '#' and white as '.'.

## Examples
### Nonogram
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

### Input
```
8 5
0
0
2 1
1 1
1 1
1 1
2 1
0
3
1 1
1 1
3
1 1
```

### Output
```
. . . . . 
. . . . . 
. # # . # 
# . . # . 
# . . # . 
# . . # . 
. # # . # 
. . . . . 

```

# Feature
`main.cpp` gives only one input to the class, but it can solve several nonograms in one run.

Also, it is expected to solve a line in quadratic time.
