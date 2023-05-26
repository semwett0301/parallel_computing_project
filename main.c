#include <stdio.h>
#include <malloc.h>
#include <mpi.h>
#include <unistd.h>
#include <assert.h>

const int MAX_LENGTH_OF_FILE_NAME = 200;
const int MAX_LENGTH_OF_DATASET_VALUE_LENGTH = 3;
const char DIVIDE_CHAR = ',';
const int MAX_VALUE_IN_DATASET = 101;


struct jaccard_comparison {
    int start;
    int finish;
    double coefficient;
};

int scan_amount_od_datasets(int *result_var) {
    printf("\nEnter amount of datasets:\n");
    if (scanf(" %d", result_var) != 1 || *result_var <= 0) {
        perror("Error in amount of datasets scanning (not int or negative)");
        return 1;
    }

    return 0;
}

int **create_dataset_matrix(int amount_of_datasets) {
    return malloc(sizeof(int *) * amount_of_datasets);
}

int *create_dataset_lengths(int amount_of_datasets) {
    return malloc(sizeof(int) * amount_of_datasets);
}

struct jaccard_comparison *create_jaccard_comparison_result(int amount_of_datasets) {
    return malloc(sizeof(struct jaccard_comparison) * (amount_of_datasets * (amount_of_datasets - 1) / 2));
}

int get_cmd_name(char *result_var) {
    if (getcwd(result_var, MAX_LENGTH_OF_FILE_NAME) == NULL) {
        perror("getcwd() error");
        return 1;
    }

    return 0;
}

void get_dataset_path(char *result_var, int dataset_index) {
    printf("\nEnter path to the %d dataset related to the current directory:\n", dataset_index);
    scanf("%s", result_var);
}

FILE *open_dataset_file(char *cmd, char *file_path) {
    FILE *current_dataset_file = fopen(strcat(cmd, file_path), "r");
    if (current_dataset_file == NULL) {
        perror(strcat("Error: could not open file ", file_path));
    }

    return current_dataset_file;
}

int get_amount_of_values_in_dataset(int *result_var) {
    printf("\nEnter amount of values in dataset:\n");
    if (scanf("%d", result_var) != 1 || *result_var <= 0) {
        perror("There was a problem in reading amount of values in dataset (not an integer or negative)");
        return 1;
    }
    return 0;
}


char **str_split(char *a_str, const char a_delim) {
    char **result;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);

    count++;

    result = malloc(sizeof(char *) * count);

    if (result) {
        size_t idx = 0;
        char *token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int transfer_buffer_to_int_array(char *buffer, int amount_of_values_in_dataset, int *result_var) {
    char **current_value_str = str_split(buffer, DIVIDE_CHAR);


    for (int i = 0; i < amount_of_values_in_dataset; i++) {
        assert(current_value_str[i] != NULL);
        int new_value = atoi(current_value_str[i]);
        if (new_value == 0 && current_value_str[i][0] != '0' && current_value_str[i][1] != '\0') {
            perror("Incorrect format of dataset");
            return 1;
        }
        result_var[i] = new_value;
    }

    return 0;
}

int fill_the_datasets(int **dataset_matrix, int *dataset_lengths, int amount_of_datasets) {
    for (int current_dataset_index = 0; current_dataset_index < amount_of_datasets; current_dataset_index++) {
        char cmd[MAX_LENGTH_OF_FILE_NAME];
        if (get_cmd_name(cmd) == 1) {
            return 1;
        }

        char new_dataset_path[MAX_LENGTH_OF_FILE_NAME];
        get_dataset_path(new_dataset_path, current_dataset_index + 1);

        FILE *current_dataset_file = open_dataset_file(cmd, new_dataset_path);
        if (current_dataset_file == NULL) {
            return 1;
        }

        int amount_of_values_in_dataset;
        get_amount_of_values_in_dataset(&amount_of_values_in_dataset);

        dataset_lengths[current_dataset_index] = amount_of_values_in_dataset;
        dataset_matrix[current_dataset_index] = malloc(sizeof(int) * amount_of_values_in_dataset);

        int buffer_length =
                (int) sizeof(char) * amount_of_values_in_dataset * (MAX_LENGTH_OF_DATASET_VALUE_LENGTH + 1);
        char *buffer = malloc(buffer_length);
        fscanf(current_dataset_file, "%s", buffer);
        fclose(current_dataset_file);

        if (transfer_buffer_to_int_array(buffer, amount_of_values_in_dataset, dataset_matrix[current_dataset_index]) ==
            1) {
            return 1;
        }

        free(buffer);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int rank_id;
    int processes_size;
    int divided_size;
    int dataset_length;
    int **dataset_matrix;
    int *dataset_lengths;
    struct jaccard_comparison* jaccard_result;

    int **result_table;
    int *gather_table;

    int amount_of_datasets;
    int *send_data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_id);
    MPI_Comm_size(MPI_COMM_WORLD, &processes_size);

    if (rank_id == 0) {
        if (scan_amount_od_datasets(&amount_of_datasets) == 1) {
            return 1;
        }

        dataset_matrix = create_dataset_matrix(amount_of_datasets);
        dataset_lengths = create_dataset_lengths(amount_of_datasets);
        jaccard_result = create_jaccard_comparison_result(amount_of_datasets);

        result_table = calloc(amount_of_datasets, sizeof(int *));

        for (int i = 0; i < amount_of_datasets; i++) {
            result_table[i] = calloc(MAX_VALUE_IN_DATASET, sizeof(int));
        }

        if (fill_the_datasets(dataset_matrix, dataset_lengths, amount_of_datasets) == 1) {
            return 1;
        }
    }

    MPI_Bcast(&amount_of_datasets, 1, MPI_INT, 0, MPI_COMM_WORLD);


    for (int dataset_number = 0; dataset_number < amount_of_datasets; dataset_number++) {

        if (rank_id == 0) {
            gather_table = calloc(processes_size * MAX_VALUE_IN_DATASET, sizeof(int));
            send_data = dataset_matrix[dataset_number];
            dataset_length = dataset_lengths[dataset_number];
        }

        MPI_Bcast(&dataset_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (processes_size >= dataset_length) {
            divided_size = 1;
        } else {
            if (dataset_length % processes_size == 0) {
                divided_size = dataset_length / processes_size;
            } else {
                divided_size = dataset_length / processes_size + 1;
            }
        }

        int received_data[divided_size];
        MPI_Scatter(send_data, divided_size, MPI_INT, received_data, divided_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);


        int final_count[MAX_VALUE_IN_DATASET];

        for (int i = 0; i < MAX_VALUE_IN_DATASET; i++) {
            final_count[i] = 0;
        }

        for (int i = 0; i < divided_size; i++) {
            if (rank_id * divided_size + i + 1 <= dataset_length) {
                final_count[received_data[i]] += 1;
            }
        }

        MPI_Gather(final_count, MAX_VALUE_IN_DATASET, MPI_INT, gather_table, MAX_VALUE_IN_DATASET, MPI_INT, 0,
                   MPI_COMM_WORLD);

        if (rank_id == 0) {
            for (int i = 0; i < MAX_VALUE_IN_DATASET; i++) {
                for (int j = 0; j < processes_size; j++) {
                    result_table[dataset_number][i] += gather_table[i + j * MAX_VALUE_IN_DATASET];
                }
            }
        }
    }

    if (rank_id == 0) {
        int jaccard_idx = 0;
        for (int i = 0; i < amount_of_datasets; i++) {
            for (int j = i + 1; j < amount_of_datasets; j++) {
                jaccard_result[jaccard_idx].start = i;
                jaccard_result[jaccard_idx].finish = j;

                int common_summary = 0;
                for (int current_pos = 0; current_pos < MAX_VALUE_IN_DATASET; current_pos++) {
                    common_summary += result_table[i][current_pos] > result_table[j][current_pos] ? result_table[j][current_pos] : result_table[i][current_pos];
                }

                jaccard_result[jaccard_idx].coefficient = (double) common_summary / (dataset_lengths[i] + dataset_lengths[j] - common_summary);
                jaccard_idx++;
            }
        }

        for (int i = 0; i < amount_of_datasets * (amount_of_datasets - 1) / 2; i++) {
            printf("\nStart: %d, Finish: %d -- Coeff: %lf", jaccard_result[i].start, jaccard_result[i].finish, jaccard_result[i].coefficient);
        }
    }

    MPI_Finalize();

    return 0;
}
