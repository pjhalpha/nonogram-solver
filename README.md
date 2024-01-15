# Usage
Without setting any options, you can see follow message:
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

# Note
CLI is an just example of using the nonogram solver class.

Include the header, create a solver instance, call its `solve` function and use it anywhere you want.

`solve` function takes one unsigned integer array that first two elements are number of rows and columns of the table, and the others are length of a line clue and it.

Given an example above, the array is as follows:
```
[8, 5, 1, 0, 1, 0, 2, 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 0, 1, 3, 2, 1, 1, 2, 1, 1, 1, 3, 2, 1, 1]
```
