bool isLonelyClause(Clause *&cnfS)
{
    if (cnfS)
    {
        if (!cnfS->head->next)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        printf("这个子句是nullptr！");
        return false;
    }
}

status DeleteClause(Clause *&cnfClause, Clause *thisClause)
{
    if (cnfClause == thisClause)
    {
        // 常态：删除头句子
        cnfClause = thisClause->next;
        Node *pNode1, *pNode2;
        for (pNode1 = thisClause->head; pNode1;)
        {
            pNode2 = pNode1->next;
            if (pNode1 == thisClause->head)
            {
                thisClause->head = pNode2;
            }
            free(pNode1);
            pNode1 = pNode2;
        }
        free(thisClause);
        thisClause = nullptr;
        return OK;
    }

    for (Clause *pClause = cnfClause; pClause; pClause = pClause->next)
    {
        if (pClause->next == thisClause)
        {
            // 完成整句部分
            pClause->next = thisClause->next;
        }
    }
    // 依次删除这一个子句的所有元素
    Node *pNode1, *pNode2;
    for (pNode1 = thisClause->head; pNode1;)
    {
        pNode2 = pNode1->next;
        if (pNode1 == thisClause->head)
        {
            thisClause->head = pNode2;
        }
        free(pNode1);
        pNode1 = pNode2;
    }

    free(thisClause);
    thisClause = nullptr;
    return OK;
    // return ERROR; // 找不到这样的对象
}

status DeleteNode(Clause *&thisClause, Node *thisNode)
{
    Node *pNode = thisClause->head;
    if (pNode == thisNode)
    {
        thisClause->head = thisNode->next;
    }
    else
    {
        for (pNode = thisClause->head; pNode; pNode = pNode->next)
        {
            if (pNode->next == thisNode) // 因为本质上是删除确定的一个，所以这里直接写==
            {
                pNode->next = thisNode->next;
            }
        }
    }
    free(thisNode);
    thisClause->number--;
    thisNode = nullptr;
    return OK;
}

status DeleteCNFClause(Clause *&cnfClause, Clause *&deleteClause)
{

    Clause *pClause, *tmpClause; // tmpClause 用于存储被删除语句的下一位，如果这是一个即将被删除的子句那么就可以通过这个得到下一个
    Node *pNode;
    // 当然还要防删除第一个

    int deleteValue = deleteClause->head->value;
    for (pClause = cnfClause; pClause; pClause = tmpClause) // 这样写的原因是防止删除之后找不到对象
    {
        tmpClause = pClause->next;
        for (pNode = pClause->head; pNode; pNode = pNode->next)
        {
            if (pNode->value == deleteValue) // pNode这个值刚好是Value，因为为真，整个语句为真
            {
                DeleteClause(cnfClause, pClause);
                break; // pNode不再存在，跳出循环
            }
            if (pNode->value == -deleteValue)
            {
                DeleteNode(pClause, pNode);
                break; // 同理，跳出循环
            }
        }
        // 以上内容尽可能删除了特定的单子句以及包括这个文字的其他子句以及包括这个文字的非的其他子句中的这个文字的非
    }
    return OK;
}

int GetFirstP(Clause *&cnfLink)
{
    return cnfLink->head->value;
}

int GetMaxP(Clause *&cnfLink)
{
    int maxInt;
    int *tmpArr = (int *)malloc(sizeof(int) * (literalCount * 2 + 1));
    for (int i = 0; i <= literalCount * 2 + 1; i++)
    {
        tmpArr[i] = 0;
    }

    for (Clause *pClause = cnfLink; pClause; pClause = pClause->next)
    {
        for (Node *pNode = pClause->head; pNode; pNode = pNode->next)
        {
            if (pNode->value > 0)
            {
                tmpArr[pNode->value]++;
            }
            else
            {
                tmpArr[literalCount - pNode->value]++;
            }
        }
        int maxCount = 0;
        for (int i = 1; i <= literalCount; i++)
        {
            if (tmpArr[i] > maxCount)
            {
                maxCount = tmpArr[i];
                maxInt = i;
            }
        }
        // 其他情况
        if (maxCount == 1)
        {
            for (int i = literalCount + 1; i <= literalCount * 2; i++)
            {
                if (tmpArr[i] > maxCount)
                {
                    maxCount = tmpArr[i];
                    maxInt = literalCount - i;
                }
            }
        }
    }
    free(tmpArr);
    return maxInt;
}

double *CalculateJWParaScore(Clause *&cnfLink)
{
    double *jwScores = (double *)calloc(2 * literalCount + 1, sizeof(double));

    for (Clause *pClause = cnfLink; pClause != nullptr; pClause = pClause->next)
    {
        double weight = pow(2.0, -pClause->number);

        for (Node *pNode = pClause->head; pNode != nullptr; pNode = pNode->next)
        {
            int index = pNode->value > 0 ? pNode->value : -pNode->value + literalCount;
            jwScores[index] += weight;
        }
    }
    return jwScores;
}

int GetJeroslowWangPara(Clause *&cnfLink)
{
    double *jwScores = CalculateJWParaScore(cnfLink);
    double HighestScore = 0.0;
    int resultInt = 0;

    for (int i = 1; i <= literalCount; i++)
    {
        double score = jwScores[i] + jwScores[i + literalCount];
        if (score > HighestScore)
        {
            HighestScore = score;
            resultInt = i;
        }
    }
    free(jwScores);
    return resultInt;
}

int GetOnePara(Clause *&cnfLink, int para)
{
    if (para == 1)
    {
        return GetFirstP(cnfLink);
    }
    else if (para == 2)
    {
        return GetMaxP(cnfLink);
    }
    else if (para == 3)
    {
        return GetJeroslowWangPara(cnfLink);
    }
    else
    {
        return GetFirstP(cnfLink);
    }
}

status AddSingleClauseAtFirst(Clause *&cnfLink, int value)
{
    Clause *pClause = (Clause *)malloc(sizeof(Clause));
    pClause->head = (Node *)malloc(sizeof(Node));
    pClause->head->next = nullptr;
    pClause->head->value = value;
    pClause->number = 1;
    pClause->next = cnfLink;
    cnfLink = pClause;
    return OK;
}

status CopyCnf(Clause *cnfLink, Clause *&copyCnfLink) // 复制CNF
{
    Clause *pClause, *ppClause;
    Node *pNode, *ppNode;

    copyCnfLink = (Clause *)malloc(sizeof(Clause));
    copyCnfLink->head = (Node *)malloc(sizeof(Node));
    copyCnfLink->next = nullptr;
    copyCnfLink->number = 0;
    ppNode = copyCnfLink->head;
    copyCnfLink->head->next = nullptr;

    ppClause = copyCnfLink;
    for (pClause = cnfLink, ppClause = copyCnfLink; pClause != NULL; pClause = pClause->next, ppClause = ppClause->next) // 复制当前cnf 为回溯做准备
    {
        for (pNode = pClause->head, ppNode = ppClause->head; pNode != NULL; pNode = pNode->next, ppNode = ppNode->next)
        {
            ppClause->number++;
            ppNode->value = pNode->value;
            ppNode->next = (Node *)malloc(sizeof(Node));
            ppNode->next->next = NULL;
            if (pNode->next == NULL)
                free(ppNode->next), ppNode->next = NULL;
        }
        ppClause->next = (Clause *)malloc(sizeof(Clause));
        ppClause->next->head = (Node *)malloc(sizeof(Node));
        ppClause->next->next = NULL;
        ppClause->next->head->next = NULL;
        ppClause->next->number = 0;
        if (pClause->next == NULL)
            free(ppClause->next->head), free(ppClause->next), ppClause->next = NULL;
    }
    // for (pClause = cnfLink; pClause != nullptr; pClause = pClause->next)
    // {
    //     for (pNode = pClause->head; pNode; pNode = pNode->next)
    //     {
    //         ppNode->value = pNode->value;
    //         if (pNode->next)
    //         {
    //             ppNode->next = (Node *)malloc(sizeof(Node));
    //             ppNode->next->next = nullptr;
    //         }
    //         ppNode = ppNode->next;
    //     }
    //     if (pClause->next)
    //     {
    //         ppClause->next = (Clause *)malloc(sizeof(Clause));
    //         ppClause->next->head = (Node *)malloc(sizeof(Node));
    //         ppClause->next->number = 0;
    //         ppClause->next->head->next = nullptr;
    //         ppClause->next->next = nullptr;
    //     }
    //     ppClause = ppClause->next;
    //     if (ppClause)
    //     {
    //         ppNode = ppClause->head;
    //     }
    // }
    return OK;
}

status DestroyCNF(Clause *&currentCNF)
{
    // ppc = cnf;
    // while (ppc)
    // {
    //     pn1 = ppc->head;   //依次删除子句
    //     while (pn1)
    //         pn2 = pn1->next, free(pn1), pn1 = pn2;  //依次删除文字
    //     pc2 = ppc->next, free(ppc), ppc = pc2;
    // }
    // cnf = NULL;

    Clause *pClauseP, *pClauseA;
    Node *pNodeP, *pNodeA;
    pClauseP = currentCNF;
    while (pClauseP)
    {
        pNodeP = pClauseP->head;
        while (pNodeP)
        {
            pNodeA = pNodeP->next;
            free(pNodeP);
            pNodeP = pNodeA;
        }
        pClauseA = pClauseP->next;
        free(pClauseP);
        pClauseP = pClauseA;
    }
    currentCNF = nullptr;

    // for (pClauseP = currentCNF; pClauseP; pClauseP = pClauseA)
    // {
    //     pClauseA = pClauseP->next;
    //     for (pNodeP = pClauseP->head; pNodeP; pNodeP = pNodeA)
    //     {
    //         pNodeA = pNodeP->next;
    //         free(pNodeP);
    //     }
    //     free(pClauseP);
    // }
    return OK;
}

bool HaveEmptyClause(Clause *cnfLink)
{
    Clause *pClause = cnfLink;
    while (pClause)
    {
        if (!pClause->head)
        {
            return true;
        }

        pClause = pClause->next;
    }
    return false;
}

bool DPLL(Clause *&cnfLink, int *resultArray)
{
    bool findLonelyClause = true;
    Clause *pClause;

    while (findLonelyClause)
    {
        findLonelyClause = false;
        pClause = cnfLink; // 每一次都干脆从开头开始，避免回溯的麻烦

        // 检测是否被全部删完
        if (!cnfLink)
        {
            // printf("第%d次进入终点，一次DPLL进行完成，结果为真\n", ++testCount);
            return true;
        }
        else if (HaveEmptyClause(cnfLink)) // cnfLink存在空，表示此式子无解
        {
            // printf("第%d次进入终点，一次DPLL进行完成，结果为假\n", ++testCount);
            return false;
        }
        while (pClause && !isLonelyClause(pClause))
        {
            pClause = pClause->next;
        }
        if (pClause) // 单子句存在
        {
            // 则考虑其正负，正则赋值为1，负则赋值为0
            if (pClause->head->value > 0)
            {
                resultArray[pClause->head->value] = 1;
            }
            else
            {
                resultArray[-pClause->head->value] = 0;
            }
            // 找到后删除相关子句
            DeleteCNFClause(cnfLink, pClause);
            // 继续寻找单子句
            findLonelyClause = true;
        }
    }

    // 没有完全删除完毕，进入下一阶段

    // 选取合适的文字
    int chosenValue;
    chosenValue = GetOnePara(cnfLink, branchRule);
    // 因为假设会影响其他元，需要对原本的链表进行复制一份
    Clause *copyCnfLink;
    CopyCnf(cnfLink, copyCnfLink);
    // 进行Add
    AddSingleClauseAtFirst(copyCnfLink, chosenValue);
    bool DPLLResult = DPLL(copyCnfLink, resultArray);
    DestroyCNF(copyCnfLink);
    if (DPLLResult)
    {
        return true;
    }
    // 结束，删除上述结果

    CopyCnf(cnfLink, copyCnfLink);
    AddSingleClauseAtFirst(copyCnfLink, -chosenValue);
    DPLLResult = DPLL(copyCnfLink, resultArray);
    DestroyCNF(copyCnfLink);
    return DPLLResult;
}
