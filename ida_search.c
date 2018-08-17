
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "uthash.h"
#include "rotate_xxx.h"
#include "ida_search_core.h"
#include "ida_search_555.h"


// To compile:
//  gcc -O3 -o ida_search ida_search_core.c ida_search.c rotate_xxx.c ida_search_555.c -lm
//
//  gcc -ggdb -o ida_search ida_search.c rotate_xxx.c ida_search_core.c ida_search_555.c -lm

// scratchpads that we do not want to allocate over and over again
char *sp_cube_state;
unsigned long array_size;
unsigned long ida_count;
unsigned long ida_count_total;
unsigned long seek_calls = 0;

// Supported IDA searches
typedef enum {
    NONE,
    UD_CENTERS_STAGE_555,
} lookup_table_type;


struct key_value_pair *ida_explored = NULL;
struct key_value_pair *UD_centers_555 = NULL;
char *pt_t_centers_cost_only = NULL;
char *pt_x_centers_cost_only = NULL;


/* Remove leading and trailing whitespaces */
char *
strstrip (char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    // Removing trailing whitespaces
    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    // Remove leading whitespaces
    // The lookup table files do not have any leading whitespaces so commenting this out to save a few CPU cycles
    //while (*s && isspace(*s))
    //    s++;

    return s;
}


/* The file must be sorted and all lines must be the same width */
unsigned long
file_binary_search (char *filename, char *state_to_find, int statewidth, unsigned long linecount, int linewidth)
{
    FILE *fh_read = NULL;
    unsigned long first = 0;
    unsigned long midpoint = 0;
    unsigned long last = linecount - 1;
    char line[128];
    int str_cmp_result;
    char *foo;

    fh_read = fopen(filename, "r");

    if (fh_read == NULL) {
        printf("ERROR: file_binary_search could not open %s\n", filename);
        exit(1);
    }

    while (first <= last) {
        midpoint = (unsigned long) ((first + last)/2);
        fseek(fh_read, midpoint * linewidth, SEEK_SET);

        if (fread(line, statewidth, 1, fh_read)) {
            seek_calls++;

            str_cmp_result = strncmp(state_to_find, line, statewidth);

            if (str_cmp_result == 0) {

                // read the entire line, not just the state
                fseek(fh_read, midpoint * linewidth, SEEK_SET);

                if (fread(line, linewidth, 1, fh_read)) {
                    strstrip(line);
                    fclose(fh_read);
                    return 1;
                } else {
                    printf("ERROR: linewidth read failed for %s\n", filename);
                    exit(1);
                }

            } else if (str_cmp_result < 0) {
                last = midpoint - 1;

            } else {
                first = midpoint + 1;
            }
        } else {
            printf("ERROR: statewidth read failed for %s\n", filename);
            exit(1);
        }
    }

    fclose(fh_read);
    return 0;
}


void
print_cube (char *cube, int size)
{
    int squares_per_side = size * size;
    int square_count = squares_per_side * 6;
    int rows = size * 3;

    for (int row=1; row <= rows; row++) {

        // U
        if (row <= size) {
            int i = ((row-1) * size) + 1;
            int i_end = i + size - 1;
            printf("\t");
            for ( ; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }
            printf("\n");

        // D
        } else if (row > (size * 2)) {
            int i = (squares_per_side * 5) + 1 + ((row - (size*2) - 1) * size);
            int i_end = i + size - 1;
            printf("\t");
            for (; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }
            printf("\n");

        // L, F, R, B
        } else {

            // L
            int i_start = squares_per_side + 1 + ((row - 1 -size) * size);
            int i_end = i_start + size - 1;
            int i = i_start;
            for (; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }

            // F
            i = i_start + squares_per_side;
            i_end = i + size - 1;
            for (; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }

            // R
            i = i_start + (squares_per_side * 2);
            i_end = i + size - 1;
            for (; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }

            // B
            i = i_start + (squares_per_side * 3);
            i_end = i + size - 1;
            for (; i <= i_end; i++) {
                printf("%c ", cube[i]);
            }

            printf("\n");
        }
    }
    printf("\n");
}


/*
 * Replace all occurrences of 'old' with 'new'
 */
void
streplace (char *str, char old, char new)
{
    int i = 0;

    /* Run till end of string */
    while (str[i] != '\0') {

        /* If occurrence of character is found */
        if (str[i] == old) {
            str[i] = new;
        }

        i++;
    }
}


void
str_replace_for_binary(char *str, char *ones)
{
    int i = 0;
    int j;
    int is_a_one = 0;

    /* Run till end of string */
    while (str[i] != '\0') {

        if (str[i] != '0') {
            j = 0;
            is_a_one = 0;

            while (ones[j] != '\0') {

                /* If occurrence of character is found */
                if (str[i] == ones[j]) {
                    is_a_one = 1;
                    break;
                }
                j++;
            }

            if (is_a_one) {
                str[i] = '1';
            } else {
                str[i] = '0';
            }
        }

        i++;
    }
}

void
str_replace_list_of_chars (char *str, char *old, char new)
{
    int i = 0;
    int j;

    /* Run till end of string */
    while (str[i] != '\0') {

        j = 0;

        while (old[j] != '\0') {

            /* If occurrence of character is found */
            if (str[i] == old[j]) {
                str[i] = new;
                break;
            }
            j++;
        }

        i++;
    }
}


void
init_cube(char *cube, int size, lookup_table_type type, char *kociemba)
{
    int squares_per_side = size * size;
    int square_count = squares_per_side * 6;
    int U_start = 1;
    int L_start = U_start + squares_per_side;
    int F_start = L_start + squares_per_side;
    int R_start = F_start + squares_per_side;
    int B_start = R_start + squares_per_side;
    int D_start = B_start + squares_per_side;

    // kociemba_string is in URFDLB order
    int U_start_kociemba = 0;
    int R_start_kociemba = U_start_kociemba + squares_per_side;
    int F_start_kociemba = R_start_kociemba + squares_per_side;
    int D_start_kociemba = F_start_kociemba + squares_per_side;
    int L_start_kociemba = D_start_kociemba + squares_per_side;
    int B_start_kociemba = L_start_kociemba + squares_per_side;

    char ones_UD[2] = {'U', 'D'};

    memset(cube, 0, sizeof(char) * (square_count + 2));
    cube[0] = 'x'; // placeholder
    memcpy(&cube[U_start], &kociemba[U_start_kociemba], squares_per_side);
    memcpy(&cube[L_start], &kociemba[L_start_kociemba], squares_per_side);
    memcpy(&cube[F_start], &kociemba[F_start_kociemba], squares_per_side);
    memcpy(&cube[R_start], &kociemba[R_start_kociemba], squares_per_side);
    memcpy(&cube[B_start], &kociemba[B_start_kociemba], squares_per_side);
    memcpy(&cube[D_start], &kociemba[D_start_kociemba], squares_per_side);
    // LOG("cube:\n%s\n\n", cube);

    switch (type)  {
    case UD_CENTERS_STAGE_555:
        // Convert to 1s and 0s
        str_replace_for_binary(cube, ones_UD);
        print_cube(cube, size);
        break;
    default:
        printf("ERROR: init_cube() does not yet support this --type\n");
        exit(1);
    }
}


int
moves_cost (char *moves)
{
    int cost = 0;
    int i = 0;

    if (moves) {
        cost++;

        while (moves[i] != '\0') {

            // Moves are separated by whitespace
            if (moves[i] == ' ') {
                cost++;
            }

            i++;
        }
    }

    return cost;
}


void
ida_prune_table_preload (struct key_value_pair **hashtable, char *filename)
{
    FILE *fh_read = NULL;
    int BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];
    char token_buffer[BUFFER_SIZE];
    char moves[BUFFER_SIZE];
    char *token_ptr = NULL;
    char state[BUFFER_SIZE];
    int cost = 0;
    struct key_value_pair * pt_entry = NULL;

    fh_read = fopen(filename, "r");
    if (fh_read == NULL) {
        printf("ERROR: ida_prune_table_preload could not open %s\n", filename);
        exit(1);
    }

    LOG("ida_prune_table_preload %s: start\n", filename);

    while (fgets(buffer, BUFFER_SIZE, fh_read) != NULL) {
        strstrip(buffer);

        // strtok modifies the buffer so make a copy
        strcpy(token_buffer, buffer);
        token_ptr = strtok(token_buffer, ":");
        strcpy(state, token_ptr);

        token_ptr = strtok(NULL, ":");
        strcpy(moves, token_ptr);
        cost = moves_cost(moves);

        // LOG("ida_prune_table_preload %s, state %s, moves %s, cost %d\n", filename, state, moves, cost);
        hash_add(hashtable, state, cost);
    }

    fclose(fh_read);
    LOG("ida_prune_table_preload %s: end\n", filename);
}


char *
ida_cost_only_preload (char *filename, unsigned long size)
{
    FILE *fh_read = NULL;
    char *ptr = malloc(sizeof(char) * size);
    memset(ptr, 0, sizeof(char) * size);
    LOG("ida_cost_only_preload: begin %s, %d entries, ptr 0x%x\n", filename, size, ptr);

    fh_read = fopen(filename, "r");
    if (fh_read == NULL) {
        printf("ERROR: ida_cost_only_preload could not open %s\n", filename);
        exit(1);
    }

    if (fread(ptr, size, 1, fh_read)) {
        fclose(fh_read);
        LOG("ida_cost_only_preload: end   %s, ptr 0x%x\n", filename, ptr);
        return ptr;
    } else {
        printf("ERROR: ida_cost_only_preload read failed %s\n", filename);
        exit(1);
    }
}


int
ida_prune_table_cost (struct key_value_pair *hashtable, char *state_to_find)
{
    int cost = 0;
    struct key_value_pair * pt_entry = NULL;

    pt_entry = hash_find(&hashtable, state_to_find);

    if (pt_entry) {
        cost = pt_entry->value;
    }

    return cost;
}


unsigned long
ida_heuristic (char *cube, lookup_table_type type, int debug)
{
    switch (type)  {
    case UD_CENTERS_STAGE_555:
        return (ida_heuristic_UD_centers_555(cube, &UD_centers_555, pt_t_centers_cost_only, pt_x_centers_cost_only, debug));
    default:
        printf("ERROR: ida_heuristic() does not yet support this --type\n");
        exit(1);
    }
}


/* Load the cube state into the scratchpad sp_cube_state */
unsigned long
get_lt_state (char *cube, lookup_table_type type)
{
    switch (type)  {
    case UD_CENTERS_STAGE_555:
        return get_555_centers(cube);
    default:
        printf("ERROR: get_lt_state() does not yet support type %d\n", type);
        exit(1);
    }
    return 0;
}


int
ida_search_complete (char *cube, lookup_table_type type)
{
    struct key_value_pair * pt_entry = NULL;

    switch (type)  {
    case UD_CENTERS_STAGE_555:
        return ida_search_complete_UD_centers_555(cube);
        /*
        // avoid the file IO all together...IDA all the way
        if (file_binary_search("lookup-table-5x5x5-step10-UD-centers-stage.txt", sp_cube_state, 14, 328877780, 19)) {
            LOG("UD_CENTERS_STAGE_555 sp_cube_state %s\n", sp_cube_state);
            return 1;
        }
        */
        break;

    default:
        printf("ERROR: ida_search_complete() does not yet support type %d\n", type);
        exit(1);
    }

    return 0;
}


void
print_moves (move_type *moves, int max_i)
{
    int i = 0;
    printf("SOLUTION: ");

    while (moves[i] != MOVE_NONE) {
        printf("%s ", move2str[moves[i]]);
        i++;

        if (i >= max_i) {
            break;
        }
    }
    printf("\n");
}


int
moves_cancel_out (move_type move, move_type prev_move)
{
    switch (move) {
    case U:        return (prev_move == U_PRIME);
    case U_PRIME:  return (prev_move == U);
    case U2:       return (prev_move == U2);
    case L:        return (prev_move == L_PRIME);
    case L_PRIME:  return (prev_move == L);
    case L2:       return (prev_move == L2);
    case F:        return (prev_move == F_PRIME);
    case F_PRIME:  return (prev_move == F);
    case F2:       return (prev_move == F2);
    case R:        return (prev_move == R_PRIME);
    case R_PRIME:  return (prev_move == R);
    case R2:       return (prev_move == R2);
    case B:        return (prev_move == B_PRIME);
    case B_PRIME:  return (prev_move == B);
    case B2:       return (prev_move == B2);
    case D:        return (prev_move == D_PRIME);
    case D_PRIME:  return (prev_move == D);
    case D2:       return (prev_move == D2);
    case Uw:       return (prev_move == Uw_PRIME);
    case Uw_PRIME: return (prev_move == Uw);
    case Uw2:      return (prev_move == Uw2);
    case Lw:       return (prev_move == Lw_PRIME);
    case Lw_PRIME: return (prev_move == Lw);
    case Lw2:      return (prev_move == Lw2);
    case Fw:       return (prev_move == Fw_PRIME);
    case Fw_PRIME: return (prev_move == Fw);
    case Fw2:      return (prev_move == Fw2);
    case Rw:       return (prev_move == Rw_PRIME);
    case Rw_PRIME: return (prev_move == Rw);
    case Rw2:      return (prev_move == Rw2);
    case Bw:       return (prev_move == Bw_PRIME);
    case Bw_PRIME: return (prev_move == Bw);
    case Bw2:      return (prev_move == Bw2);
    case Dw:       return (prev_move == Dw_PRIME);
    case Dw_PRIME: return (prev_move == Dw);
    case Dw2:      return (prev_move == Dw2);
    default:
        printf("ERROR: moves_cancel_out add support for %d\n", move);
        exit(1);
    }

    return 0;
}


int
steps_on_same_face_and_layer(move_type move, move_type prev_move)
{
    switch (move) {
    case U:
    case U_PRIME:
    case U2:
        switch (prev_move) {
        case U:
        case U_PRIME:
        case U2:
            return 1;
        default:
            return 0;
        }
        break;

    case L:
    case L_PRIME:
    case L2:
        switch (prev_move) {
        case L:
        case L_PRIME:
        case L2:
            return 1;
        default:
            return 0;
        }
        break;

    case F:
    case F_PRIME:
    case F2:
        switch (prev_move) {
        case F:
        case F_PRIME:
        case F2:
            return 1;
        default:
            return 0;
        }
        break;

    case R:
    case R_PRIME:
    case R2:
        switch (prev_move) {
        case R:
        case R_PRIME:
        case R2:
            return 1;
        default:
            return 0;
        }
        break;

    case B:
    case B_PRIME:
    case B2:
        switch (prev_move) {
        case B:
        case B_PRIME:
        case B2:
            return 1;
        default:
            return 0;
        }
        break;

    case D:
    case D_PRIME:
    case D2:
        switch (prev_move) {
        case D:
        case D_PRIME:
        case D2:
            return 1;
        default:
            return 0;
        }
        break;

    // 2-layer turns
    case Uw:
    case Uw_PRIME:
    case Uw2:
        switch (prev_move) {
        case Uw:
        case Uw_PRIME:
        case Uw2:
            return 1;
        default:
            return 0;
        }
        break;

    case Lw:
    case Lw_PRIME:
    case Lw2:
        switch (prev_move) {
        case Lw:
        case Lw_PRIME:
        case Lw2:
            return 1;
        default:
            return 0;
        }
        break;

    case Fw:
    case Fw_PRIME:
    case Fw2:
        switch (prev_move) {
        case Fw:
        case Fw_PRIME:
        case Fw2:
            return 1;
        default:
            return 0;
        }
        break;

    case Rw:
    case Rw_PRIME:
    case Rw2:
        switch (prev_move) {
        case Rw:
        case Rw_PRIME:
        case Rw2:
            return 1;
        default:
            return 0;
        }
        break;

    case Bw:
    case Bw_PRIME:
    case Bw2:
        switch (prev_move) {
        case Bw:
        case Bw_PRIME:
        case Bw2:
            return 1;
        default:
            return 0;
        }
        break;

    case Dw:
    case Dw_PRIME:
    case Dw2:
        switch (prev_move) {
        case Dw:
        case Dw_PRIME:
        case Dw2:
            return 1;
        default:
            return 0;
        }
        break;

    default:
        printf("ERROR: steps_on_same_face_and_layeradd support for %d\n", move);
        exit(1);
    }

    return 0;
}

int
ida_search (int cost_to_here,
            move_type *moves_to_here,
            int threshold,
            move_type prev_move,
            char *cube,
            lookup_table_type type)
{
    int cost_to_goal = 0;
    int f_cost = 0;
    move_type move;
    char cube_tmp[array_size];

    int debug = 0;

    if (debug) {
        print_moves(moves_to_here, cost_to_here);
    }

    ida_count++;
    cost_to_goal = ida_heuristic(cube, type, debug);
    f_cost = cost_to_here + cost_to_goal;

    // Abort Searching
    if (f_cost >= threshold) {
        if (debug) {
            LOG("IDA prune f_cost %d vs threshold %d (cost_to_here %d, cost_to_goal %d)\n",
                f_cost, threshold, cost_to_here, cost_to_goal);
            LOG("\n");
        }
        return 0;
    }

    if (ida_search_complete(cube, type)) {
        // We are finished!!
        LOG("IDA count %d, f_cost %d vs threshold %d (cost_to_here %d, cost_to_goal %d)\n",
            ida_count, f_cost, threshold, cost_to_here, cost_to_goal);
        print_moves(moves_to_here, cost_to_here);
        return 1;
    }

    unsigned long lt_state = get_lt_state(cube, type);
    char my_ida_explored_state[64];
    char cost_to_here_str[3];
    sprintf(my_ida_explored_state, "%lux", lt_state);
    sprintf(cost_to_here_str, "%d", cost_to_here);
    strcat(my_ida_explored_state, cost_to_here_str);

    if (hash_find(&ida_explored, my_ida_explored_state)) {
        return 0;
    }

    hash_add(&ida_explored, my_ida_explored_state, 0);

    for (int i = 0; i < MOVE_COUNT_555; i++) {
        move = moves_555[i];

        if (steps_on_same_face_and_layer(move, prev_move)) {
            continue;
        }

        char cube_copy[array_size];
        memcpy(cube_copy, cube, sizeof(char) * array_size);
        rotate_555(cube_copy, cube_tmp, array_size, move);
        moves_to_here[cost_to_here] = move;

        if (ida_search(cost_to_here + 1, moves_to_here, threshold, move, cube_copy, type)) {
            return 1;
        }
    }

    return 0;
}


int
ida_solve (char *cube, lookup_table_type type)
{
    int MAX_SEARCH_DEPTH = 20;
    move_type moves_to_here[MAX_SEARCH_DEPTH];
    int min_ida_threshold = 0;

    ida_prune_table_preload(&UD_centers_555, "lookup-table-5x5x5-step10-UD-centers-stage.txt.5-deep");
    pt_t_centers_cost_only = ida_cost_only_preload("lookup-table-5x5x5-step11-UD-centers-stage-t-center-only.cost-only.txt", 16711681);
    pt_x_centers_cost_only = ida_cost_only_preload("lookup-table-5x5x5-step12-UD-centers-stage-x-center-only.cost-only.txt", 16711681);

    // get_lt_state(cube, type);
    min_ida_threshold = ida_heuristic(cube, type, 0);
    LOG("min_ida_threshold %d\n", min_ida_threshold);

    for (int threshold = min_ida_threshold; threshold <= MAX_SEARCH_DEPTH; threshold++) {
        ida_count = 0;
        memset(moves_to_here, MOVE_NONE, sizeof(move_type) * MAX_SEARCH_DEPTH);
        hash_delete_all(&ida_explored);

        if (ida_search(0, moves_to_here, threshold, MOVE_NONE, cube, type)) {
            ida_count_total += ida_count;
            LOG("IDA threshold %d, explored %d branches (%d total), found solution\n", threshold, ida_count, ida_count_total);
            free(pt_t_centers_cost_only);
            pt_t_centers_cost_only = NULL;
            free(pt_x_centers_cost_only);
            pt_x_centers_cost_only = NULL;
            return 1;
        } else {
            ida_count_total += ida_count;
            LOG("IDA threshold %d, explored %d branches\n", threshold, ida_count);
        }
    }

    free(pt_t_centers_cost_only);
    pt_t_centers_cost_only = NULL;
    free(pt_x_centers_cost_only);
    pt_x_centers_cost_only = NULL;

    LOG("IDA failed with range %d->%d\n", min_ida_threshold, MAX_SEARCH_DEPTH);
    return 0;
}


int
main (int argc, char *argv[])
{
    lookup_table_type type = NONE;
    int cube_size_type = 0;
    int cube_size_kociemba = 0;
    char kociemba[300];
    memset(kociemba, 0, sizeof(char) * 300);

    for (int i = 1; i < argc; i++) {
        if (strmatch(argv[i], "-k") || strmatch(argv[i], "--kociemba")) {
            i++;
            strcpy(kociemba, argv[i]);
            cube_size_kociemba = (int) sqrt(strlen(kociemba) / 6);

        } else if (strmatch(argv[i], "-t") || strmatch(argv[i], "--type")) {
            i++;

            if (strmatch(argv[i], "5x5x5-UD-centers-stage")) {
                type = UD_CENTERS_STAGE_555;
                cube_size_type = 5;
            } else {
                printf("ERROR: %s is an invalid --type\n", argv[i]);
                exit(1);
            }

        } else if (strmatch(argv[i], "-h") || strmatch(argv[i], "--help")) {
            printf("\nida_search --kociemba KOCIEMBA_STRING --type 5x5x5-UD-centers-stage\n\n");
            exit(0);

        } else {
            printf("ERROR: %s is an invalid arg\n", argv[i]);
            exit(1);
        }
    }

    if (!type) {
        printf("ERROR: --type is required\n");
        exit(1);
    }

    if (cube_size_type != cube_size_kociemba) {
        printf("ERROR: --type cube size is %d, --kociemba cube size is %d\n", cube_size_type, cube_size_kociemba);
        exit(1);
    }

    if (cube_size_kociemba < 2 || cube_size_kociemba > 7) {
        printf("ERROR: only 2x2x2 through 7x7x7 cubes are supported, yours is %dx%dx%d\n", cube_size_kociemba, cube_size_kociemba, cube_size_kociemba);
        exit(1);
    }

    int cube_size = cube_size_kociemba;
    array_size = (cube_size * cube_size * 6) + 2;
    char cube[array_size];
    char cube_tmp[array_size];

    sp_cube_state = malloc(sizeof(char) * array_size);
    memset(cube_tmp, 0, sizeof(char) * array_size);
    init_cube(cube, cube_size, type, kociemba);

    // print_cube(cube, cube_size);
    ida_solve(cube, type);

    // Fw2 Dw' Rw B' Lw' F D' B' Lw' Dw Bw'
    /*
    memcpy(cube_tmp, cube, sizeof(char) * array_size);
    rotate_555(cube, cube_tmp, array_size, Fw2);

    memcpy(cube_tmp, cube, sizeof(char) * array_size);
    rotate_555(cube, cube_tmp, array_size, Dw_PRIME);

    print_cube(cube, cube_size);
    */
}