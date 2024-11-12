import time

def print_matrix(matrix, size):
    for i in range(size):
        for j in range(size):
            print(matrix[i][j], end=" ")
        print()


size = 10000
matrix = [[0] * size for _ in range(size)]
value = 0

for i in range(size):
    for j in range(size):
        matrix[i][j] = value
        value += 1

start_time = time.time()

flat_array = []
for i in range(size):
    for j in range(size):
        flat_array.append(matrix[i][j])

flat_array.sort()
flat_array = flat_array[::-1]

index = 0
for i in range(size):
    for j in range(size):
        matrix[i][j] = flat_array[index]
        index += 1

end_time = time.time()

execution_time = (end_time - start_time) * 1000
print(f"Execution time: {execution_time:.2f} ms")