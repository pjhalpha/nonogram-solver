#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 50
#define MAX_HINT_SIZE 20
#define MAX_HINT_LINE_STRING_SIZE MAX_HINT_SIZE * 5

int table[MAX_TABLE_SIZE + 1][MAX_TABLE_SIZE + 1], table_size[2];
int hint[2][MAX_TABLE_SIZE][MAX_HINT_SIZE + 1];
int line[MAX_TABLE_SIZE], line_size, line_full;
int line_hint[MAX_TABLE_SIZE], line_hint_size, line_hint_sum, line_hint_loc;
int new_line[MAX_TABLE_SIZE], new_line_case[MAX_TABLE_SIZE], new_line_check;

void solve(int table_remain);
void solveLine(int line_hint_sum, int line_hint_loc, int line_case_loc);
void printTable(void);

void solve(int table_remain) {
    int table_row, table_col;
    int table_changes = 0;

    // table[][MAX_TABLE_SIZE] [MAX_TABLE_SIZE][] 0: This line has blanks, 1: This line is full
    line_size = table_size[1];
    for (table_row = 0; table_row < table_size[0]; table_row++) {
        if (table[table_row][MAX_TABLE_SIZE]) {
            continue;
        }

        new_line_check = 0;
        for (table_col = 0; table_col < line_size; table_col++) {
            line[table_col] = table[table_row][table_col];
            new_line[table_col] = 0;
            new_line_case[table_col] = 0;
        }
        
        line_hint_size = hint[0][table_row][MAX_HINT_SIZE];
        line_hint_sum = 0;
        for (line_hint_loc = 0; line_hint_loc < line_hint_size; line_hint_loc++) {
            line_hint[line_hint_loc] = hint[0][table_row][line_hint_loc];
            line_hint_sum += line_hint[line_hint_loc];
        }
        
        solveLine(line_hint_sum, 0, -1);

        if (!new_line_check) {
            return;
        }

        line_full = 1;
        for (table_col = 0; table_col < line_size; table_col++) {
            if (new_line[table_col] != line[table_col]) {
                table[table_row][table_col] = new_line[table_col];
                table_changes++;
            }

            line_full = line_full && (new_line[table_col] != 0);
        }

        if (line_full) {
            table[table_row][MAX_TABLE_SIZE] = 1;
        }
    }
    line_size = table_size[0];
    for (table_col = 0; table_col < table_size[1]; table_col++) {
        if (table[MAX_TABLE_SIZE][table_col]) {
            continue;
        }
        
        new_line_check = 0;
        for (table_row = 0; table_row < line_size; table_row++) {
            line[table_row] = table[table_row][table_col];
            new_line[table_row] = 0;
            new_line_case[table_row] = 0;
        }
        
        line_hint_size = hint[1][table_col][MAX_HINT_SIZE];
        line_hint_sum = 0;
        for (line_hint_loc = 0; line_hint_loc < line_hint_size; line_hint_loc++) {
            line_hint[line_hint_loc] = hint[1][table_col][line_hint_loc];
            line_hint_sum += line_hint[line_hint_loc];
        }
        
        solveLine(line_hint_sum, 0, -1);

        if (!new_line_check) {
            return;
        }

        line_full = 1;
        for (table_row = 0; table_row < line_size; table_row++) {
            if (new_line[table_row] != line[table_row]) {
                table[table_row][table_col] = new_line[table_row];
                table_changes++;
            }
            
            line_full = line_full && (new_line[table_row] != 0);
        }

        if (line_full) {
            table[MAX_TABLE_SIZE][table_col] = 1;
        }
    }

    table_remain -= table_changes;
    if (table_remain == 0) {
        printTable();

        return;
    } else if (table_changes) {
        solve(table_remain);
    }

    return;
}

void solveLine(int line_hint_sum, int line_hint_loc, int line_case_loc) {
    if (line_case_loc == -1) {
        if (line[0] != -1) {
            solveLine(line_hint_sum, 0, 0);
        }
        for (line_case_loc = 0; (line_case_loc < line_size - line_hint_size - line_hint_sum + 1) && (line[line_case_loc] != 1); line_case_loc++) {
            new_line_case[line_case_loc] = -1;
            
            solveLine(line_hint_sum, 0, line_case_loc + 1);
        }
    } else {
        int line_case_loc_next;

        line_case_loc_next = line_case_loc + line_hint[line_hint_loc];
        for (; line_case_loc < line_case_loc_next; line_case_loc++) {
            if (line[line_case_loc] == -1) {
                return;
            }

            new_line_case[line_case_loc] = 1;
        }
        line_hint_sum -= line_hint[line_hint_loc++];

        if (line_hint_loc < line_hint_size) {
            line_case_loc_next = line_size - line_hint_size - line_hint_sum + line_hint_loc + 1;
            for (; line_case_loc < line_case_loc_next; line_case_loc++) {
                if (line[line_case_loc] == 1) {
                    return;
                }

                new_line_case[line_case_loc] = -1;

                solveLine(line_hint_sum, line_hint_loc, line_case_loc + 1);
            }
        } else {  
            for (; line_case_loc < line_size; line_case_loc++) {
                if (line[line_case_loc] == 1) {
                    return;
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
        }
    }
}

void printTable(void) {
    int table_row, table_col;

    for (table_row = 0; table_row < table_size[0]; table_row++) {
        for (table_col = 0; table_col < table_size[1]; table_col++) {
            switch (table[table_row][table_col]) {
                case -1:
                    printf("%2c", '.');

                    break;
                
                case 0:
                    printf("%2c", '-');

                    break;

                case 1:
                    printf("%2c", '#');

                    break;
            }
        }
        printf("\n");
    }
}

int main(void) {
    int direction;
    char line_hint_string[MAX_HINT_LINE_STRING_SIZE], *line_hint_string_ptr;

    scanf("%d %d%*c", &table_size[0], &table_size[1]);
    for (direction = 0; direction < 2; direction ++) {
        for (line_size = 0; line_size < table_size[direction]; line_size++) {
            scanf("%[^\n]%*c", line_hint_string);
            line_hint_string_ptr = strtok(line_hint_string, " ");
            
            for (line_hint_size = 0; (line_hint_size < MAX_HINT_SIZE) && (line_hint_string_ptr != NULL); line_hint_size++)
            {
                hint[direction][line_size][line_hint_size] = atoi(line_hint_string_ptr);
                line_hint_string_ptr = strtok(NULL, " ");
            }
            hint[direction][line_size][MAX_HINT_SIZE] = line_hint_size;
        }
    }

    solve(table_size[0] * table_size[1]);
    
    return 0;
}