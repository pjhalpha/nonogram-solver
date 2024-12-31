# Outline
The example CLI program and page with wasm using the nonogram solver library.

Check out RAEDME.md contained in each folder.

# Note
The maximum size of a nonogram is 255 × 255, which is defined as `Nonogram::MAX_SIZE` in the `nonogram.hpp` file.

If the nonogram looks like this:
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
then, the `clue_arr` parameter for `Nonogram::Clue::init` should be:
```
{8, 5, 1, 0, 1, 0, 2, 2, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 0, 1, 3, 2, 1, 1, 2, 1, 1, 1, 3, 2, 1, 1}
```

You can inherit from `Nonogram::Solver` and override the methods `Nonogram::Solver::wrap` and `Nonogram::Solver::inspect` to customize the solving process, such as visualizing the steps or specifying the number of solutions to find.
