#include <stdio.h>
#include <malloc.h>
#include <mpi.h>
#include <unistd.h>

const int MAX_LENGTH_OF_FILE_NAME = 200;
const int MAX_LENGTH_OF_DATASET_VALUE_LENGTH = 3;
const char DIVIDE_CHAR = ',';


int main() {
    int amount_of_datasets;
    printf("Enter amount of datasets:");
    scanf(" %d", &amount_of_datasets);

    int **dataset_matrix = malloc(sizeof(int *) * amount_of_datasets);
    int *dataset_lengths = malloc(sizeof(int) * amount_of_datasets);

    char cmd[MAX_LENGTH_OF_FILE_NAME];

    for (int current_dataset_index = 0; current_dataset_index < amount_of_datasets; current_dataset_index++) {
        char new_dataset_path[MAX_LENGTH_OF_FILE_NAME];
        printf("Enter path to the %d dataset related to the current directory:", current_dataset_index + 1);
        scanf(" %s", new_dataset_path);

        if (getcwd(cmd, MAX_LENGTH_OF_FILE_NAME) != NULL) {
            FILE *current_dataset_file = fopen(strcat(cmd, new_dataset_path), "r");
            if (current_dataset_file == NULL) {
                printf("Error: could not open file %s", new_dataset_path);
                return 1;
            }

            int amount_of_values_in_dataset;
            printf("Enter amount of values in dataset:");
            scanf(" %d", &amount_of_values_in_dataset);
            dataset_lengths[current_dataset_index] = amount_of_values_in_dataset;
            dataset_matrix[current_dataset_index] = malloc(sizeof(int) * amount_of_values_in_dataset);

            int buffer_length =
                    (int) sizeof(char) * amount_of_values_in_dataset * (MAX_LENGTH_OF_DATASET_VALUE_LENGTH + 1);
            char *buffer = malloc(buffer_length);
            fgets(buffer, buffer_length, current_dataset_file);
            fclose(current_dataset_file);

            int current_buffer_index = 0;

            for (int current_value_index = 0;
                 current_value_index < amount_of_values_in_dataset; current_value_index++) {
                char current_value_str[MAX_LENGTH_OF_DATASET_VALUE_LENGTH];

                for (int current_char_index = 0; current_char_index < MAX_LENGTH_OF_DATASET_VALUE_LENGTH ||
                                                 buffer[current_buffer_index] ==
                                                 DIVIDE_CHAR; current_char_index++, current_buffer_index++) {
                    current_value_str[current_char_index] = buffer[current_buffer_index];
                }

                dataset_matrix[current_dataset_index][current_value_index] = atoi(current_value_str);
            }
            free(buffer);
        } else {
            perror("getcwd() error");
            return 1;
        }

        printf("%d", dataset_matrix[0][3]);
    }

    return 0;
}
