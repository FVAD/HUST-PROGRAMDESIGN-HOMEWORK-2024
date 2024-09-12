int Read(FILE *fp) // 读取一个文件中输入的数字
{
    char ch = getc(fp);
    bool notMinus = true;
    int result = 0;
    while (ch < '0' || ch > '9')
    {
        if (ch == '-')
        {
            // 输入的值是一个负数
            notMinus = false;
            ch = getc(fp); // 读取 - 下一个
        }
        else
        {
            return inf; // 输入了不合法的字符
        }
    }
    while (ch >= '0' && ch <= '9')
    {
        result = result * 10 + ch - '0';
        ch = getc(fp);
    }
    if (ch != EOF && ch != ' ' && ch != '\n')
    {
        return inf; // 测试用例中的读取结束只可能是EOF，空格，换行符，同时下一个字符就是正确的数字或者符号
    }
    return notMinus ? result : -result;
}

status LoadFile(Clause *&headClause, char fileName[]) // 载入CNF文件并且建造链表
{
    FILE *fp = fopen(fileName, "r");
    char ch;
    while ((ch = getc(fp)) == 'c') // 检测注释行
    {
        // printf("www \n");
        while ((ch = getc(fp)) != '\n')
        {
            continue; // 该句确保读取完毕当前注释行之后在下一行开头回到上一个while以此检测下一行是否为注释行
        }
    }
    // 注释读取完毕，此时当前字符（即不是'c'的行首字符）必定为'p'，否则该文件不符合规范
    if (ch != 'p')
    {
        printf("CNF文件的'p'存在错误！\n");
        printf("%c\n", ch);
        return ERROR;
    }
    // 接下来的五个字符必定是' cnf '
    char CNF_TEST_FORMAT[6] = " cnf ";
    for (int i = 0; i < 5; i++)
    {
        if ((ch = getc(fp)) != CNF_TEST_FORMAT[i])
        {
            printf("CNF文件的'p'后方并不是' cnf '！\n");
            return ERROR;
        }
    }

    if ((literalCount = Read(fp)) == inf || (clauseCount = Read(fp)) == inf)
    {
        printf("ERROR! ' cnf '后两个计数词错误");
        return ERROR;
    }
    printf("literalCount = %d, clauseCount = %d\n", literalCount, clauseCount);
    // 读取了正确的数字，现在进入造链表的过程

    headClause = (Clause *)malloc(sizeof(Clause));
    headClause->next = nullptr;
    headClause->head = (Node *)malloc(sizeof(Node));
    // headClause->invisibleNumber = 0;
    headClause->number = 0;
    headClause->head->next = nullptr;

    int readNumber;
    Clause *pClause = headClause;
    Node *pNode = headClause->head;
    for (int i = 0; i < clauseCount; i++)
    {
        // 开始读取每一行的语句，读到0进入下一行
        readNumber = Read(fp);
        if (readNumber == inf)
        {
            printf("读取每行子句时错误！错误点位：第%d行", i + 1);
            return ERROR;
        }
        while (readNumber)
        {

            pClause->number++;
            pNode->value = readNumber;
            // pNode->isDeleted = false;
            pNode->next = (Node *)malloc(sizeof(Node));

            if ((readNumber = Read(fp)) == inf)
            {
                printf("读取每行子句时错误！错误点位：第%d行", i + 1);
                return ERROR;
            }
            if (!readNumber)
            {
                pNode->next = nullptr;
            }
            pNode = pNode->next;
        }
        free(pNode); // 释放多分配的内存空间
        if (i < clauseCount - 1)
        {
            pClause->next = (Clause *)malloc(sizeof(Clause));
            pClause = pClause->next;
            pClause->number = 0;
            pClause->head = (Node *)malloc(sizeof(Node));
            pClause->next = nullptr;
            pNode = pClause->head;
            // 两个指针下移
        }
        else
        {
            pClause->next = nullptr;
            pNode = pClause->head;
        }
    }

    // 分配数组空间
    resArr = (int *)malloc(sizeof(int) * (literalCount + 1));
    for (int i = 0; i <= literalCount; i++)
        resArr[i] = 1;
    fclose(fp);
    return OK;
}

status PrintFileTest(Clause *&headClause, char fileName[])
{
    FILE *fp = fopen(fileName, "w");
    for (Clause *pClause = headClause; pClause; pClause = pClause->next)
    {
        for (Node *pNode = pClause->head; pNode; pNode = pNode->next)
        {
            fprintf(fp, "%d ", pNode->value);
        }
        fprintf(fp, "\n");
    }
    return OK;
}

status PrintSATToFile(int *resultArr, char fileName[], bool DPLLResult, double time1, double time2)
{
    FILE *fp = fopen(fileName, "w+");

    fprintf(fp, "s %d\n", DPLLResult ? 1 : 0);
    if (DPLLResult)
    {
        fprintf(fp, "v ");
        for (int i = 1; i <= literalCount; i++)
        {
            // printf("the name %d's value is %s\n", i, resultArr[i] == 1 ? "true":"false");
            // fprintf(fp, "变元%d，对应的值是%s\n", i, resultArr[i] == 1 ? "true" : "false");
            fprintf(fp, "%s%d ", resultArr[i] == 1 ? "" : "-", i);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "t branchRuleA cost: %lf branchRuleB cost: %lf", time1, time2);
    fclose(fp);
    return true;
}