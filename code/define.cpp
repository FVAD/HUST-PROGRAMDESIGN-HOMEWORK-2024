#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define OK 1
#define ERROR 0
#define N 9
#define MAX_CLAUSES 1000000

typedef int status;
int inf = 0x3f3f3f3f; // 设定错误值
int branchRule = 1;
int holesInt;
int *resArr;
int testCount;
int literalCount, clauseCount;
double timeA, timeB;

typedef struct Node
{
    int value;
    // bool isDeleted = false;
    Node *next;
} Node; // 目的是避免删除链表节点（改了）
typedef struct Clause
{
    int number;
    // int invisibleNumber; // 根据isDeleted算实际的number
    Node *head;
    Clause *next;
} Clause;

// Functions
int Read(FILE *fp);
status LoadFile(Clause *&headClause, char fileName[]);
status PrintFileTest(Clause *&headClause, char fileName[]);
status PrintSATToFile(int *resultArr, char fileName[], bool DPLLResult, double time1, double time2);
bool isLonelyClause(Clause *&cnfS);
status DeleteClause(Clause *&cnfClause, Clause *thisClause);
status DeleteNode(Clause *&thisClause, Node *thisNode);
status DeleteCNFClause(Clause *&cnfClause, Clause *&deleteClause);
int GetFirstP(Clause *&cnfLink);
int GetMaxP(Clause *&cnfLink);
double *CalculateJWParaScore(Clause *&cnfLink);
int GetJeroslowWangPara(Clause *&cnfLink);
int GetOnePara(Clause *&cnfLink, int para);
status AddSingleClauseAtFirst(Clause *&cnfLink, int value);
status CopyCnf(Clause *cnfLink, Clause *&copyCnfLink);
status DestroyCNF(Clause *&currentCNF);
bool HaveEmptyClause(Clause *cnfLink);
bool DPLL(Clause *&cnfLink, int *resultArray);

void shuffle(int *array, int size);
bool isSafe(int grid[N][N], int row, int col, int num);
bool solveSudoku(int grid[N][N], int row, int col);
void printGrid(int grid[N][N]);
void removeKDigits(int grid[N][N], int k);
bool solveSudokuUnique(int grid[N][N], int row, int col, int *count);
void CopySudokuGrid(int origin[N][N], int copyGrid[N][N]);
bool hasUniqueSolution(int grid[N][N]);

void generateCNF(int sudoku[N][N], const char *filename);
void SudokuMain();
