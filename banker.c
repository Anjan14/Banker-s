//********************************************************************
//
// Name: Anjan Roy
// Class: CSC 360 - Operating Systems 
// Programming Project 04: Implementatin of Banker's Algorithm
// July 7th, 2024
// Instructor: Dr. Siming Liu
//
//********************************************************************
#include <stdio.h>
#include <stdlib.h>

// Function: readFile
// Description: Reads the input file and initializes the allocation, max, available, and request matrices.
// Value Parameter Data Dictionary:
// - filename: char pointer to the name of the input file.
// Reference Parameter Data Dictionary:
// - n: integer reference to store the number of processes.
// - m: integer reference to store the number of resource types.
// - allocation: double pointer to store the allocation matrix.
// - max: double pointer to store the max matrix.
// - available: pointer to store the available vector.
// - requestProcess: integer reference to store the process making the request.
// - request: pointer to store the request vector.
void readFile(char *filename);

// Function: printMatrices
// Description: Prints the allocation and max matrices.
// Value Parameter Data Dictionary: None
// Reference Parameter Data Dictionary:
// - allocation: double pointer to the allocation matrix.
// - max: double pointer to the max matrix.
// - n: integer reference to the number of processes.
// - m: integer reference to the number of resource types.
void printMatrices();

// Function: calculateNeedMatrix
// Description: Calculates the need matrix using the allocation and max matrices.
// Value Parameter Data Dictionary: None
// Reference Parameter Data Dictionary:
// - need: double pointer to the need matrix.
// - allocation: double pointer to the allocation matrix.
// - max: double pointer to the max matrix.
// - n: integer reference to the number of processes.
// - m: integer reference to the number of resource types.
void calculateNeedMatrix();

// Function: isSafe
// Description: Checks if the system is in a safe state.
// Value Parameter Data Dictionary: None
// Reference Parameter Data Dictionary:
// - n: integer reference to the number of processes.
// - m: integer reference to the number of resource types.
// - allocation: double pointer to the allocation matrix.
// - need: double pointer to the need matrix.
// - available: pointer to the available vector.
// Local Variable Data Dictionary:
// - work: pointer to store a copy of the available vector.
// - finish: pointer to track which processes can finish.
// - safeSequence: array to store the sequence of safe processes.
int isSafe();

// Function: canRequestBeGranted
// Description: Checks if a resource request can be granted and updates matrices if the request is safe.
// Value Parameter Data Dictionary:
// - process: integer value of the requesting process.
// - request: pointer to the request vector.
// Reference Parameter Data Dictionary:
// - n: integer reference to the number of processes.
// - m: integer reference to the number of resource types.
// - allocation: double pointer to the allocation matrix.
// - need: double pointer to the need matrix.
// - available: pointer to the available vector.
// Local Variable Data Dictionary:
// - safe: integer to store if the system is safe after granting the request.
int canRequestBeGranted(int process, int request[]);

// Function: printVector
// Description: Prints a vector with a title.
// Value Parameter Data Dictionary:
// - title: char pointer to the title of the vector.
// - vector: pointer to the vector.
// - length: integer value of the length of the vector.
// Reference Parameter Data Dictionary: None
void printVector(char *title, int *vector, int length);

// Function: printMatrix
// Description: Prints a matrix with a title.
// Value Parameter Data Dictionary:
// - title: char pointer to the title of the matrix.
// - matrix: double pointer to the matrix.
// - rows: integer value of the number of rows.
// - cols: integer value of the number of columns.
// Reference Parameter Data Dictionary: None
void printMatrix(char *title, int **matrix, int rows, int cols);

int n, m;
int **allocation, **max, **need;
int *available, *request;
int requestProcess;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    readFile(argv[1]);
    printMatrices();
    calculateNeedMatrix();
    printMatrix("The Need Matrix is...", need, n, m);
    printVector("The Available Vector is...", available, m);

    if (isSafe()) {
        printf("THE SYSTEM IS IN A SAFE STATE!\n");
    } else {
        printf("THE SYSTEM IS NOT IN A SAFE STATE!\n");
    }

    printf("The Request Vector is...\n");
    for (int j = 0; j < m; j++) {
        printf("%d ", request[j]);
    }
    printf("\n");

    if (canRequestBeGranted(requestProcess, request)) {
        printf("THE REQUEST CAN BE GRANTED!\n");
    } else {
        printf("THE REQUEST CANNOT BE GRANTED!\n");
    }

    return 0;
}

void readFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &n);
    fscanf(file, "%d", &m);

    // Allocate memory for matrices and vectors
    allocation = (int **)malloc(n * sizeof(int *));
    max = (int **)malloc(n * sizeof(int *));
    need = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        allocation[i] = (int *)malloc(m * sizeof(int));
        max[i] = (int *)malloc(m * sizeof(int));
        need[i] = (int *)malloc(m * sizeof(int));
    }
    available = (int *)malloc(m * sizeof(int));
    request = (int *)malloc(m * sizeof(int));

    // Read allocation matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(file, "%d", &allocation[i][j]);
        }
    }

    // Read max matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(file, "%d", &max[i][j]);
        }
    }

    // Read available vector
    for (int j = 0; j < m; j++) {
        fscanf(file, "%d", &available[j]);
    }

    // Read request vector
    fscanf(file, "%d:", &requestProcess);
    for (int j = 0; j < m; j++) {
        fscanf(file, "%d", &request[j]);
    }

    fclose(file);
}

void printMatrices() {
    printMatrix("The Allocation Matrix is...", allocation, n, m);
    printMatrix("The Max Matrix is...", max, n, m);
}

void calculateNeedMatrix() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
}

int isSafe() {
    int *work = (int *)malloc(m * sizeof(int));
    int *finish = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < m; i++) {
        work[i] = available[i];
    }
    for (int i = 0; i < n; i++) {
        finish[i] = 0;
    }

    int safeSequence[n];
    int count = 0;

    while (count < n) {
        int found = 0;
        for (int i = 0; i < n; i++) {
            if (finish[i] == 0) {
                int j;
                for (j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        break;
                    }
                }
                if (j == m) {
                    for (int k = 0; k < m; k++) {
                        work[k] += allocation[i][k];
                    }
                    safeSequence[count++] = i;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (found == 0) {
            free(work);
            free(finish);
            return 0; // System is not in a safe state
        }
    }

    free(work);
    free(finish);
    return 1; // System is in a safe state
}

int canRequestBeGranted(int process, int request[]) {
    for (int j = 0; j < m; j++) {
        if (request[j] > need[process][j]) {
            return 0; // Request exceeds maximum claim
        }
    }
    for (int j = 0; j < m; j++) {
        if (request[j] > available[j]) {
            return 0; // Not enough resources available
        }
    }

    // Temporarily allocate resources
    for (int j = 0; j < m; j++) {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    int safe = isSafe();

    // Revert allocation
    for (int j = 0; j < m; j++) {
        available[j] += request[j];
        allocation[process][j] -= request[j];
        need[process][j] += request[j];
    }

    return safe;
}

void printVector(char *title, int *vector, int length) {
    printf("%s\n", title);
    for (int i = 0; i < length; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

void printMatrix(char *title, int **matrix, int rows, int cols) {
    printf("%s\n", title);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
