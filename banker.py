'''

    Name: Anjan Roy
    Class: CSC 360 - Operating Systems
    Programming Project 04: Implementation of Banker's Algorithm
    Instructor: Dr. Siming Liu

'''
import sys

def read_file(filename):
    """
    Function: read_file
    Description: Reads the input file and initializes the allocation, max, available, and request matrices.
    Value Parameter Data Dictionary:
    - filename: str, name of the input file.
    Reference Parameter Data Dictionary:
    - n: int, number of processes.
    - m: int, number of resource types.
    - allocation: list of lists, allocation matrix.
    - max_matrix: list of lists, max matrix.
    - available: list, available vector.
    - request_process: int, process making the request.
    - request: list, request vector.
    """
    with open(filename, 'r') as file:
        n = int(file.readline())
        m = int(file.readline())

        allocation = []
        for _ in range(n):
            allocation.append(list(map(int, file.readline().split())))

        max_matrix = []
        for _ in range(n):
            max_matrix.append(list(map(int, file.readline().split())))

        available = list(map(int, file.readline().split()))

        request_line = file.readline().split(':')
        request_process = int(request_line[0])
        request = list(map(int, request_line[1].split()))

    return n, m, allocation, max_matrix, available, request_process, request

def print_matrix(title, matrix):
    """
    Function: print_matrix
    Description: Prints a matrix with a title.
    Value Parameter Data Dictionary:
    - title: str, title of the matrix.
    - matrix: list of lists, matrix to print.
    Reference Parameter Data Dictionary: None
    """
    print(title)
    for row in matrix:
        print(' '.join(map(str, row)))

def print_vector(title, vector):
    """
    Function: print_vector
    Description: Prints a vector with a title.
    Value Parameter Data Dictionary:
    - title: str, title of the vector.
    - vector: list, vector to print.
    Reference Parameter Data Dictionary: None
    """
    print(title)
    print(' '.join(map(str, vector)))

def calculate_need_matrix(n, m, max_matrix, allocation):
    """
    Function: calculate_need_matrix
    Description: Calculates the need matrix using the allocation and max matrices.
    Value Parameter Data Dictionary: None
    Reference Parameter Data Dictionary:
    - n: int, number of processes.
    - m: int, number of resource types.
    - max_matrix: list of lists, max matrix.
    - allocation: list of lists, allocation matrix.
    - need: list of lists, need matrix.
    """
    need = [[0] * m for _ in range(n)]
    for i in range(n):
        for j in range(m):
            need[i][j] = max_matrix[i][j] - allocation[i][j]
    return need

def is_safe(n, m, allocation, need, available):
    """
    Function: is_safe
    Description: Checks if the system is in a safe state.
    Value Parameter Data Dictionary: None
    Reference Parameter Data Dictionary:
    - n: int, number of processes.
    - m: int, number of resource types.
    - allocation: list of lists, allocation matrix.
    - need: list of lists, need matrix.
    - available: list, available vector.
    Local Variable Data Dictionary:
    - work: list, copy of the available vector.
    - finish: list, track which processes can finish.
    - safe_sequence: list, sequence of safe processes.
    """
    work = available[:]
    finish = [False] * n
    safe_sequence = []

    while len(safe_sequence) < n:
        found = False
        for i in range(n):
            if not finish[i]:
                if all(need[i][j] <= work[j] for j in range(m)):
                    for j in range(m):
                        work[j] += allocation[i][j]
                    safe_sequence.append(i)
                    finish[i] = True
                    found = True
        if not found:
            return False
    return True

def can_request_be_granted(process, request, n, m, allocation, need, available):
    """
    Function: can_request_be_granted
    Description: Checks if a resource request can be granted and updates matrices if the request is safe.
    Value Parameter Data Dictionary:
    - process: int, process making the request.
    - request: list, request vector.
    Reference Parameter Data Dictionary:
    - n: int, number of processes.
    - m: int, number of resource types.
    - allocation: list of lists, allocation matrix.
    - need: list of lists, need matrix.
    - available: list, available vector.
    Local Variable Data Dictionary:
    - safe: bool, if the system is safe after granting the request.
    """
    if any(request[j] > need[process][j] for j in range(m)):
        return False
    if any(request[j] > available[j] for j in range(m)):
        return False

    for j in range(m):
        available[j] -= request[j]
        allocation[process][j] += request[j]
        need[process][j] -= request[j]

    safe = is_safe(n, m, allocation, need, available)

    for j in range(m):
        available[j] += request[j]
        allocation[process][j] -= request[j]
        need[process][j] += request[j]

    return safe

def main():
    """
    Function: main
    Description: Main function to execute the Banker's algorithm.
    Value Parameter Data Dictionary: None
    Reference Parameter Data Dictionary: None
    """
    if len(sys.argv) != 2:
        print("Usage: python banker.py <input file>")
        sys.exit(1)

    filename = sys.argv[1]
    n, m, allocation, max_matrix, available, request_process, request = read_file(filename)

    print(f"There are {n} processes in the system.")
    print(f"There are {m} resource types.")
    print_matrix("The Allocation Matrix is...", allocation)
    print_matrix("The Max Matrix is...", max_matrix)

    need = calculate_need_matrix(n, m, max_matrix, allocation)
    print_matrix("The Need Matrix is...", need)
    print_vector("The Available Vector is...", available)

    if is_safe(n, m, allocation, need, available):
        print("THE SYSTEM IS IN A SAFE STATE!")
    else:
        print("THE SYSTEM IS NOT IN A SAFE STATE!")

    print("The Request Vector is...")
    print(f"{request_process}:{' '.join(map(str, request))}")

    if can_request_be_granted(request_process, request, n, m, allocation, need, available):
        print("THE REQUEST CAN BE GRANTED!")
    else:
        print("THE REQUEST CANNOT BE GRANTED!")

if __name__ == "__main__":
    main()
