# Build
```
g++ nonogram.hpp clue.cpp solver.cpp linesolver.cpp cli.cpp -O3 -o nonogram-solver
```

# Usage
```
nonogram-solver <-i <input file>> [-o <output file>] [-m <max count>]

    -i <input file>
        Specify the path of the input file.

    -o <output file>
        Specify the path of the output file.
        Otherwise, ".out" is appended after the input path.

    -m <maximum count>
        Specify the maximum count of solutions to be found.
        Set this option as negative value to find all solutions.
        Otherwise, only one solution could be found.
```
You can also see this message by running the program without any options.

## Input File
The first line of input contains R and C, where R and C are the number of rows and columns of the table.

The next R and C lines contain the clue of the rows and columns of the clue.

All values are separated with spaces and each line does not contain any empty line.

## Output File
The specified maximum count or less of solved table is printed with black as '#' and white as '.'.

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

## Input File
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

## Output File
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
