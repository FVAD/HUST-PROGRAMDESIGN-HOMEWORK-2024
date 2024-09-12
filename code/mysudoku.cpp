
void shuffle(int *array, int size) // 打乱数组，便于生成随机数独
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

bool isSafe(int grid[N][N], int row, int col, int num) // 判断这个点位的数字是否合理, row 和 col 是 n - 1
{
    for (int x = 0; x < N; x++)
    {
        if (grid[row][x] == num || grid[x][col] == num)
        {
            return false;
        }
    }

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (grid[i + startRow][j + startCol] == num)
            {
                return false;
            }
        }
    }

    if (row == col)
    {
        for (int i = 0; i < N; i++)
        {
            if (grid[i][i] == num)
            {
                return false;
            }
        }
    }
    if (row + col == N - 1)
    {
        for (int i = 0; i < N; i++)
        {
            if (grid[i][N - i - 1] == num)
            {
                return false;
            }
        }
    }

    return true;
}

bool solveSudoku(int grid[N][N], int row, int col) // 生成数独
{
    if (row == N - 1 && col == N)
    {
        // 生成数独的方式是不断加col，col加到顶之后进位，所以最终结束条件是row到达最大值，col到达最大值下一位
        return true;
    }
    if (col == N)
    {
        row++;
        col = 0;
    }
    if (grid[row][col] != 0)
    {
        // 表示这个位置已经存在数字，进入下一个
        return solveSudoku(grid, row, col + 1);
    }

    int numList[N]; // 产生随机数的数组
    for (int i = 0; i < N; i++)
    {
        numList[i] = i + 1; // 数组初始化
    }
    shuffle(numList, N); // 打乱数组顺序

    for (int i = 0; i < N; i++)
    {
        int num = numList[i];
        if (isSafe(grid, row, col, num))
        {
            grid[row][col] = num;
            if (solveSudoku(grid, row, col + 1)) // 进入下一个位置，即向col+1的方向移动一次
            {
                return true; // false在最后，所以一半情况都是返回true
            }
            grid[row][col] = 0;
        }
    }
    return false;
}

void printGrid(int grid[N][N])
{
    for (int r = 0; r < N; r++)
    {
        if (r % 3 == 0 && r != 0)
        {
            printf("-------+-------+------\n");
        }
        for (int d = 0; d < N; d++)
        {
            if (d % 3 == 0 && d != 0)
            {
                printf(" |");
            }
            printf(" %d", grid[r][d] ? grid[r][d] : 0);
        }
        printf("\n");
    }
}

void removeKDigits(int grid[N][N], int k)
{
    while (k != 0)
    {
        int i = rand() % N;
        int j = rand() % N;
        if (grid[i][j] != 0)
        {
            grid[i][j] = 0;
            k--;
        }
    }
}

bool solveSudokuUnique(int grid[N][N], int row, int col, int *count) // 验证数独是否存在多个解
{
    if (row == N - 1 && col == N)
    {
        (*count)++;
        return (*count) > 1; // 继续寻找多于一个解
    }
    if (col == N)
    {
        row++;
        col = 0;
    }
    if (grid[row][col] != 0)
    {
        return solveSudokuUnique(grid, row, col + 1, count);
    }

    int numList[N];
    for (int i = 0; i < N; i++)
    {
        numList[i] = i + 1;
    }
    shuffle(numList, N);

    for (int i = 0; i < N; i++)
    {
        int num = numList[i];
        if (isSafe(grid, row, col, num))
        {
            grid[row][col] = num;
            if (solveSudokuUnique(grid, row, col + 1, count))
            {
                return true;
            }
            grid[row][col] = 0;
        }
    }
    return false;
}

void CopySudokuGrid(int origin[N][N], int copyGrid[N][N]) // 复制数独
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            copyGrid[i][j] = origin[i][j];
        }
    }
}

bool hasUniqueSolution(int grid[N][N])
{
    int count = 0;
    solveSudokuUnique(grid, 0, 0, &count);
    return count == 1;
}

void generateCNF(int sudoku[N][N], const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("无法打开文件!\n");
        return;
    }

    int clauseCount = 0;

    // 写入CNF头（初始子句数为0，稍后会更新）
    fprintf(file, "c\nc Created by the program\nc\np cnf %d %d\n", N * N * N, 0); // 占位符

    // 确保每个位置只能有一个数字的约束
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            // 每个格子存在至少一个数字
            bool isFirst = true; // 用于控制空格的输出
            for (int k = 1; k <= N; k++)
            {
                if (isFirst)
                {
                    fprintf(file, "%d", (r * N + c) * N + k);
                    isFirst = false;
                }
                else
                {
                    fprintf(file, " %d", (r * N + c) * N + k);
                }
            }
            fprintf(file, " 0\n");
            clauseCount++;

            // 确保每个格子只能有一个数字
            for (int k1 = 1; k1 <= N; k1++)
            {
                for (int k2 = k1 + 1; k2 <= N; k2++)
                {
                    fprintf(file, "-%d -%d 0\n", (r * N + c) * N + k1, (r * N + c) * N + k2);
                    clauseCount++;
                }
            }
        }
    }

    // 确保每行中每个数字只能出现一次
    for (int r = 0; r < N; r++)
    {
        for (int k = 1; k <= N; k++)
        {
            bool isFirst = true;
            for (int c1 = 0; c1 < N; c1++)
            {
                if (isFirst)
                {
                    fprintf(file, "%d", (r * N + c1) * N + k);
                    isFirst = false;
                }
                else
                {
                    fprintf(file, " %d", (r * N + c1) * N + k);
                }
            }
            fprintf(file, " 0\n");
            clauseCount++;

            for (int c1 = 0; c1 < N; c1++)
            {
                for (int c2 = c1 + 1; c2 < N; c2++)
                {
                    fprintf(file, "-%d -%d 0\n", (r * N + c1) * N + k, (r * N + c2) * N + k);
                    clauseCount++;
                }
            }
        }
    }

    // 确保每列中每个数字只能出现一次
    for (int c = 0; c < N; c++)
    {
        for (int k = 1; k <= N; k++)
        {
            bool isFirst = true;
            for (int r1 = 0; r1 < N; r1++)
            {
                if (isFirst)
                {
                    fprintf(file, "%d", (r1 * N + c) * N + k);
                    isFirst = false;
                }
                else
                {
                    fprintf(file, " %d", (r1 * N + c) * N + k);
                }
            }
            fprintf(file, " 0\n");
            clauseCount++;

            for (int r1 = 0; r1 < N; r1++)
            {
                for (int r2 = r1 + 1; r2 < N; r2++)
                {
                    fprintf(file, "-%d -%d 0\n", (r1 * N + c) * N + k, (r2 * N + c) * N + k);
                    clauseCount++;
                }
            }
        }
    }

    // 确保每个3x3子方块中每个数字只能出现一次
    for (int blockRow = 0; blockRow < 3; blockRow++)
    {
        for (int blockCol = 0; blockCol < 3; blockCol++)
        {
            for (int k = 1; k <= N; k++)
            {
                bool isFirst = true;
                for (int r = 0; r < 3; r++)
                {
                    for (int c = 0; c < 3; c++)
                    {
                        int rowIndex = blockRow * 3 + r;
                        int colIndex = blockCol * 3 + c;
                        if (isFirst)
                        {
                            fprintf(file, "%d", (rowIndex * N + colIndex) * N + k);
                            isFirst = false;
                        }
                        else
                        {
                            fprintf(file, " %d", (rowIndex * N + colIndex) * N + k);
                        }
                    }
                }
                fprintf(file, " 0\n");
                clauseCount++;

                for (int i = 0; i < 3; i++)
                {
                    for (int j = i + 1; j < 3; j++)
                    {
                        for (int r1 = 0; r1 < 3; r1++)
                        {
                            for (int c1 = 0; c1 < 3; c1++)
                            {
                                int rowIndex1 = blockRow * 3 + r1;
                                int colIndex1 = blockCol * 3 + c1;
                                for (int r2 = 0; r2 < 3; r2++)
                                {
                                    for (int c2 = 0; c2 < 3; c2++)
                                    {
                                        int rowIndex2 = blockRow * 3 + r2;
                                        int colIndex2 = blockCol * 3 + c2;
                                        if (rowIndex1 != rowIndex2 || colIndex1 != colIndex2)
                                        {
                                            fprintf(file, "-%d -%d 0\n", (rowIndex1 * N + colIndex1) * N + k, (rowIndex2 * N + colIndex2) * N + k);
                                            clauseCount++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 添加数独已填入的数字的子句
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            if (sudoku[r][c] != 0)
            {
                int k = sudoku[r][c];
                fprintf(file, "%d 0\n", (r * N + c) * N + k);
                clauseCount++;
            }
        }
    }

    // 现在在文件的开头更新子句计数
    rewind(file);
    fprintf(file, "c\nc Created by the program\nc\np cnf %d %d\n", N * N * N, clauseCount);

    fclose(file);
    printf("CNF文件已保存到 %s\n", filename);
}

void SudokuMain()
{
    printf("----请选择游戏难度：\n");
    printf("---- >> 1 Easy(10)\t  >> 2 Moderate(25)\t  >> 3 Hard(30)  >> 4 Custom(Enter the number of holesInt)\n");
    int ch;
    scanf("%d", &ch);
    if (ch == 1)
    {
        holesInt = 10;
    }
    else if (ch == 2)
    {
        holesInt = 25;
    }
    else if (ch == 3)
    {
        holesInt = 30;
    }
    else if (ch == 4)
    {
        printf("Enter the number of holesInt: ");
        scanf("%d", &holesInt);
    }
    else
    {
        holesInt = 10;
        printf("难度参数错误，自动选择最低难度\n");
    }

    int sudokuOrigin[N][N] = {0};
    int sudokuProblem[N][N] = {0};

    srand(time(NULL));

    solveSudoku(sudokuOrigin, 0, 0);
    CopySudokuGrid(sudokuOrigin, sudokuProblem);

    bool flag_sd = true;
    while (flag_sd)
    {
        removeKDigits(sudokuProblem, holesInt);

        if (hasUniqueSolution(sudokuProblem))
        {
            flag_sd = false;
        }
        else
        {
            CopySudokuGrid(sudokuOrigin, sudokuProblem);
        }
        // 确保获取唯一解
    }



    printf("数独创建成功！打印如下：\n");
    printGrid(sudokuProblem);

    printf("数独答案打印如下：\n");
    printGrid(sudokuOrigin);

    generateCNF(sudokuProblem, "sudoku.cnf");
    printf("保存至\"sudoku.cnf\"\n");

    Clause *sudokuCNF;
    LoadFile(sudokuCNF, "sudoku.cnf");

    int Hcnt = 0;
    bool haveCheck = false;

    int fillSudoku[N][N] = {0};
    CopySudokuGrid(sudokuProblem, fillSudoku); // 复制进填入新表
    while (1)
    {
        int x, y, z;
        printf("请输入行、列、填入的数字，输入0 0 0重置，输入0 0 1退出！\n");
        scanf("%d%d%d", &x, &y, &z);
        if (x == 0 && y == 0 && z == 0)
        {
            CopySudokuGrid(sudokuProblem, fillSudoku);
            DestroyCNF(sudokuCNF);
            sudokuCNF = nullptr;
            LoadFile(sudokuCNF, "sudoku.cnf");
            Hcnt = 0;
            haveCheck = false;
            continue;
        }
        else if (x == 0 && y == 0 && z == 1)
        {
            DestroyCNF(sudokuCNF);
            sudokuCNF = nullptr;
            return;
        }
        else if (x < 1 || x > 9 || y < 1 || y > 9 || z < 1 || z > 9 || fillSudoku[x-1][y-1] != 0)
        {
            printf("输入信息有误！请重新输入\n");
            continue;
        }
        else if (!isSafe(fillSudoku, x - 1, y - 1, z))
        {
            printf("输入信息有误！此点无法放入该数字，请重新输入\n");
            continue;
        }

        fillSudoku[x-1][y-1] = z;

        printf("填入完成！现在的数独是：\n");
        printGrid(fillSudoku);

        AddSingleClauseAtFirst(sudokuCNF, ((x - 1) * N + (y - 1)) * N + z);

        Hcnt++;
        if (!haveCheck)
        {
            printf("----是否进行可行性验证?\n 1 -> y      0 -> n\n");
            // printf("注意：可行性验证每局游戏只能进行一次！！！\n");
            int ifcheck;
            scanf("%d", &ifcheck);
            if (ifcheck)
            {
                resArr = (int *)malloc(sizeof(int) * (literalCount + 1));
                for (int i = 0; i <= literalCount; i++)
                {
                    resArr[i] = 1;
                }

                branchRule = 3;
                bool res = DPLL(sudokuCNF, resArr);
                printf("DPLLFinished\n");
                if (!res)
                {
                    printf("不可满足！\n");
                }

                else
                {
                    printf("可满足！\n");
                }
            }
            haveCheck = true;
        }

        if (Hcnt == holesInt)
        {
            printf("----恭喜！填充完成！\n");
            break;
        }
        printf("\n------------------PLEASE 按回车键继续-----------------\n");
        getchar(), getchar();
    }
}
