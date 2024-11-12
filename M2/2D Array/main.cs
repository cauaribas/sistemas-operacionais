using System.Diagnostics;

void PrintMatrix(int[,] matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            Console.Write(matrix[i, j] + " ");
        }
        Console.WriteLine();
    }
}

int size = 10000;
int[,] matrix = new int[size, size];
int value = 0;

for (int i = 0; i < size; i++)
{
    for (int j = 0; j < size; j++)
    {
        matrix[i, j] = value++;
    }
}

Stopwatch stopwatch = new Stopwatch();
stopwatch.Start();

int[] flatArray = new int[size * size];
for (int i = 0; i < size; i++)
{
    for (int j = 0; j < size; j++)
    {
        flatArray[i * size + j] = matrix[i, j];
    }
}

Array.Sort(flatArray);
Array.Reverse(flatArray);

for (int i = 0; i < size; i++)
{
    for (int j = 0; j < size; j++)
    {
        matrix[i, j] = flatArray[i * size + j];
    }
}

stopwatch.Stop();

Console.WriteLine($"Execution time: {stopwatch.ElapsedMilliseconds} ms");
Console.ReadLine();