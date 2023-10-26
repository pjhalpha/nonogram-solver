# Usage
```
nonogram-solver <-i <input file>> [-o <output file>] [-ins <time intervals>]

    -i <input file>         Specify the path of the input file.
    -o <output file>        Specify the path of the output file, otherwise ".out" is appended after the input path.
    -ins <time intervals>   Print the process of solving the nonogram at specified time intervals. (Unused feature)
```

## Input File
The first line of input contains R and C, where R and C are the number of rows and columns of the table.

The next R and C lines contain the clue of the rows and columns of the clue.

All values are separated with spaces and each line does not contain any empty line.

## Output File
If the program found solutions, regardless of the number of them, only one solved table is printed with black as '#' and white as '.'.

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
