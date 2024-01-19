# Outline
The example CLI program and web with wasm using the nonogram solver library.

Check out RAEDME.md contained in each folder.

# Note
The maximum size of nonogram is 255 × 255 now but you can edit it by changing `Nonogram::MAX_SIZE`.

`solve` function in `Nonogram::Sovler` takes one unsigned integer array that first two elements are number of rows and columns of the table, and the others are length of a line clue and it.
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
```
{8, 5, 1, 0, 1, 0, 2, 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 0, 1, 3, 2, 1, 1, 2, 1, 1, 1, 3, 2, 1, 1}
```
