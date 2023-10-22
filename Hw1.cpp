#include <iostream>
#include <fstream>
#define MAX_ROW 100
#define MAX_COL 100

using namespace std;

struct Node
{
public:
    struct Node *left;
    struct Node *right;
    struct Node *up;
    struct Node *down;
    struct Node *column;
    int rowID;
    int colID;
    int nodeCount;
};

struct Node *header = new Node();

struct Node Matrix[MAX_ROW][MAX_COL];

bool ProbMat[MAX_ROW][MAX_COL];

vector<struct Node *> solutions;

vector<struct Node *> minSolutions;

int minCost = 100;

int cardinality = 0;

int setCount = 0;

int nRow = 0, nCol = 0;

string inputFileName;

vector<int> weight;

int getRight(int i) { return (i + 1) % nCol; }
int getLeft(int i) { return (i - 1 < 0) ? nCol - 1 : i - 1; }
int getUp(int i) { return (i - 1 < 0) ? nRow : i - 1; }
int getDown(int i) { return (i + 1) % (nRow + 1); }

Node *createToridolMatrix()
{
    for (int i = 0; i <= nRow; i++)
    {
        for (int j = 0; j < nCol; j++)
        {

            if (ProbMat[i][j])
            {
                int a, b;

                if (i)
                    Matrix[0][j].nodeCount += 1;

                Matrix[i][j].column = &Matrix[0][j];

                Matrix[i][j].rowID = i;
                Matrix[i][j].colID = j;

                // left pointer
                a = i;
                b = j;
                do
                {
                    b = getLeft(b);
                } while (!ProbMat[a][b] && b != j);
                Matrix[i][j].left = &Matrix[i][b];

                a = i;
                b = j;

                // right pointer
                do
                {
                    b = getRight(b);
                } while (!ProbMat[a][b] && b != j);
                Matrix[i][j].right = &Matrix[i][b];

                a = i;
                b = j;
                // up pointer
                do
                {
                    a = getUp(a);
                } while (!ProbMat[a][b] && a != i);
                Matrix[i][j].up = &Matrix[a][j];

                a = i;
                b = j;
                // down pointer
                do
                {
                    a = getDown(a);
                } while (!ProbMat[a][b] && a != i);
                Matrix[i][j].down = &Matrix[a][j];
            }
        }
    }

    // header right pointer to first column
    header->right = &Matrix[0][0];

    // header left pointer to last column
    header->left = &Matrix[0][nCol - 1];

    Matrix[0][0].left = header;
    Matrix[0][nCol - 1].right = header;
    return header;
}

Node *getMinColumn()
{
    struct Node *h = header;
    struct Node *min_col = h->right;
    h = h->right->right;
    do
    {
        if (h->nodeCount < min_col->nodeCount)
        {
            min_col = h;
        }
        h = h->right;
    } while (h != header);

    return min_col;
}

// delete choose cover

void cover(struct Node *targetNode)
{
    struct Node *row, *rightNode;

    struct Node *colNode = targetNode->column;

    colNode->left->right = colNode->right;
    colNode->right->left = colNode->left;

    for (row = colNode->down; row != colNode; row = row->down)
    {
        for (rightNode = row->right; rightNode != row; rightNode = rightNode->right)
        {
            rightNode->up->down = rightNode->down;
            rightNode->down->up = rightNode->up;

            Matrix[0][rightNode->colID].nodeCount -= 1;
        }
    }
}

// recover choose cover

void recover(struct Node *targetNode)
{
    struct Node *rowNode, *leftNode;

    struct Node *colNode = targetNode->column;

    for (rowNode = colNode->up; rowNode != colNode; rowNode = rowNode->up)
    {
        for (leftNode = rowNode->left; leftNode != rowNode; leftNode = leftNode->left)
        {
            leftNode->up->down = leftNode;
            leftNode->down->up = leftNode;

            Matrix[0][leftNode->colID].nodeCount += 1;
        }
    }
    colNode->left->right = colNode;
    colNode->right->left = colNode;
}

void printSolutions()
{
    vector<struct Node *>::iterator i;
    int w[] = {1, 1, 1, 1, 1, 1, 1, 1};
    int cost = 0;
    for (i = solutions.begin(); i != solutions.end(); i++)
    {
        cost = weight[(*i)->rowID - 1] + cost;
        cardinality++;
    }

    if (cost <= minCost)
    {
        minCost = cost;
        for (i = solutions.begin(); i != solutions.end(); i++)
        {
            minSolutions.push_back((*i));
        }
    }

    setCount++;
    // cout << "\n";
    //  cout << "cost:" << cost << "\n";
}

void printMinSolutions()
{
    cout << "Print Solutions:";
    vector<struct Node *>::iterator i;
    int cost = 0;
    for (i = minSolutions.begin(); i != minSolutions.end(); i++)
    {
        cout << (*i)->rowID << " ";
    }
    cout << "\n";
    cout << "cost:"
         << "(" << cardinality / setCount << "," << minCost << ")\n";
}

void search()
{
    struct Node *rowNode;
    struct Node *rightNode;
    struct Node *leftNode;
    struct Node *column;

    if (header->right == header)
    {
        printSolutions();
        return;
    }

    column = getMinColumn();

    cover(column);

    for (rowNode = column->down; rowNode != column; rowNode = rowNode->down)
    {
        solutions.push_back(rowNode);

        for (rightNode = rowNode->right; rightNode != rowNode; rightNode = rightNode->right)
            cover(rightNode);

        search();

        solutions.pop_back();

        column = rowNode->column;

        for (leftNode = rowNode->left; leftNode != rowNode; leftNode = leftNode->left)
            recover(leftNode);
    }

    recover(column);
}

void readFile(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage" << argv[0] << "input file" << endl;
        return;
    }

    inputFileName = argv[1];
    ifstream inputFile(inputFileName);

    if (!inputFile.is_open())
    {
        cerr << "Fail to open the input file" << endl;
        return;
    }

    vector<int> numbers;
    int num;

    while (inputFile >> num)
    {
        numbers.push_back(num);
    }
    /*
    string line;
    while (getline(inputFile, line))
    {
        cout << line << endl;
    }
    */
    inputFile.close();

    for (int i = 0; i < numbers.size(); ++i)
    {
        // cout << "Number " << i << ": " << numbers[i] << endl;
        if (i == 0)
            nRow = numbers[i];
        if (i == 1)
            nCol = numbers[i];

        if (i >= 2 && i < nCol + 2)
            weight.push_back(numbers[i]);
    }

    for (int i = 0; i <= nRow; i++)
    {
        for (int j = 0; j < nCol; j++)
        {
            if (i == 0)
                ProbMat[i][j] = true;
            else
                ProbMat[i][j] = false;
        }
    }

    for (int i = nCol + 2; i < numbers.size(); i += 2)
    {
        // cout << "Number " << i << ": " << numbers[i] << endl;
        // cout << "Number " << i + 1 << ": " << numbers[i + 1] << endl;
        ProbMat[numbers[i + 1]][numbers[i] - 1] = true;
    }

    for (int i = 0; i < weight.size(); i++)
    {
        cout << "weight" << weight[i] << endl;
    }

    return;
}

int main(int argc, char *argv[])
{

    nRow = 7;
    nCol = 7;
    // initialize the promblem matrix

    readFile(argc, argv);

    /*
ProbMat[1][0] = true;
ProbMat[1][3] = true;
ProbMat[1][6] = true;
ProbMat[2][0] = true;
ProbMat[2][3] = true;
ProbMat[3][3] = true;
ProbMat[3][4] = true;
ProbMat[3][6] = true;
ProbMat[4][2] = true;
ProbMat[4][4] = true;
ProbMat[4][5] = true;
ProbMat[5][1] = true;
ProbMat[5][2] = true;
ProbMat[5][5] = true;
ProbMat[5][6] = true;
ProbMat[6][1] = true;
ProbMat[6][6] = true;
ProbMat[7][0] = true;
ProbMat[7][3] = true;

ProbMat[1][0] = true;
ProbMat[1][1] = true;
ProbMat[2][1] = true;
ProbMat[3][2] = true;
ProbMat[3][1] = true;
ProbMat[4][0] = true;
ProbMat[4][3] = true;
*/

    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j <= nCol; j++)
        {
            // if it's row 0, it consist of column
            // headers. Initialize it with 1
            if (!ProbMat[j][i])
            {
                // ProbMat[i][j] = true;
                printf("0");
            }
            else
            {
                // ProbMat[i][j] = false;
                printf("1");
            }
        }
        printf("\n");
    }
    createToridolMatrix();
    search();
    printMinSolutions();
    return 0;
}