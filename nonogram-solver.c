#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 100
#define MAX_CLUE_SIZE 30
#define MAX_CLUE_LINE_STRING_SIZE MAX_CLUE_SIZE * 5

struct TableList {
    int table[MAX_TABLE_SIZE + 1][MAX_TABLE_SIZE + 1];
    struct TableList *prev_table;
} table_struct = {{0, }, NULL}, *table_struct_ptr = &table_struct;
int table_size[2];
int clue[2][MAX_TABLE_SIZE][MAX_CLUE_SIZE + 1];
int line[MAX_TABLE_SIZE], line_size;
int line_clue[MAX_TABLE_SIZE], line_clue_size, line_clue_check[MAX_TABLE_SIZE];
int new_line[MAX_TABLE_SIZE], new_line_case[MAX_TABLE_SIZE], new_line_check;
int guess;

int solve(int table_remain);
int solveLine(int line_clue_sum, int line_clue_loc, int line_case_loc);
int guessLine(int g_line[], int line_size, int g_line_clue[], int g_line_clue_size, int line_clue_sum, int line_clue_loc, int g_new_line[], int g_new_line_loc, int table_row, int table_remain);
void printTable(void);

int solve(int table_remain) {
    int table_row, table_col;
    int table_changes = 0;
    int line_clue_sum, line_clue_loc;
    int line_full;

    // return 0: This table is the soultion, 1: This table is impossible
    // table_struct_ptr->table[][MAX_TABLE_SIZE] [MAX_TABLE_SIZE][] 0: This line may have more determined changes, 1: This line has no more determined change, 2: This line is full

    line_size = table_size[1];
    for (table_row = 0; table_row < table_size[0]; table_row++) {
        if (table_struct_ptr->table[table_row][MAX_TABLE_SIZE] >= 1) {
            continue;
        }

        new_line_check = 0;
        for (table_col = 0; table_col < line_size; table_col++) {
            line[table_col] = table_struct_ptr->table[table_row][table_col];
            new_line[table_col] = 0;
            new_line_case[table_col] = 0;
        }
        
        line_clue_size = clue[0][table_row][MAX_CLUE_SIZE];
        line_clue_sum = 0;
        for (line_clue_loc = 0; line_clue_loc < line_clue_size; line_clue_loc++) {
            line_clue[line_clue_loc] = clue[0][table_row][line_clue_loc];
            line_clue_sum += line_clue[line_clue_loc];
            line_clue_check[line_clue_loc] = 0;
        }
        
        if (solveLine(line_clue_sum, 0, -1) == 1) {
            table_struct_ptr->table[table_row][MAX_TABLE_SIZE] = 1;

            continue;   
        }

        if (!new_line_check) {
            return 1;
        }

        line_full = 1;
        for (table_col = 0; table_col < line_size; table_col++) {
            if (new_line[table_col] != line[table_col]) {
                table_struct_ptr->table[table_row][table_col] = new_line[table_col];
                table_struct_ptr->table[MAX_TABLE_SIZE][table_col] = 0;
                table_changes++;
            }

            line_full = line_full && (new_line[table_col] != 0);
        }

        if (line_full) {
            table_struct_ptr->table[table_row][MAX_TABLE_SIZE] = 2;
        } else {
            table_struct_ptr->table[table_row][MAX_TABLE_SIZE] = 1;
        }
    }
    line_size = table_size[0];
    for (table_col = 0; table_col < table_size[1]; table_col++) {
        if (table_struct_ptr->table[MAX_TABLE_SIZE][table_col] >= 1) {
            continue;
        }
        
        new_line_check = 0;
        for (table_row = 0; table_row < line_size; table_row++) {
            line[table_row] = table_struct_ptr->table[table_row][table_col];
            new_line[table_row] = 0;
            new_line_case[table_row] = 0;
        }
        
        line_clue_size = clue[1][table_col][MAX_CLUE_SIZE];
        line_clue_sum = 0;
        for (line_clue_loc = 0; line_clue_loc < line_clue_size; line_clue_loc++) {
            line_clue[line_clue_loc] = clue[1][table_col][line_clue_loc];
            line_clue_sum += line_clue[line_clue_loc];
            line_clue_check[line_clue_loc] = 0;
        }
        
        if (solveLine(line_clue_sum, 0, -1) == 1) {
            table_struct_ptr->table[MAX_TABLE_SIZE][table_col] = 1;

            continue;
        }

        if (!new_line_check) {
            return 1;
        }

        line_full = 1;
        for (table_row = 0; table_row < line_size; table_row++) {
            if (new_line[table_row] != line[table_row]) {
                table_struct_ptr->table[table_row][table_col] = new_line[table_row];
                table_struct_ptr->table[table_row][MAX_TABLE_SIZE] = 0;
                table_changes++;
            }
            
            line_full = line_full && (new_line[table_row] != 0);
        }

        if (line_full) {
            table_struct_ptr->table[MAX_TABLE_SIZE][table_col] = 2;
        } else {
            table_struct_ptr->table[MAX_TABLE_SIZE][table_col] = 1;
        }
    }
    
    table_remain -= table_changes;
    if (table_remain == 0) {
        printTable();

        return 0;
    } else if (table_changes) {
        return solve(table_remain);
    } else {
        int g_line[MAX_TABLE_SIZE], g_line_size;
        int g_line_clue[MAX_TABLE_SIZE], g_line_clue_size;
        int g_new_line[MAX_TABLE_SIZE];

        guess = 1;

        g_line_size = table_size[1];
        for (table_row = 0; table_row < table_size[0]; table_row++) {
            if (table_struct_ptr->table[table_row][MAX_TABLE_SIZE] == 2) {
                continue;
            }

            table_struct_ptr->table[table_row][MAX_TABLE_SIZE] = 2;

            for (table_col = 0; table_col < g_line_size; table_col++) {
                g_line[table_col] = table_struct_ptr->table[table_row][table_col];
                g_new_line[table_col] = 0;

                if (g_line[table_col] == 0) {
                    table_remain--;
                }
            }
            
            g_line_clue_size = clue[0][table_row][MAX_CLUE_SIZE];
            line_clue_sum = 0;
            for (line_clue_loc = 0; line_clue_loc < g_line_clue_size; line_clue_loc++) {
                g_line_clue[line_clue_loc] = clue[0][table_row][line_clue_loc];
                line_clue_sum += g_line_clue[line_clue_loc];
            }
            
            return guessLine(g_line, g_line_size, g_line_clue, g_line_clue_size, line_clue_sum, 0, g_new_line, -1, table_row, table_remain);
        }
    }
}

int solveLine(int line_clue_sum, int line_clue_loc, int line_case_loc) {
    if (line_case_loc == -1) {
        if (line[0] != -1) {
            solveLine(line_clue_sum, 0, 0);
        }
        for (line_case_loc = 0; (line_case_loc < line_size - line_clue_size - line_clue_sum + 1) && (line[line_case_loc] != 1); line_case_loc++) {
            new_line_case[line_case_loc] = -1;
            
            solveLine(line_clue_sum, 0, line_case_loc + 1);
        }

        // return 0: This line case has definitive changes, 1: This line case has no definitive change
        
        for (line_case_loc = 0; line_case_loc < line_size; line_case_loc++) {
            if (new_line[line_case_loc] != line[line_case_loc]) {
                return 0;
            }
        }

        return 1;
    } else {
        // return 0: This line case is possible, 1: This line case is impossible

        int line_case_loc_next;

        line_case_loc_next = line_case_loc + line_clue[line_clue_loc];
        for (; line_case_loc < line_case_loc_next; line_case_loc++) {
            if (line[line_case_loc] == -1) {
                return 1;
            }

            new_line_case[line_case_loc] = 1;
        }
        line_clue_sum -= line_clue[line_clue_loc++];

        if (line_clue_loc < line_clue_size) {
            line_case_loc_next = line_size - line_clue_size - line_clue_sum + line_clue_loc + 1;
            for (; line_case_loc < line_case_loc_next; line_case_loc++) {
                if (line[line_case_loc] == 1) {
                    return 1;
                }

                new_line_case[line_case_loc] = -1;

                if ((solveLine(line_clue_sum, line_clue_loc, line_case_loc + 1) == 0) && (line_clue_check[line_clue_loc])) {
                    return 0;
                }
            }

            line_clue_check[line_clue_loc] = 1;

            return 0;
        } else {  
            for (; line_case_loc < line_size; line_case_loc++) {
                if (line[line_case_loc] == 1) {
                    return 1;
                }
                
                new_line_case[line_case_loc] = -1;
            }

            if (!new_line_check) {
                new_line_check = 1;

                for (line_case_loc = 0; line_case_loc < line_size; line_case_loc++) {
                    new_line[line_case_loc] = new_line_case[line_case_loc];
                }
            } else {
                for (line_case_loc = 0; line_case_loc < line_size; line_case_loc++) {
                    new_line[line_case_loc] = (new_line[line_case_loc] + new_line_case[line_case_loc]) / 2;
                }
            }

            return 0;
        }
    }
}

int guessLine(int g_line[], int g_line_size, int g_line_clue[], int g_line_clue_size, int line_clue_sum, int line_clue_loc, int g_new_line[], int g_new_line_loc, int table_row, int table_remain) {
    if (g_new_line_loc == -1) {
        // return 0: This guessed table is the solution, 1: This guessed table is impossible

        if (g_line[0] != -1) {
            if (guessLine(g_line, g_line_size, g_line_clue, g_line_clue_size, line_clue_sum, 0, g_new_line, 0, table_row, table_remain) == 0) {
                return 0;
            }
        }
        for (g_new_line_loc = 0; (g_new_line_loc < g_line_size - g_line_clue_size - line_clue_sum + 1) && (g_line[g_new_line_loc] != 1); g_new_line_loc++) {
            g_new_line[g_new_line_loc] = -1;
            
            if (guessLine(g_line, g_line_size, g_line_clue, g_line_clue_size, line_clue_sum, 0, g_new_line, g_new_line_loc + 1, table_row, table_remain) == 0) {
                return 0;
            }
        }

        return 1;
    } else {
        // return 0: This guessed line case is the solution, 1: This guessed line case is impossible

        int line_case_loc_next;

        line_case_loc_next = g_new_line_loc + g_line_clue[line_clue_loc];
        for (; g_new_line_loc < line_case_loc_next; g_new_line_loc++) {
            if (g_line[g_new_line_loc] == -1) {
                return 1;
            }

            g_new_line[g_new_line_loc] = 1;
        }
        line_clue_sum -= g_line_clue[line_clue_loc++];

        if (line_clue_loc < g_line_clue_size) {
            line_case_loc_next = g_line_size - g_line_clue_size - line_clue_sum + line_clue_loc + 1;
            for (; g_new_line_loc < line_case_loc_next; g_new_line_loc++) {
                if (g_line[g_new_line_loc] == 1) {
                    return 1;
                }

                g_new_line[g_new_line_loc] = -1;

                if (guessLine(g_line, g_line_size, g_line_clue, g_line_clue_size, line_clue_sum, line_clue_loc, g_new_line, g_new_line_loc + 1, table_row, table_remain) == 0) {
                    return 0;
                }
            }

            return 1;
        } else {
            for (; g_new_line_loc < g_line_size; g_new_line_loc++) {
                if (g_line[g_new_line_loc] == 1) {
                    return 1;
                }
                
                g_new_line[g_new_line_loc] = -1;
            }
            
            int g_table_row, g_table_col;

            struct TableList *g_table_struct_ptr = malloc(sizeof(struct TableList));
            g_table_struct_ptr->prev_table = table_struct_ptr;
            for (g_table_row = 0; g_table_row < table_size[0]; g_table_row++) {
                for (g_table_col = 0; g_table_col < table_size[1]; g_table_col++) {
                    g_table_struct_ptr->table[g_table_row][g_table_col] = table_struct_ptr->table[g_table_row][g_table_col];
                }
            
                g_table_struct_ptr->table[g_table_row][MAX_TABLE_SIZE] = table_struct_ptr->table[g_table_row][MAX_TABLE_SIZE];
            }
            for (g_table_col = 0; g_table_col < table_size[1]; g_table_col++) {
                if (table_struct_ptr->table[MAX_TABLE_SIZE][g_table_col] == 2) {
                    g_table_struct_ptr->table[MAX_TABLE_SIZE][g_table_col] = 2;
                } else {
                    g_table_struct_ptr->table[MAX_TABLE_SIZE][g_table_col] = 0;
                }
            }
            
            for (g_new_line_loc = 0; g_new_line_loc < g_line_size; g_new_line_loc++) {
                g_table_struct_ptr->table[table_row][g_new_line_loc] = g_new_line[g_new_line_loc];
            }

            table_struct_ptr = g_table_struct_ptr;

            int g_new_line_success = solve(table_remain);

            table_struct_ptr = table_struct_ptr->prev_table;
            free(g_table_struct_ptr);

            return g_new_line_success;
        }
    }
}

void printTable(void) {
    int table_row, table_col;

    for (table_row = 0; table_row < table_size[0]; table_row++) {
        for (table_col = 0; table_col < table_size[1]; table_col++) {
            switch (table_struct_ptr->table[table_row][table_col]) {
                case -1:
                    printf("%2c", '.');

                    break;
                case 0:
                    printf("%2c", '-');

                    break;
                case 1:
                    if (guess == 0) {
                        printf("%2c", '#');

                        break;
                    }

                    printf("%2c", '$');

                    break;
            }
        }
        printf("\n");
    }
}

int main(void) {
    int direction;
    char line_clue_string[MAX_CLUE_LINE_STRING_SIZE], *line_clue_string_ptr;

    scanf("%d %d%*c", &table_size[0], &table_size[1]);
    for (direction = 0; direction < 2; direction ++) {
        for (line_size = 0; line_size < table_size[direction]; line_size++) {
            scanf("%[^\n]%*c", line_clue_string);
            line_clue_string_ptr = strtok(line_clue_string, " ");
            
            for (line_clue_size = 0; (line_clue_size < MAX_CLUE_SIZE) && (line_clue_string_ptr != NULL); line_clue_size++)
            {
                clue[direction][line_size][line_clue_size] = atoi(line_clue_string_ptr);
                line_clue_string_ptr = strtok(NULL, " ");
            }
            clue[direction][line_size][MAX_CLUE_SIZE] = line_clue_size;
        }
    }

    solve(table_size[0] * table_size[1]);

    return 0;
}
