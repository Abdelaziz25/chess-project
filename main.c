#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
bool validTo(char from[], char to[], int player_turn, bool test);
bool validFrom(char from[], int player_turn);
void move(char from[], char to[], int player_turn);
bool check(int player_turn);
int count;
int pieceofcheck[2];
int convertCharToNumber(char c);
bool testStalemate = false;
bool namesaved = false;
char copyBoard[8][8];
bool copy_kingMoved[2];
bool copy_rookMoved[2][2];
unsigned char boardPawnNo[2] = {(char)8, (char)8};
unsigned char boardKnightNo[2] = {(char)2, (char)2};
unsigned char boardRookNo[2] = {(char)2, (char)2};
unsigned char boardBishopNo[2] = {(char)2, (char)2};
unsigned char boardQueenNo[2] = {(char)1, (char)1};
char chess[8][8] =
{
    {'r', 'n', 'b', 'k', 'q', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {'.', '-', '.', '-', '.', '-', '.', '-'},
    {'-', '.', '-', '.', '-', '.', '-', '.'},
    {'.', '-', '.', '-', '.', '-', '.', '-'},
    {'-', '.', '-', '.', '-', '.', '-', '.'},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R'},
};
// castling global variables
bool kingMoved[2] = {false, false};
bool rookMoved[2][2] =
{
    {false, false},
    {false, false}
};
//global variables that help us in enpassant special case
bool enpassant = false;
int enpassant_turn = 0;
int enpassant_x;
int enpassant_y;
//Undo & Redo structures
typedef struct sllist
{
    char chess_board[8][8];
    bool KingMoved[2];
    bool RookMoved[2][2];
    int Enpassant_x;
    int Enpassant_y;
    int Enpassant_turn;
    bool Enpassant;
    unsigned char PawnNo[2];
    unsigned char KnightNo[2];
    unsigned char RookNo[2];
    unsigned char BishopNo[2];
    unsigned char QueenNo[2];
    struct sllist *next;
}
node;
node *Redo_list = NULL;
node *Undo_list = NULL;
// It is a function that is used to add undo node so that this node can be used in undo function to make the undo variables that current variables of chessboard
void add_UndoNode()
{
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        printf("No space in memory\n");
        return;
    }
    // copying chessboard to the undo node
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            n->chess_board[i][j] = copyBoard[i][j];
        }
    }
    // copying castling variables to the undo node
    n->KingMoved[0] = copy_kingMoved[0];
    n->KingMoved[1] = copy_kingMoved[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            n->RookMoved[i][j] = copy_rookMoved[i][j];
        }
    }
    // copying enpassant variables to the undo node
    n->Enpassant_x = enpassant_x;
    n->Enpassant_y = enpassant_y;
    n->Enpassant_turn = enpassant_turn - 1;
    n->Enpassant = enpassant;
    // copying the number of pieces including the eaten one to the undo node
    for (int i = 0; i < 2; i++)
    {
        n->PawnNo[i] = boardPawnNo[i];
        n->QueenNo[i] = boardQueenNo[i];
        n->KnightNo[i] = boardKnightNo[i];
        n->RookNo[i] = boardRookNo[i];
        n->BishopNo[i] = boardBishopNo[i];
    }
    n->next = Undo_list;
    Undo_list = n;
}
// It is the function that is used to undo one turn
bool undo()
{
    // see if there is no undo steps
    if (Undo_list == NULL)
    {
        printf("There is no undo steps\n");
        return false;
    }
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        printf("No space in memory\n");
        return false;
    }
    // copying chessboard to the redo node
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            n->chess_board[i][j] = copyBoard[i][j];
        }
    }
    // copying castling variables to the redo node
    n->KingMoved[0] = copy_kingMoved[0];
    n->KingMoved[1] = copy_kingMoved[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            n->RookMoved[i][j] = copy_rookMoved[i][j];
        }
    }
    // copying enpassant variables to the redo node
    n->Enpassant_x = enpassant_x;
    n->Enpassant_y = enpassant_y;
    n->Enpassant_turn = enpassant_turn - 1;
    n->Enpassant = enpassant;
    // copying the number of pieces including the eaten one to the redo node
    for (int i = 0; i < 2; i++)
    {
        n->PawnNo[i] = boardPawnNo[i];
        n->QueenNo[i] = boardQueenNo[i];
        n->KnightNo[i] = boardKnightNo[i];
        n->RookNo[i] = boardRookNo[i];
        n->BishopNo[i] = boardBishopNo[i];
    }
    n->next = Redo_list;
    Redo_list = n;
    // copying chessboard from the last node in Undo_list to the current chessboard variables
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            chess[i][j] = Undo_list->chess_board[i][j];
        }
    }
    // copying castling variables from the last node in Undo_list to the current chessboard variables
    kingMoved[0] = Undo_list->KingMoved[0];
    kingMoved[1] = Undo_list->KingMoved[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            rookMoved[i][j] = Undo_list->RookMoved[i][j];
        }
    }
    // copying enpassant variables from the last node in Undo_list to the current chessboard variables
    enpassant_x = Undo_list->Enpassant_x;
    enpassant_y = Undo_list->Enpassant_y;
    enpassant_turn = Undo_list->Enpassant_turn;
    enpassant = Undo_list->Enpassant;
    // copying the number of pieces including the eaten one from the last node in Undo_list to the current chessboard variables
    for (int i = 0; i < 2; i++)
    {
        boardPawnNo[i] = Undo_list->PawnNo[i];
        boardQueenNo[i] = Undo_list->QueenNo[i];
        boardKnightNo[i] = Undo_list->KnightNo[i];
        boardRookNo[i] = Undo_list->RookNo[i];
        boardBishopNo[i] = Undo_list->BishopNo[i];
    }
    node *tmp = Undo_list -> next;
    // freeing last node in Undo_list
    free(Undo_list);
    Undo_list = tmp;
    return true;
}
// It is the function that is used to undo one turn
bool redo()
{
    // see if there is redo steps
    if (Redo_list == NULL)
    {
        printf("There is no Redo steps\n");
        return false;
    }
    // Moving all chessboard variable to last node of Redo_list
    add_UndoNode();
    // copying chessboard from the last node in Redo_list to the current chessboard variables
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            chess[i][j] = Redo_list->chess_board[i][j];
        }
    }
    // copying castling variables from the last node in Redo_list to the current chessboard variables
    kingMoved[0] = Redo_list->KingMoved[0];
    kingMoved[1] = Redo_list->KingMoved[1];
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            rookMoved[i][j] = Redo_list->RookMoved[i][j];
        }
    }
    // copying enpassant variables from the last node in Redo_list to the current chessboard variables
    enpassant_x = Redo_list->Enpassant_x;
    enpassant_y = Redo_list->Enpassant_y;
    enpassant_turn = Redo_list->Enpassant_turn;
    enpassant = Redo_list->Enpassant;
    // copying the number of pieces including the eaten one from the last node in Redo_list to the current chessboard variables
    for (int i = 0; i < 2; i++)
    {
        boardPawnNo[i] = Redo_list->PawnNo[i];
        boardQueenNo[i] = Redo_list->QueenNo[i];
        boardKnightNo[i] = Redo_list->KnightNo[i];
        boardRookNo[i] = Redo_list->RookNo[i];
        boardBishopNo[i] = Redo_list->BishopNo[i];
    }
    // freeing last node in Redo_list
    node *tmp = Redo_list -> next;
    free(Redo_list);
    Redo_list = tmp;
    return true;
}
// It is the function that is used to remove all undo nodes in Undo_list
void removeUndo()
{
    while (Undo_list != NULL)
    {
        node *tmp = Undo_list -> next;
        free(Undo_list);
        Undo_list = tmp;
    }
}
// It is the function that is used to remove all redo nodes in Redo_list
void removeRedo()
{
    while (Redo_list != NULL)
    {
        node *tmp = Redo_list -> next;
        free(Redo_list);
        Redo_list = tmp;
    }
}
//function to see if square index for example "A3" if it is written in correct form
bool validSt(char s[])
{
    if((s[0] >= 'A' && s[0] <= 'H') || (s[0] >= 'a' && s[0] <= 'h'))
    {
        if(s[1] <= '8' && s[1] >= '1' )
        {
            return true;
        }
    }
    return false;
}
char  convertnumbertochar(int x)
{
    /// This function convert number to characters.
    char r = (char)( x + 49);
    if(r <= '8' && r >= '1')
    {
        return r;
    }
    return '!';
}
bool rook2(int i , int j, char rook, char queen)
{
    /// this function check if there is an opponent rook and queen make check on the king.
    int n;
     /// this check vertical down squares if there is an opponent rook or queen and it return check neither rook or queen it return false.
    for(n = i+1; n < 8; n++)
    {
        if(chess[n][j] == rook || chess[n][j] == queen)
        {
            pieceofcheck[0] = n ;
            pieceofcheck[1] = j;
            return true;
        }
        else if((chess[n][j] > 'a' && chess[n][j] < 'z')||(chess[n][j] > 'A' && chess[n][j] < 'Z'))
        {
            break;
        }

    }
    /// this check vertical up squares if there is an opponent rook or queen and it return check neither rook or queen it return false.
    int m;
    for(m = i - 1; m > -1;  m--)
    {
        if(chess[m][j] == rook || chess[m][j]== queen)
        {
            pieceofcheck[0] = m;
            pieceofcheck[1] = j;
            return true;
        }
        else if ((chess[m][j] > 'a' && chess[m][j] < 'z')||(chess[m][j] > 'A' && chess[m][j] < 'Z'))
        {
            break;
        }
    }
    /// this check horizontal right squares if there is an opponent rook or queen and it return check neither rook or queen it return false.
    int x;
    int y;
    for(x = j + 1; x < 8; x++)
    {
        if(chess[i][x] == rook || chess[i][x] == queen)
        {
            pieceofcheck[0] = i;
            pieceofcheck[1] = x;
            return true;
        }
        else if ((chess[i][x] > 'a' && chess[i][x] < 'z')||(chess[i][x] > 'A' && chess[i][x] < 'Z'))
        {
            break;
        }
    }
    /// this check horizontal left squares if there is an opponent rook or queen and it return check neither rook or queen it return false.
    for(y = j - 1; y > -1; y--)
    {
        if(chess[i][y] == rook || chess[i][y] == queen)
        {
            pieceofcheck[0] = i;
            pieceofcheck[1] = y;
            return true;
        }
        else if ((chess[i][y] > 'a' && chess[i][y] < 'z')||(chess[i][y] > 'A' && chess[i][y] < 'Z'))
        {
            break;
        }
    }
    return false;
}
bool bishop2(int i , int j, char bishop, char queen)
{
    ///  this function is used to check square diagonals of opponent bishop or queen.
    int count2;
    int help1 = 0;
    int help2 = 0;
    int help3 = 0;
    int help4 = 0;

    for(count2 = 1; count2 < 8; count2++)
    {
        /// this check  top left squares if there is an opponent bishop or queen neither it return false.
        if(0 <= i - count2 && i - count2 < 8 && j - count2 < 8 && j - count2 >= 0 && help1 == 0)
        {
            if(chess[i - count2][j - count2] == bishop || chess[ i - count2 ][ j - count2 ] == queen)
            {
                pieceofcheck[0] = i - count2;
                pieceofcheck[1] = j - count2;
                return true;
            }
            else if ((chess[ i - count2 ][ j - count2 ] > 'a' && chess[ i - count2 ][ j - count2 ] < 'z')||(chess[ i - count2 ][ j - count2 ] > 'A' && chess[ i - count2 ][ j - count2 ] < 'Z'))
            {
                help1 = 1;
            }
        }
        /// this check bottom left  squares if there is an opponent bishop or queen neither it return false.
        if(0 <= i + count2 && i + count2 < 8 && j - count2 < 8 && j - count2 >= 0 && help2 == 0)
        {
            if(chess[i+count2][j-count2]== bishop || chess[i+count2][j-count2] == queen)
            {
                pieceofcheck[0] = i + count2;
                pieceofcheck[1] = j - count2;
                return true;
            }
            else if ((chess[i+count2][j-count2] > 'a' && chess[i+count2][j-count2] < 'z')||(chess[i+count2][j-count2] > 'A' && chess[i+count2][j-count2] < 'Z'))
            {
                help2 = 1;
            }
        }
        /// this check bottom right squares if there is an opponent bishop or queen neither it return false.
        if(0 <= i + count2 && i + count2 < 8 && j + count2 < 8 && j + count2 >= 0 && help3 == 0)
        {
            if(chess[i+count2][j+count2] == bishop || chess[i+count2][j+count2] == queen)
            {
                pieceofcheck[0] = i + count2;
                pieceofcheck[1] = j + count2;
                return true;
            }
            else if ((chess[i+count2][j+count2] > 'a' && chess[i+count2][j+count2] < 'z')||(chess[i+count2][j+count2] > 'A' && chess[i+count2][j+count2] < 'Z'))
            {
                help3 = 1;
            }
        }
         /// this check top right squares if there is an opponent bishop or queen neither it return false.
        if(0 <= i - count2 && i-count2 < 8 && j + count2 < 8 && j + count2 >= 0 && help4 == 0)
        {
            if(chess[i-count2][j+count2] == bishop || chess[i-count2][j+count2] == queen)
            {
                pieceofcheck[0] = i - count2;
                pieceofcheck[1] = j + count2;
                return true;
            }
            else if ((chess[i-count2][j+count2] > 'a' && chess[i-count2][j+count2] < 'z')||(chess[i-count2][j+count2] > 'A' && chess[i-count2][j+count2] < 'Z'))
            {
                help4 = 1;
            }
        }
        if(help1 == 1 && help2 == 1 && help3 == 1 && help4 == 1)
        {
            break;
        }
    }
    return false;
}
bool knights2(int i, int j, char knight)
{
    /// this function is used to check if there any opponent knight make check on the king.
    int h = i + 1;
    int v = j + 2;
    /// this check if there is an opponent knight in bottom right neither it return false.
    if(chess[h][v] == knight )
    {
        pieceofcheck[0] = h;
        pieceofcheck[1] = v;
        return true;
    }
    int h1 = i - 1;
    int v1 = j + 2;
    /// this check if there is an opponent knight in top right neither it return false.
    if(chess[h1][v1] == knight )
    {
        pieceofcheck[0] = h1;
        pieceofcheck[1] = v1;
        return true;
    }
    int h2 = i - 1;
    int v2 = j - 2;
    /// this check if there is an opponent knight in top left neither it return false.
    if(chess[h2][v2] == knight )
    {
        pieceofcheck[0] = h2;
        pieceofcheck[1] = v2;
        return true;
    }
    int h3 = i + 1;
    int v3 = j - 2;
    /// this check if there is an opponent knight in bottom left neither it return false.
    if(chess[h3][v3] == knight )
    {
        pieceofcheck[0] = h3;
        pieceofcheck[1] = v3;
        return true;
    }
    int h4 = i + 2;
    int v4 = j + 1;
    /// this check if there is an opponent knight in bottom right neither it return false.
    if(chess[h4][v4] == knight )
    {
        pieceofcheck[0] = h4;
        pieceofcheck[1] = v4;
        return true;
    }
    int h5 = i + 2;
    int v5 = j - 1;
    /// this check if there is an opponent knight in bottom left neither it return false.
    if(chess[h5][v5] == knight )
    {
        pieceofcheck[0] = h5;
        pieceofcheck[1] = v5;
        return true;
    }
    int h6= i - 2;
    int v6= j + 1;
    /// this check if there is an opponent knight in top right neither it return false.
    if(chess[h6][v6] == knight )
    {
        pieceofcheck[0] = h6;
        pieceofcheck[1] = v6;
        return true;
    }
    int h7 = i - 2;
    int v7 = j - 1;
    /// this check if there is an opponent knight in top left neither it return false.
    if(chess[h7][v7] == knight )
    {
        pieceofcheck[0] = h7;
        pieceofcheck[1] = v7;
        return true;
    }
   return false;
}
bool pawn2(int i,int j , char pawn )
{
   ///  this function is used to check if there is any opponent pawn make check on the king.
   /// check for black pawns.
    if(pawn == 'P')
    {
        int x1 = i + 1;
        int y1 = j - 1 ;
        /// this check if there pawn bottom left make check on the king.
        if(chess[x1][y1] == pawn)
        {
            pieceofcheck[0] = x1;
            pieceofcheck[1] = y1;
            return true;
        }
        int x2 = i + 1;
        int y2 = j + 1;
         /// this check if there pawn bottom right make check on the king.
        if(chess[x2][y2] == pawn)
        {
            pieceofcheck[0] = x2;
            pieceofcheck[1] = y2;
            return true;
        }
    }
    /// check for white pawns.
    else
    {
        int x3 = i - 1;
        int y3 = j + 1;
         /// this check if there pawn top left make check on the king.
        if(chess[x3][y3] == pawn)
        {
            pieceofcheck[0] = x3;
            pieceofcheck[1] = y3;
            return true;
        }
        int x4 = i - 1;
        int y4 = j - 1;
         /// this check if there pawn bottom top right make check on the king.
        if(chess[x4][y4] == pawn)
        {
            pieceofcheck[0] = x4;
            pieceofcheck[1] = y4;
            return true;
        }
    }
    return false;
}
bool king2(int i, int j, char king)
{
    /// this function king is used to check if there is an opponent king make check on the king.
    /// check for black king.
    if(king == 'K')
    {
        int x;
        x = i + 1;
        int y = j + 1;
        /// check if there an opponent king bottom right make check on the king.
        if(chess[x][y] == king)
        {
            pieceofcheck[0] = x;
            pieceofcheck[1] = y;
            return true;
        }
        int x1 = i + 1;
        int y1 = j - 1;
        /// check if there an opponent king bottom left make check on the king.
        if(chess[x1][y1] == king)
        {
            pieceofcheck[0] = x1;
            pieceofcheck[1] = y1;
            return true;
        }
        int x2 = i - 1;
        int y2 = j - 1;
        /// check if there an opponent king top left make check on the king.
        if(chess[x2][y2] == king)
        {
            pieceofcheck[0] = x2;
            pieceofcheck[1] = y2;
            return true;
        }
        int x3 = i - 1;
        int y3 = j + 1;
        /// check if there an opponent king top right make check on the king.
        if(chess[x3][y3] == king)
        {
            pieceofcheck[0] = x3;
            pieceofcheck[1] = y3;
            return true;
        }
        int x4 = i;
        int y4 = j - 1;
        /// check if there an opponent king horizontal left make check on the king.
        if(chess[x4][y4] == king)
        {
            pieceofcheck[0] = x4;
            pieceofcheck[1] = y4;
            return true;
        }
        int x5 = i;
        int y5 = j + 1;
        /// check if there an opponent king horzintal right make check on the king.
        if(chess[x5][y5] == king)
        {
            pieceofcheck[0] = x5;
            pieceofcheck[1] = y5;
            return true;
        }
        int x6 = i + 1;
        int y6 = j;
        /// check if there an opponent king vertical down make check on the king.
        if(chess[x6][y6] == king)
        {
            pieceofcheck[0] = x6;
            pieceofcheck[1] = y6;
            return true;
        }
        int x7 = i - 1;
        int y7 = j;
        /// check if there an opponent king vertical up make check on the king.
        if(chess[x7][y7] == king)
        {
            pieceofcheck[0] = x7;
            pieceofcheck[1] = y7;
            return true;
        }
        return false;
    }
    else
    {
        int x;
        x= i + 1;
        int y = j + 1;
        /// check if there an opponent king bottom right make check on the king.
        if(chess[x][y] == king)
        {
            pieceofcheck[0] = x;
            pieceofcheck[1] = y;
            return true;
        }
        int x1 = i + 1;
        int y1 = j - 1;
        /// check if there an opponent king bottom left make check on the king.
        if(chess[x1][y1] == king)
        {
            pieceofcheck[0] = x1;
            pieceofcheck[1] = y1;
            return true;
        }
        int x2 = i - 1;
        int y2 = j - 1;
        /// check if there an opponent king top left make check on the king.
        if(chess[x2][y2] == king)
        {
            pieceofcheck[0] = x2;
            pieceofcheck[1] = y2;
            return true;
        }
        int x3 = i - 1;
        int y3 = j + 1;
        /// check if there an opponent king top right make check on the king.
        if(chess[x3][y3] == king)
        {
            pieceofcheck[0] = x3;
            pieceofcheck[1] = y3;
            return true;
        }
        int x4 = i;
        int y4 = j - 1;
        /// check if there an opponent king horizontal left make check on the king.
        if(chess[x4][y4] == king)
        {
            pieceofcheck[0] = x4;
            pieceofcheck[1] = y4;
            return true;
        }
        int x5 = i;
        int y5 = j + 1;
        /// check if there an opponent king horizontal right make check on the king.
        if(chess[x5][y5] == king)
        {
            pieceofcheck[0] = x5;
            pieceofcheck[1] = y5;
            return true;
        }
        int x6 = i + 1;
        int y6 = j;
        /// check if there an opponent king vertical down make check on the king.
        if(chess[x6][y6] == king)
        {
            pieceofcheck[0] = x6;
            pieceofcheck[1] = y6;
            return true;
        }
        int x7 = i - 1;
        int y7 = j;
        /// check if there an opponent king vertical up make check on the king.
        if(chess[x7][y7] == king)
        {
            pieceofcheck[0] = x7;
            pieceofcheck[1] = y7;
            return true;
        }
        return false;
    }
}
void save(int player_turn)
{
    /// this function is used to save the game .
    char file1[100];
    if(!namesaved)
    {
        printf("ENTER FILE NAME TO BE SAVED : ");
        gets(file1);
    }
    /// we use FILE and point on the file which is fl and we use fopen built in function to open the file .
    FILE*fl;
    fl = fopen(file1,"w");
    /// in this loop we save the chessboard .
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            fputc(chess[i][j],fl);
        }
    }
    /// and here we save the castling king movement.
    if(kingMoved[0] == true )
    {
        fputc('1',fl);
    }
    else
    {
        fputc('0',fl);
    }
    if(kingMoved[1] == true)
    {
        fputc('1',fl);
    }
    else
    {
        fputc('0',fl);
    }
    /// we save castling for rook.
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(rookMoved[i][j] == true)
            {
                fputc('1',fl);
            }
            else
            {
                fputc('0',fl);
            }
        }
    }
    /// we save for enpassent movement.
    if(enpassant == true)
    {
        fputc('1',fl);
    }
    else
    {
        fputc('0',fl);
    }
    if(enpassant_turn == 1)
    {
        fputc('1',fl);
    }
    else
    {
        fputc('9',fl);
    }
    /// we save enpassent index .
    fputc(convertnumbertochar(enpassant_x),fl);
    fputc(convertnumbertochar(enpassant_y),fl);
    /// we save number of pieces that are eaten in addition to not eaten.
    fputc(boardPawnNo[0],fl);
    fputc(boardBishopNo[0],fl);
    fputc(boardQueenNo[0],fl);
    fputc(boardRookNo[0],fl);
    fputc(boardKnightNo[0],fl);
    fputc(boardPawnNo[1],fl);
    fputc(boardBishopNo[1],fl);
    fputc(boardQueenNo[1],fl);
    fputc(boardRookNo[1],fl);
    fputc(boardKnightNo[1],fl);
    /// we save player turns.
     if(player_turn == 1)
    {
        fputc('1',fl);
    }
    else
    {
        fputc('2',fl);
    }
    fclose(fl);
}
bool enpassentincheck(int player_turn)
{
    /// this  function  is used to check if there is opponent pawn make check on the king and we this check can be removed by enpassent move
    int s = pieceofcheck[0];
    int d = pieceofcheck[1];
    if(player_turn == 1)
    {
        if(chess[s][d] != 'P')
        {
            return true;
        }
    }
    else
    {
        if(chess[s][d] != 'p')
        {
            return true;
        }
    }
    int count3 = 0;
    int count4 = 0;
    int white1[9][2];
    int black1[9][2];
    int n1 = 0;
    int m1 = 0;
    /// in this loop we find the index of all friendly white pawn pieces.
    for(n1 = 0; n1 < 8; n1++)
    {
        for(m1 = 0; m1 < 8; m1++)
        {
            if(chess[n1][m1] == 'p')
            {
                white1[count3][0] = n1;
                white1[count3][1] = m1;
                count3++;
            }
            /// in this loop we find the index of all friendly black pawn pieces.
            else if (chess[n1][m1] == 'P' )
            {
                black1[count4][0] = n1;
                black1[count4][1] = m1;
                count4++;
            }
        }
    }
    char a[2];
    char a1[2];
    char a2[2];
    char temp;
    temp = chess[enpassant_x][enpassant_y];
    int w[2];
    int z2;
    int z1;
    if(player_turn == 1)
    {
        if((enpassant) && enpassant_turn == 0)
        {
            /// we loop on the friendly white pawn pieces if there is any on of them can go to remove the check through enpassent move.
            for(int g = 0; g < count3; g++)
            {
                int v1 = 1;
                for(int z = 0; z < 2; z++)
                {
                    w[z] = white1[g][z];
                    a[v1] = convertnumbertochar(white1[g][z]);
                    v1--;
                }
                z2 = w[0];
                z1 = w[1];
                if(chess[z2][z1] == 'p')
                {
                    a2[0] = convertnumbertochar(enpassant_y-1);
                    a2[1] = convertnumbertochar(enpassant_x);
                    if(chess[enpassant_x][enpassant_y-1] == '.' || chess[enpassant_x][enpassant_y-1] == '-')
                    {
                        if((enpassant_x == z2 + 1 && enpassant_y-1 == z1+1) ||(enpassant_x == z2+1 && enpassant_y-1 == z1-1))
                        {
                            move(a,a2,player_turn);
                            if(!check(player_turn))
                            {
                                move(a2,a,player_turn);
                                chess[enpassant_x][enpassant_y] = temp;
                                return false;
                            }
                            else
                            {
                                move(a2,a,player_turn);
                                chess[enpassant_x][enpassant_y] = temp;
                            }
                        }
                    }
                    else
                    {
                        return true;
                    }
                }
            }

        }
        else
        {
            return true;
        }
    }
    else
    {
        if((enpassant) && enpassant_turn == 0)
        {
             /// we loop on the friendly black pawn pieces if there is any on of them can go to remove the check through enpassent move.
            for(int g = 0;g < count4; g++)
            {
                int v1 = 1;
                for(int z = 0; z < 2; z++)
                {
                    w[z] = white1[g][z];
                    a[v1] = convertnumbertochar(black1[g][z]);
                    v1--;
                }
                z2 = w[0];
                z1 = w[1];
                a2[0] = convertnumbertochar(enpassant_y-1);
                a2[1] = convertnumbertochar(enpassant_x);
                if(chess[enpassant_x][enpassant_y-1] == '.' || chess[enpassant_x][enpassant_y-1] == '-')
                {
                    if((enpassant_x == z2 - 1 && enpassant_y-1 == z1 + 1) ||(enpassant_x == z2 - 1 && enpassant_y-1 == z1 - 1))
                    {
                        if(a2[0] == '!' || a2[1] == '!')
                        {
                            break;
                        }
                        move(a,a2,player_turn);
                        if(!check(player_turn))
                        {
                            move(a2,a,player_turn);
                            chess[enpassant_x][enpassant_y] = temp;
                            return false;
                        }
                        else
                        {
                          move(a2,a,player_turn);
                          chess[enpassant_x][enpassant_y] = temp;
                        }
                    }
                }
            }
        }
        else
        {
            return true;
        }
    }
}
bool pathbetweenkingandcheck(int i,int j,int player_turn)
{
    /// this  function is used to find if there is available places between king and piece of check to remove check.
    int count1 = 0;
    int count2 = 0;
    int n;
    int m;
    int white[16][2];
    int black[16][2];
    for(n = 0; n < 8; n++)
    {
        for(m = 0; m < 8; m++)
        {
            /// we loop to find all friendly white pieces .
            if(chess[n][m] > 'a' && chess[n][m] < 'z' && chess[n][m] != 'k')
            {
                white[count1][0] = n;
                white[count1][1] = m;
                count1++;
            }
            else if(chess[n][m] > 'A' && chess[n][m] < 'Z' && chess[n][m] != 'K')
            {
                /// we loop to find all friendly black pieces .
                black[count2][0] = n;
                black[count2][1] = m;
                count2++;
            }
        }
    }
    int s = pieceofcheck[0];
    int d = pieceofcheck[1];
    char b[2];
    char To[2];
    int min1;
    int min2;
    if(player_turn == 1)
    {
        /// we loop on the index of white pieces if any of them can go between piece of check and king to remove check .
        for(int v = 0; v < count1; v++)
        {
            int l = 1;
            for(int u = 0; u < 2; u++)
            {
                b[l] = convertnumbertochar(white[v][u]);
                l--;
            }
            int min;
            /// if the index of i which is the vertical index of the king is equal to index of vertical index of piece of check.
            if(i == s)
            {
                if(j > d)
                {
                    int x11;
                    min = d;
                    x11 = d;
                    /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x11 + 1; min < j; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
                else
                {
                    min = j;
                    int x12;
                    x12 = j;
                    /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x12 + 1; min < d; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if(j == d)
            {
                if(i > s)
                {
                    /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                    min = s;
                    int x13 = min;
                    for(min = x13 + 1; min < i; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
                else
                {
                    min = i;
                    int x14 = i;
                    /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x14 + 1; min < s; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            /// if both index of king and check piece is different
            int min1;
            int min2;
            if(i > s && j > d)
            {
                min1 = s;
                min2 = d;
                int x15 = s;
                int y15 = d;
                /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x15 + 1; min1 < i; min1++)
                {
                    for(min2 = y15 + 1; min2 < j; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if(i > s && j < d)
            {
                min1 = s;
                min2 = j;
                int x16 = s;
                int y16 = j;
                /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x16 + 1; min1 < i; min1++)
                {
                    for(min2 = y16 + 1; min2 < d; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if(i < s && j < d)
            {
                min1 = i;
                min2 = j;
                int x99;
                int x98;
                x99 = min1;
                x98 = min2;
                /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x99 + 1; min1 < s; min1++)
                {
                    for(min2 = x98 + 1; min2 < d; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            if( i < s && j > d)
            {
                int min1;
                int min2;
                min1 = i;
                min2 = d;
                int x55=i;
                int y55=d;

                /// we loop to find if any of white friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x55 + 1; min1 < s;min1++)
                {
                    for(min2 = y55 + 1; min2 < j; min2++)
                    {
                        To[0] = convertnumbertochar(min2);
                        To[1] = convertnumbertochar(min1);
                        if(validFrom(b,player_turn))
                        {
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    else
    {
        for(int v = 0; v < count2; v++)
        {
            int l = 1;
            for(int u = 0; u < 2; u++)
            {
                b[l] = convertnumbertochar(black[v][u]);
                l--;
            }
            int min;
            if(i == s)
            {
                if( j > d)
                {
                    min = d;
                    int x11 = d;
                    /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x11 + 1; min < j; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
                else
                {
                    min = j;
                    int x12;
                    x12 = j;
                    /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x12 + 1; min < d; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if(j == d)
            {
                if(i > s)
                {
                    min = s;
                    int x13 = s;
                    /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x13 + 1; min < i; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
                else
                {
                    min = i;
                    int x14 = i;
                    /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                    for(min = x14 + 1; min < s; min++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[1] = convertnumbertochar(min);
                            To[0] = convertnumbertochar(s);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            /// if both index of king and check piece is different.
            int min1;
            int min2;
            if( i > s && j > d)
            {
                min1 = s;
                min2 = d;
                int x15 = s;
                int y15 = d;
                /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x15 + 1; min1 < i; min1++)
                {
                    for(min2 = y15 + 1; min2 < j; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if( i > s && j < d)
            {
                min1 = s;
                min2 = j;
                int x16 = s;
                int y16 = j;
                /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x16 + 1; min1 < i; min1++)
                {
                    for(min2 = y16 + 1; min2 < d; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if(i < s && j < d)
            {
                min1 = i;
                min2 = j;
                int x17 = i;
                int y17 = j;
                /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x17 + 1; min1 < s; min1++)
                {
                    for( min2 = y17 + 1; min2 < d; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }
                            }
                        }
                    }
                }
            }
            else if( i < s && j > d)
            {
                min1 = i;
                min2 = d;
                int x18 = i;
                int y18 = d;
                /// we loop to find if any of black friendly pieces to this place and we check if it is valid move and remove piece of check.
                for(min1 = x18 + 1; min1 < s; min1++)
                {
                    for(min2= y18 + 1; min2 < j; min2++)
                    {
                        if(validFrom(b,player_turn))
                        {
                            To[0] = convertnumbertochar(min2);
                            To[1] = convertnumbertochar(min1);
                            if(validTo(b,To,player_turn,true))
                            {
                                if(To[0] == '!' || To[1] == '!')
                                {
                                    break;
                                }
                                move(b,To,player_turn);
                                if(!check(player_turn))
                                {
                                    move(To,b,player_turn);
                                    return false;
                                }
                                else
                                {
                                    move(To,b,player_turn);
                                }

                            }
                        }
                    }
                }
            }
        }

        return true;
    }
}
bool eatcheckpiece(int player_turn)
{
    /// this function check if there is any friendly piece can eat check piece.
    check(player_turn);
    int count1 = 0;
    int count2 = 0;
    int n;
    int m;
    int white[16][2];
    int black[16][2];
    for(n = 0; n < 8; n++)
    {
        for(m = 0; m < 8; m++)
        {
            /// we loop to find friendly white pieces.
            if(chess[n][m] > 'a' && chess[n][m] < 'z' && chess[n][m] != 'k')
            {
                white[count1][0] = n;
                white[count1][1] = m;
                count1++;
            }
            else if (chess[n][m] > 'A' && chess[n][m] < 'Z'&& chess[n][m] != 'K')
            {
                /// we loop to find friendly black pieces.
                black[count2][0] = n;
                black[count2][1] = m;
                count2++;
            }
        }
    }
    int s = pieceofcheck[0];
    int d = pieceofcheck[1];
    char temp;
    temp = chess[s][d];
    char b[2];
    char To[2];
    if(player_turn == 1)
    {
        /// we loop on the index of white pieces to see if it can eat check piece and we see if it is valid move.
        for(int v = 0; v < count1; v++)
        {
            int l = 1;
            for(int u = 0; u < 2; u++)
            {
                b[l] = convertnumbertochar(white[v][u]);
                l--;
            }

            if(validFrom(b,player_turn))
            {
                To[0] = convertnumbertochar(d);
                To[1] = convertnumbertochar(s);
                if(validTo(b,To,player_turn,true))
                {
                    if(To[0] == '!' || To[1] == '!')
                    {
                        break;
                    }
                    move(b,To,player_turn);

                    if(!check(player_turn))
                    {
                        move(To,b,player_turn);
                        chess[s][d] = temp;
                        return false;
                    }
                    else
                    {
                        move(To,b,player_turn);
                        chess[s][d] = temp;
                    }
                 }
            }
        }
        return true;
    }
    else
    {
        /// we loop to see if any black piece can eat check piece and we see if it is valid move.
        for(int v = 0; v < count2; v++)
        {
            int l = 1;
            for(int u = 0; u < 2; u++)
            {
                b[l] = convertnumbertochar(black[v][u]);
                l--;
            }
            if(validFrom(b,player_turn))
            {
                To[0] = convertnumbertochar(d);
                To[1] = convertnumbertochar(s);
                if(validTo(b,To,player_turn,true))
                {
                    if(To[0] == '!' || To[1] == '!')
                    {
                        break;
                    }
                    move(b,To,player_turn);
                    if(!check(player_turn))
                    {
                        move(To,b,player_turn);
                        chess[s][d] = temp;
                        return false;
                    }
                    else
                    {
                        move(To,b,player_turn);
                        chess[s][d] = temp;
                    }
                }
            }
        }
        return true;
    }
}
bool kingmove(int i,int j,int player_turn)
{
    /// this function check if the king can move in its available pieces when the king is under check and move to its available pieces to remove check.
    char form[2];
    form[0] = convertnumbertochar(j);
    form[1] = convertnumbertochar(i);
    char to[2];
    int pieceofcheck1[2];
    pieceofcheck1[0] = pieceofcheck[0];
    pieceofcheck1[1] = pieceofcheck[1];
    int x;
    int y;
    char temp;
    int s;
    int d;
    s = pieceofcheck1[0];
    d = pieceofcheck1[1];
    temp = chess[s][d];
    /// we loop on the available places that the king can go to remove check.
    for(x = -1; x < 2; x++)
    {
        for(y = -1; y < 2; y++)
        {
            if(validFrom(form,player_turn))
            {
                to[0] = convertnumbertochar( y + j);
                to[1] = convertnumbertochar( x + i);
                if(x == 0 && y == 0 )
                {
                    continue;
                }
                if(to[0] == '!' || to[1] == '!')
                {
                    break;
                }
                if(validTo(form,to,player_turn,true))
                {
                    move(form,to,player_turn);
                    if(!check(player_turn))
                    {
                        move(to,form,player_turn);
                        chess[s][d] = temp;
                        return false;
                    }
                    else
                    {
                        move(to,form,player_turn);
                        chess[s][d] = temp;
                    }
                }
            }
         }
     }
    return true;
}
bool checkmate( int player_turn)
{
    /// function to see checkmate on the king.
    if(!check(player_turn))
    {
        return false;
    }
    bool x9 = false;
    char king ;
    int i = 0;
    int j = 0;
    /// we loop to find the king index.
    if(player_turn == 1)
    {
        king = 'k';
    }
    else
    {
        king = 'K';
    }
    for(i = 0; i < 8; i++)
    {
        for( j = 0; j < 8; j++)
        {
            if(chess[i][j] == king)
            {
                x9 = true;
                break;
            }
        }
        if(x9)
        {
            break;
        }
    }
    /// we call the functions and see if it can remove check if it return return false so no checkmate else so there is checkmate.

    if(!(kingmove(i,j,player_turn)))
    {
        return false;
    }
    else if(count > 1)
    {
        return true;
    }

    if(!(eatcheckpiece(player_turn)))
    {
        return false;
    }
    else if(!(pathbetweenkingandcheck(i,j,player_turn)))
    {
        return false;
    }

   else if(!(enpassentincheck(player_turn)))
    {
        return false;
    }
    return true;
}
bool nomove(int player_turn)
{
    /// this function see if there any friendly piece cant move so it is stale mate.
    int count1;
    int count2;
    int n = 0;
    int m = 0;
    int white3[16][2];
    int black3[16][2];
    count1 = 0;
    count2 = 0;
    for(n = 0; n < 8; n++)
    {
        for(m = 0; m < 8; m++)
        {
            /// we loop to find friendly white piece.
            if(chess[n][m] > 'a' && chess[n][m] < 'z')
            {
                white3[count1][0] = n;
                white3[count1][1] = m;
                count1++;
            }
            else if(chess[n][m] > 'A' && chess[n][m] < 'Z')
            {
                /// we loop to find friendly black piece.
                black3[count2][0] = n;
                black3[count2][1] = m;
                count2++;
            }
        }
    }
    char To[2];
    char b[2];
    char r[2];
    char TO[2];
    int s;
    int o;
    int v;
    int z;

    if (player_turn == 1)
    {

        for(int s = 0; s < count1; s++)
        {
            int l = 1;
            for(int v = 0; v < 2; v++)
            {
                b[l] = convertnumbertochar(white3[s][v]);
                l--;
            }
            /// we loop on the chessboard to see if any white piece can go to this place and we see if it is valid move .
            if(validFrom(b,player_turn))
            {
                for( o = 0; o < 8; o++)
                {
                    for( z = 0; z < 8; z++)
                    {
                        To[1] = convertnumbertochar(o);
                        To[0] = convertnumbertochar(z);
                        if(validTo(b,To,player_turn, true))
                        {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    else
    {
        int c;
        for( c = 0; c < count2; c++)
        {
            int h = 1;
            for(int k = 0; k < 2; k++)
            {
                r[h] = convertnumbertochar(black3[c][k]);
                h--;
            }
            if(validFrom(r,player_turn))
            {
                 /// we loop on the chessboard to see if any black piece can go to this place and we see if it is valid move .
                for(int n = 0; n < 8; n++)
                {
                    for(int m = 0; m < 8; m++)
                    {
                        TO[1] = convertnumbertochar(n);
                        TO[0] = convertnumbertochar(m);
                        if(TO[0] == '!' || TO[1] == '!' )
                        {
                            break;
                        }
                        if(validTo(r,TO,player_turn,false))
                        {
                            return false;
                        }
                    }
                }
            }

        }
        return true;
    }
}
bool bishoponthesamecolour()
{
    /// this function see if there are two bishops and two kings on the chess board and the two bishops are on the same color.
    bool flag2;
    flag2 = false;
    int x3;
    int x4;
    /// we loop to find the white bishop.
    for( x3 = 0; x3 < 8; x3++)
    {
        for( x4 = 0; x4 < 8; x4++)
        {
            if(chess[x3][x4] == 'b')
            {
                flag2 = true;
                break;
            }
        }
        if(flag2)
        {
            break;
        }
    }
    int x5;
    int x6;
    bool flag3;
    flag3 = false;
    /// we loop to find black bishop.
    for( x5 = 0; x5 < 8; x5++)
    {
        for( x6 = 0; x6 < 8; x6++)
        {
            if(chess[x5][x6] == 'B')
            {
                flag3 = true;
                break;
            }
        }
        if(flag3)
        {
            break;
        }
    }
    int x7;
    int x8;
    int y5 = 0;
    int y = 0;
    /// we loop to find if there is only two kings and two bishops if there is more return false.
    for(x7 = 0;x7 < 8; x7++)
    {
        for(x8 = 0; x8 < 8; x8++)
        {
            if(chess[x7][x8] == 'k'|| chess[x7][x8] == 'K' || chess[x7][x8] == 'b'|| chess[x7][x8] == 'B')
            {
                y5++;
            }
            else if((chess[x7][x8] > 'a' && chess[x7][x8] < 'z' && chess[x7][x8] != 'k'&& chess[x7][x8] != 'b') ||(chess[x7][x8] > 'A' && chess[x7][x8] < 'Z' && chess[x7][x8] != 'K'&& chess[x7][x8] != 'B'))
            {
                y++;
            }
        }
    }
    /// we see if the bishops are on the same colors.
    if(y5 == 4 && y == 0)
    {
        if(( x3 + x4 ) % 2 == 0 &&( x5 + x6 ) % 2 == 0)
        {
            return true;
        }
       else if(( x3 + x4 ) % 2 != 0 && ( x5 + x6 ) % 2 != 0)
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
        return false;
    }
}
bool oneontwo()
{
    /// this function see if there are two kings and bishop or two kings and knight then it will be stalemate.
    int x9 = 0;
    int flag = 1;
    int y4 = 0;
    /// we loop on chess board if there are two kings and bishops or two kings and knight .
    for(int a1 = 0; a1 < 8; a1++)
    {
        for(int b1 = 0; b1 < 8; b1++)
        {
            if(chess[a1][b1] == 'k' || chess[a1][b1] == 'K' || chess[a1][b1] == 'n'||chess[a1][b1] == 'N'|| chess[a1][b1] == 'b'|| chess[a1][b1] == 'B' )
            {
                x9++;
            }
            else if( ( chess[a1][b1] > 'a' && chess[a1][b1] < 'z' && chess[a1][b1] != 'k'&& chess[a1][b1] != 'b'&&chess[a1][b1] != 'n' ) ||(chess[a1][b1] > 'A' && chess[a1][b1] < 'Z' && chess[a1][b1] != 'K' && chess[a1][b1] != 'B' && chess[a1][b1] != 'N' ) )
            {
                y4++;
            }
        }
    }
    if(x9 == 3 && y4 == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool kingandking()
{
    /// this function see if there is only two kings in chessboard then it will be stalemate.
    int x = 0;
    int y = 0;
    bool x2 = false;
    /// we loop on the chessboard to find if there are two kings or more.
    for(x = 0; x < 8; x++)
    {
        for(y = 0; y < 8; y++)
        {
            if((chess[x][y] > 'a' && chess[x][y] < 'z' && chess[x][y] != 'k')||(chess[x][y] > 'A' && chess[x][y] < 'Z'&& chess[x][y] != 'K'))
            {
                x2 = true;
                break;
            }
        }
        if(x2)
        {
            break;
        }
    }
    if(!x2)
    {
        return true;
    }
}
bool stalemate(int player_turn)
{
    /// we see if there any condition of stalemate if it is true then it will be stalemate.
    testStalemate = true;
    if(check(player_turn))
    {
        return false;
    }
    if(kingandking())
    {
        return true;
    }
    if(oneontwo())
    {
        return true;
    }
    if(bishoponthesamecolour())
    {
        return true;
    }
   if(nomove(player_turn))
   {
       return true;
   }
   return false;
}
bool ischecked(char king, int i , int j,int player_turn)
{
    /// this function see if the king is in check .
    count = 0;
    int n;
    int m;
    char rook;
    char queen;
    char bishop;
    char knight;
    char pawn;
    char KING ;
    if(king == 'k')
    {
        rook = 'R';
        queen = 'Q';
        bishop = 'B';
        knight = 'N';
        pawn = 'P';
        KING = 'K';
    }
    else
    {
        rook = 'r';
        queen = 'q';
        bishop = 'b';
        knight = 'n';
        pawn = 'p';
        KING = 'k';
    }
  /// we call these function to see if the king is on check if it return true the it is check.
    if(rook2(i,j,rook,queen))
    {
        count++;
    }
    if(bishop2(i,j,bishop,queen))
    {
        count ++;
    }
    if(knights2(i,j,knight))
    {
        count++;
    }
    if(pawn2(i,j,pawn))
    {
        count++;
    }
    if(king2(i,j,KING))
    {
        count++;
    }
    if(count > 1)
    {
        return true;
    }
    else if(count == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool check(int player_turn)
{
    /// in this function we find the index of king to see if it is on check.
    char king = (player_turn == 1)? 'k':'K';
    int i;
    int j;
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
        {
            if(chess[i][j] == king )
            {
                if( ischecked(king, i, j,player_turn))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
}
//function to give an index for character to help us in indexing of string
//for example it returns 2 for B and it return 5 for 5
int convertCharToNumber(char c)
{
    if (c >= 'A' && c <= 'H')
    {
        return (int)((int)(c) - 65);
    }
    else if (c >= 'a' && c <= 'h')
    {
        return (int)((int)(c) - 97);
    }
    else
    {
        return (int)((int)(c) - 49);
    }
}
int load()
{
    /// this function used to load the game and we use the same file in which we save.
    char y,name[100];
    printf("Enter file name which you save\n");
    FILE*fl;
    int i = 0;
    int j = 0;
    while(1)
    {
        gets(name);
        fl = fopen(name,"r");
        if(fl)
        {
            break;
        }
        else
        {
            printf("not found\n");
        }
    }
    /// we loop to load the chessboard using getc.
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
        {
            y = getc(fl);
            chess[i][j] = y;
        }
    }
    /// we load castling movements rook and king.
    y = getc(fl);
    if(y == '1')
    {
        kingMoved[0] = true;
    }
    else
    {
        kingMoved[0] = false;
    }
    y = getc(fl);
    if(y == '1')
    {
        kingMoved[1] = true;
    }
    else
    {
        kingMoved[1] = false;
    }
    for(int n = 0; n < 2 ; n++)
    {
        for(int m = 0; m < 2; m++)
        {
            y = getc(fl);
            rookMoved[n][m] = y;
        }
    }
    /// we load enpassenet movement,
    y = getc(fl);
    if(y == '1')
    {
        enpassant = true;
    }
    else
    {
        enpassant = false;
    }
    y = getc(fl);
    if(y == '1')
    {
        enpassant_turn = 1;
    }
    else
    {
        enpassant_turn = 9;
    }
    /// we load enpassenet index.
    y = getc(fl);
    enpassant_x = convertCharToNumber(y);
    y = getc(fl);
    enpassant_y = convertCharToNumber(y);
    /// we load promotion pieces.
    y = getc(fl);
    boardPawnNo[0] = y;
    y = getc(fl);
    boardBishopNo[0] = y;
    y = getc(fl);
    boardQueenNo[0] = y;
    y = getc(fl);
    boardRookNo[0] = y;
    y = getc(fl);
    boardKnightNo[0] = y;
    y = getc(fl);
    boardPawnNo[1] = y;
    y = getc(fl);
    boardBishopNo[1] = y;
    y = getc(fl);
    boardQueenNo[1] = y;
    y = getc(fl);
    boardRookNo[1] = y;
    y = getc(fl);
    boardKnightNo[1] = y;
    /// we load player turns.
    y = getc(fl);
    if(y == '1')
    {
        fclose(fl);
        return  1;
    }
    else
    {
        fclose(fl);
        return 2;
    }
}
//global variable for "last from square piece" letter
//for example it will be 'p' for first player's pawn
char fromPiece;
//function that checks if "from square" is valid
//by seeing it contains one of the players pieces
bool validFrom(char from[], int player_turn)
{
    int fromX = convertCharToNumber(from[0]);
    int fromY = convertCharToNumber(from[1]);
    if (player_turn == 1)
    {
        if (chess[fromY][fromX] < 'z' && chess[fromY][fromX] > 'a')
        {
            fromPiece = chess[fromY][fromX];

            return true;
        }
    }
    else
    {
        if (chess[fromY][fromX] < 'Z' && chess[fromY][fromX] > 'A')
        {
            fromPiece = chess[fromY][fromX];
            return true;
        }
    }
    return false;
}
//check if knight can go to "to square"
bool knight(int fromX, int fromY, int toX, int toY)
{
    if (toX == fromX + 2 && toY == fromY + 1)
    {
        return true;
    }
    if (toX == fromX + 2 && toY == fromY - 1)
    {
        return true;
    }
    if (toX == fromX - 2 && toY == fromY + 1)
    {
        return true;
    }
    if (toX == fromX - 2 && toY == fromY - 1)
    {
        return true;
    }
    if (toX == fromX + 1 && toY == fromY + 2)
    {
        return true;
    }
    if (toX == fromX - 1 && toY == fromY + 2)
    {
        return true;
    }
    if (toX == fromX + 1 && toY == fromY - 2)
    {
        return true;
    }
    if (toX == fromX - 1 && toY == fromY - 2)
    {
        return true;
    }
    return false;
}
//check if bishop can go to "to square"
bool bishop(int fromX, int fromY, int toX, int toY)
{
    int j;
    for (int i = fromX + 1; i < 8; i++)
    {
        j = fromY + abs(fromX - i);
        if (j >= 8)
        {
            break;
        }
        if (toX == i && toY == j)
        {
            return true;
        }
        if (chess[j][i] != '.' && chess[j][i] != '-')
        {
            break;
        }
    }
    for (int i = fromX + 1; i < 8; i++)
    {
        j = fromY - abs(fromX - i);
        if (j < 0)
        {
            break;
        }
        if (toX == i && toY == j)
        {
            return true;
        }
        if (chess[j][i] != '.' && chess[j][i] != '-')
        {
            break;
        }
    }
    for (int i = fromX - 1; i >= 0; i--)
    {
        j = fromY + abs(fromX - i);
        if (j >= 8)
        {
            break;
        }
        if (toX == i && toY == j)
        {
            return true;
        }
        if (chess[j][i] != '.' && chess[j][i] != '-')
        {
            break;
        }
    }
    for (int i = fromX - 1; i >= 0; i--)
    {
        j = fromY - abs(fromX - i);
        if (j < 0)
        {
            break;
        }
        if (toX == i && toY == j)
        {
            return true;
        }
        if (chess[j][i] != '.' && chess[j][i] != '-')
        {
            break;
        }
    }
    return false;
}
//check if rook can go to "to square"
bool rook(int fromX, int fromY, int toX, int toY)
{
    if (fromY == toY)
    {
        for (int i = fromX + 1; i < 8; i++)
        {
            if (toX == i)
            {
                return true;
            }
            if (chess[toY][i] != '.' && chess[toY][i] != '-')
            {
                break;
            }
        }
        for (int i = fromX - 1; i >= 0; i--)
        {
            if (toX == i)
            {
                return true;
            }
            if (chess[toY][i] != '.' && chess[toY][i] != '-')
            {
                break;
            }
        }
    }
    if (fromX == toX)
    {
        for (int i = fromY + 1; i < 8; i++)
        {
            if (toY == i)
            {
                return true;
            }
            if (chess[i][toX] != '.' && chess[i][toX] != '-')
            {
                break;
            }
        }
        for (int i = fromY - 1; i >= 0; i--)
        {
            if (toY == i)
            {
                return true;
            }
            if (chess[i][toX] != '.' && chess[i][toX] != '-')
            {
                break;
            }
        }
    }
    return false;
}
//check if queen can go to "to square"
bool queen(int fromX, int fromY, int toX, int toY)
{
    return (rook(fromX, fromY, toX, toY) || bishop(fromX, fromY, toX, toY));
}
//check if pawn can go to "to square"
bool pawn(int fromX, int fromY, int toX, int toY, int player_turn)
{
    if (player_turn == 1)
    {
        if (toX == fromX && (toY == fromY + 1 || (toY == fromY + 2 && fromY == 1)))
        {
            if (chess[toY][toX] == '.' || chess[toY][toX] == '-')
            {
                if (toY == fromY + 2 && !check(player_turn)&& !testStalemate)
                {
                    enpassant = true;
                    enpassant_x = toX;
                    enpassant_y = toY;
                    enpassant_turn = 0;
                }
                return true;
            }
        }
        if ((toX == fromX + 1 || toX == fromX - 1) && toY == fromY + 1)
        {
            if (chess[toY][toX] != '.' && chess[toY][toX] != '-')
            {
                return true;
            }
            if (enpassant && enpassant_turn == 1 && toX == enpassant_x && enpassant_y + 1 == toY)
            {
                chess[enpassant_y][enpassant_x] = ((enpassant_x + enpassant_y) % 2 == 0) ? '.' : '-';
                return true;
            }
        }
    }
    else
    {
        if (toX == fromX && (toY == fromY - 1 || (toY == fromY - 2 && fromY == 6)))
        {
            if (chess[toY][toX] == '.' || chess[toY][toX] == '-')
            {
                if (toY == fromY - 2 && !check(player_turn) && !testStalemate)
                {
                    enpassant = true;
                    enpassant_x = toX;
                    enpassant_y = toY;
                    enpassant_turn = 0;
                }
                return true;
            }
        }
        if ((toX == fromX + 1 || toX == fromX - 1) && toY == fromY - 1)
        {
            if (chess[toY][toX] != '.' && chess[toY][toX] != '-')
            {
                return true;
            }
            if (enpassant && enpassant_turn == 1 && toX == enpassant_x && enpassant_y - 1 == toY)
            {
                chess[enpassant_y][enpassant_x] = ((enpassant_x + enpassant_y) % 2 == 0) ? '.' : '-';
                return true;
            }
        }
    }
    return false;
}
//check if king can go to "to square"
bool king(int fromX, int fromY, int toX, int toY, int player_turn)
{
    if ((abs(toX - fromX) == 0 || abs(toX - fromX) == 1) && (abs(toY - fromY) == 0 || abs(toY - fromY) == 1))
    {
        return true;
    }
    if (toY == 0 && fromY == 0 && abs(toX - fromX) == 2 && !kingMoved[player_turn - 1] && !check(player_turn))
    {
        int rook_direct = ((fromX - toX) > 0) ? 0 : 1;
        if (!rookMoved[player_turn - 1][rook_direct])
        {
            if (rook_direct == 0)
            {
                if (chess[0][0] != 'r')
                {
                    return false;
                }
                char first[2] = {'A', '1'};
                char second[2] = {'C', '1'};
                move(first, second, player_turn);
            }
            else
            {
                if (chess[0][7] != 'r')
                {
                    return false;
                }
                char first[2] = {'H', '1'};
                char second[2] = {'E', '1'};
                move(first, second, player_turn);
            }
            return true;
        }
    }
    if (toY == 7 && fromY == 7 && abs(toX - fromX) == 2 && !kingMoved[player_turn - 1] && !check(player_turn))
    {
        int rook_direct = ((fromX - toX) > 0) ? 0 : 1;
        if (!rookMoved[player_turn - 1][rook_direct])
        {
            if (rook_direct == 0)
            {
                if (chess[7][0] != 'R')
                {
                    return false;
                }
                char first[2] = {'A', '8'};
                char second[2] = {'C', '8'};
                move(first, second, player_turn);
            }
            else
            {
                if (chess[7][7] != 'R')
                {
                    return false;
                }
                char first[2] = {'H', '8'};
                char second[2] = {'E', '8'};
                move(first, second, player_turn);
            }
            return true;
        }
    }
    return false;
}
//function that checks if "to square" is valid
//by generating all possible values of last From square piece and see if "to square" is one of them
//it return true if it is one of them
bool validTo(char from[], char to[], int player_turn, bool test)
{
    int toX = convertCharToNumber(to[0]);
    int toY = convertCharToNumber(to[1]);
    int fromX = convertCharToNumber(from[0]);
    int fromY = convertCharToNumber(from[1]);
    // see if the player piece is going to square contains one of his pieces
    // it returns false in this case
    if (player_turn == 1)
    {
        if (islower(chess[toY][toX]))
        {
            return false;
        }
    }
    else
    {
        if (isupper(chess[toY][toX]))
        {
            return false;
        }
    }
    char Temp = chess[fromY][fromX];
    char Temp2 = chess[toY][toX];
    // testing if this function will lead to check so it is not valid move
    if(chess[fromY][fromX] != 'k' && chess[fromY][fromX] != 'K')
    {
        chess[fromY][fromX] = ((fromY + fromX) % 2 == 0) ? '.' : '-';
        if (!test)
        {
            chess[toY][toX] = Temp;
            if (check(player_turn))
            {
                chess[toY][toX] = Temp2;
                chess[fromY][fromX] = Temp;
                return false;
            }
        }
        chess[toY][toX] = Temp2;
        chess[fromY][fromX] = Temp;
    }
    else
    {
        chess[toY][toX] = Temp;
        chess[fromY][fromX] = ((fromY + fromX) % 2 == 0) ? '.' : '-';
        if(!test)
        {
        if (check(player_turn))
        {
            chess[fromY][fromX] = Temp;
            chess[toY][toX] = Temp2;
            return false;
        }
      }
        chess[fromY][fromX] = Temp;
        chess[toY][toX] = Temp2;
    }
    fromPiece=chess[fromY][fromX];
    if (toupper(fromPiece) == 'N')
    {
        return knight(fromX, fromY, toX, toY);
    }
    if (toupper(fromPiece) == 'B')
    {
        return bishop(fromX, fromY, toX, toY);
    }
    if (toupper(fromPiece) == 'R')
    {
        return rook(fromX, fromY, toX, toY);
    }
    if (toupper(fromPiece) == 'Q')
    {
        return queen(fromX, fromY, toX, toY);
    }
    if (toupper(fromPiece) == 'P')
    {

        return pawn(fromX, fromY, toX, toY, player_turn);
    }
    if (toupper(fromPiece) == 'K')
    {
        return king(fromX, fromY, toX, toY, player_turn);
    }

    return false;
}
// functions to move the piece in the from square to the to square
void move(char from[], char to[], int player_turn)
{
    int toX = convertCharToNumber(to[0]);
    int toY = convertCharToNumber(to[1]);
    int fromX = convertCharToNumber(from[0]);
    int fromY = convertCharToNumber(from[1]);
    // handling castling variable to know if any rook or king is going to move
    if (!check(player_turn))
    {
        if (chess[fromY][fromX] == 'k')
        {
            kingMoved[player_turn - 1] = true;
        }
        else if (chess[fromY][fromX] == 'K')
        {
            kingMoved[player_turn - 1] = true;
        }
        else if (chess[fromY][fromX] == 'r' && fromX == 0 && fromY == 0)
        {
            rookMoved[player_turn - 1][0] = true;
        }
        else if (chess[fromY][fromX] == 'r' && fromX == 7 && fromY == 0)
        {
            rookMoved[player_turn - 1][1] = true;
        }
        else if (chess[fromY][fromX] == 'R' && fromX == 0 && fromY == 7)
        {
            rookMoved[player_turn - 1][0] = true;
        }
        else if (chess[fromY][fromX] == 'R' && fromX == 7 && fromY == 7)
        {
            rookMoved[player_turn - 1][1] = true;
        }
        chess[toY][toX] = chess[fromY][fromX];
        chess[fromY][fromX] = ((fromY + fromX) % 2 == 0) ? '.' : '-';
    }
    else
    {
        chess[toY][toX] = chess[fromY][fromX];
        chess[fromY][fromX] = ((fromY + fromX) % 2 == 0) ? '.' : '-';
    }
}
// promotion to promote the pawns
// by seeing if there are pawns at last row in the chessboard then asking the user for the piece he want to promote his pawn to
void promotion()
{
    for (int i = 0; i < 8; i++)
    {
        if (chess[0][i] == 'P')
        {
            printf("Promotion\n");
            printf("Enter the piece you want to promote your pawn to: ");
            char promotedToPiece[3];
            boardPawnNo[1] = (char)((int)boardPawnNo[1] - 1);
            while (true)
            {
                fgets(promotedToPiece, 3, stdin);
                if (strlen(promotedToPiece) != 2)
                {
                    printf("Enter N , Q , R or B\n");
                    continue;
                }
                if (promotedToPiece[0] == 'N' || promotedToPiece[0] == 'Q' || promotedToPiece[0] == 'R' || promotedToPiece[0] == 'B')
                {
                    chess[0][i] = promotedToPiece[0];
                    if(toupper(chess[0][i]) == 'N')
                    {
                        boardKnightNo[1] = (char)((int)boardKnightNo[1] + 1);
                    }
                    else if(toupper(chess[0][i]) == 'Q')
                    {
                        boardQueenNo[1] = (char)((int)boardQueenNo[1] + 1);
                    }
                    else if(toupper(chess[0][i]) == 'R')
                    {
                        boardRookNo[1] = (char)((int)boardRookNo[1] + 1);
                    }
                    else if(toupper(chess[0][i]) == 'B')
                    {
                        boardBishopNo[1] = (char)((int)boardBishopNo[1] + 1);
                    }
                    return;
                }
                else
                {
                    printf("Enter N , Q , R or B\n");
                }
            }
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (chess[7][i] == 'p')
        {
            printf("Promotion\n");
            printf("Enter the piece you want to promote your pawn to: ");
            boardPawnNo[0] = (char)((int)boardPawnNo[0] - 1);
            char promotedToPiece[3];
            while (true)
            {
                fgets(promotedToPiece, 3, stdin);
                if (strlen(promotedToPiece) != 2)
                {
                    printf("Enter n , q , r or b\n");
                    continue;
                }
                if (promotedToPiece[0] == 'n' || promotedToPiece[0] == 'q' || promotedToPiece[0] == 'r' || promotedToPiece[0] == 'b')
                {
                    chess[7][i] = promotedToPiece[0];
                    if(toupper(chess[7][i]) == 'N')
                    {
                        boardKnightNo[0] = (char)((int)boardKnightNo[0] + 1);
                    }
                    else if(toupper(chess[7][i]) == 'Q')
                    {
                        boardQueenNo[0] = (char)((int)boardQueenNo[0] + 1);
                    }
                    else if(toupper(chess[7][i]) == 'R')
                    {
                        boardRookNo[0] = (char)((int)boardRookNo[0] + 1);
                    }
                    else if(toupper(chess[7][i]) == 'B')
                    {
                        boardBishopNo[0] = (char)((int)boardBishopNo[0] + 1);
                    }
                    return;
                }
                else
                {
                    printf("Enter n , q , r or b\n");
                }
            }
        }
    }
}
//function to print eaten pieces by counting the number of pieces of each type of every player
// then we see if one of the types of the pieces is lower than the number that was at the beginning in chessboard
// we also handle if the number pawns decreased in promotion and the number of other type pieces number increased
void print_eaten_pieces()
{
    int pawnNo[2] = {0, 0};
    int bishopNo[2] = {0, 0};
    int rookNo[2] = {0, 0};
    int knightNo[2] = {0, 0};
    int queenNo[2] = {0, 0};
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chess[i][j] == 'r')
            {
                rookNo[0]++;
            }
            else if (chess[i][j] == 'R')
            {
                rookNo[1]++;
            }
            else if (chess[i][j] == 'p')
            {
                pawnNo[0]++;
            }
            else if (chess[i][j] == 'P')
            {
                pawnNo[1]++;
            }
            else if (chess[i][j] == 'b')
            {
                bishopNo[0]++;
            }
            else if (chess[i][j] == 'B')
            {
                bishopNo[1]++;
            }
            else if (chess[i][j] == 'n')
            {
                knightNo[0]++;
            }
            else if (chess[i][j] == 'N')
            {
                knightNo[1]++;
            }
            else if (chess[i][j] == 'q')
            {
                queenNo[0]++;
            }
            else if (chess[i][j] == 'Q')
            {
                queenNo[1]++;
            }
        }
    }
    printf("Eaten white pieces: ");
    if (pawnNo[0] < (int)boardPawnNo[0])
    {
        for (int i = pawnNo[0]; i < (int)boardPawnNo[0]; i++)
        {
            printf("p ");
        }
    }
    if (rookNo[0] < (int)boardRookNo[0])
    {
        for (int i = rookNo[0]; i < (int)boardRookNo[0]; i++)
        {
            printf("r ");
        }
    }
    if (bishopNo[0] < (int)boardBishopNo[0])
    {
        for (int i = bishopNo[0]; i < (int)boardBishopNo[0]; i++)
        {
            printf("b ");
        }
    }
    if (knightNo[0] < (int)boardKnightNo[0])
    {
        for (int i = knightNo[0]; i < (int)boardKnightNo[0]; i++)
        {
            printf("n ");
        }
    }
    if (queenNo[0] < (int)boardQueenNo[0])
    {
        for (int i = queenNo[0]; i < (int)boardQueenNo[0]; i++)
        {
            printf("q ");
        }
    }
    printf("\nEaten Black Pieces: ");
    if (pawnNo[1] < (int)boardPawnNo[1])
    {
        for (int i = pawnNo[1]; i < (int)boardPawnNo[1]; i++)
        {
            printf("P ");
        }
    }
    if (rookNo[1] < (int)boardRookNo[1])
    {
        for (int i = rookNo[1]; i < (int)boardRookNo[1]; i++)
        {
            printf("R ");
        }
    }
    if (bishopNo[1] < (int)boardBishopNo[1])
    {
        for (int i = bishopNo[1]; i < (int)boardBishopNo[1]; i++)
        {
            printf("B ");
        }
    }
    if (knightNo[1] < (int)boardKnightNo[1])
    {
        for (int i = knightNo[1]; i < (int)boardKnightNo[1]; i++)
        {
            printf("N ");
        }
    }
    if (queenNo[1] < (int)boardQueenNo)
    {
        for(int i = queenNo[1]; i < (int)boardQueenNo[1]; i++)
        {
            printf("Q ");
        }
    }
    printf("\n");
}
int main()
{
    char input[100];
    int player = 1;
    printf("\t\t\tWELCOME TO OUR CHESS GAME \n");
    while (true)
    {
        printf("Enter new for new game or load to load previous game: ");
        fgets(input, 100, stdin);
        if (strlen(input) <= 5)
        {
            if (strcmp(input, "new\n") == 0)
            {
                save(player);
                namesaved = true;
                break;
            }
            else if (strcmp(input, "load\n") == 0)
            {
                player = load();
                save(player);
                namesaved = true;
                break;
            }
        }
    }
    char From[100], To[100];
    bool close = false;
    while (!checkmate(player) && !stalemate(player) && !close)
    {
        testStalemate = false;
        for (int i = 0; i < 80; i++)
        {
            printf("*");
        }
        bool noplayerchange = false;
        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                copyBoard[i][j] = chess[i][j];
            }
        }
        copy_kingMoved[0] = kingMoved[0];
        copy_kingMoved[1] = kingMoved[1];
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                copy_rookMoved[i][j] = rookMoved[i][j];
            }
        }
        if (player == 1)
        {
            printf("\t\t\tIt is white player turn\n");
        }
        else
        {
            printf("\t\t\tIt is black player turn\n");
        }
        if(check(player))
        {
            printf("\t\t\t!!!!!!!! Check !!!!!!!!\n");
        }
        print_eaten_pieces();
        enpassant_turn++;
        printf(" \n");
        printf("\t\t\t      A B C D E F G H \n");
        printf("\t\t\t     _________________   \n");
        for (int i = 0; i < 8; i++)
        {
            printf("\t\t\t%d   ", i + 1);
            for (int j = 0; j < 8; j++)
            {
                if (j == 0)
                {
                    printf("| ");
                }
                if (chess[i][j] == '-')
                {
                    printf(". ");
                }
                else if(chess[i][j] == '.')
                {
                    printf("- ");
                }
                else
                {
                    printf("%c ", chess[i][j]);
                }
            }
            printf("|  %d\n", i + 1);
        }
        printf("\t\t\t     -----------------   \n");
        printf("\t\t\t      A B C D E F G H \n");
        bool isundoredo = false;
        bool stepFinished = false;
        while (true)
        {
            printf("Enter the index of the piece you want to move: ");
            fgets(From, 100, stdin);
            if (strcmp(From, "undo\n") == 0)
            {
                noplayerchange = !undo();
                break;
            }
            if (strcmp(From, "redo\n") == 0)
            {
                noplayerchange = !redo();
                break;
            }
            if (strcmp(From, "close\n") == 0)
            {
                close = true;
                break;
            }
            if (strlen(From) != 3)
            {
                printf("Enter the index in the correct form\n");
            }
            else
            {
                if (validSt(From))
                {
                    if (validFrom(From, player))
                    {
                        while (true)
                        {
                            printf("Enter the index of the square you want to move you piece to: ");
                            fgets(To, 100, stdin);
                            if (strcmp(To, "undo\n") == 0)
                            {
                                noplayerchange = !undo();
                                isundoredo = true;
                                break;
                            }
                            if (strcmp(To, "redo\n") == 0)
                            {
                                noplayerchange = !redo();
                                isundoredo = true;
                                break;
                            }
                            if (strcmp(To, "close\n") == 0)
                            {
                                close = true;
                                break;
                            }
                            if (strlen(To) != 3)
                            {
                                printf("Enter the index in the correct form\n");
                            }
                            else
                            {
                                if (validSt(To))
                                {
                                    if (validTo(From, To, player, 0))
                                    {
                                        add_UndoNode();
                                        move(From, To, player);
                                        promotion();
                                        removeRedo();
                                        stepFinished = true;
                                        break;
                                    }
                                    else
                                    {
                                        printf("Enter valid move to your piece\n");
                                        break;
                                    }
                                }
                                else
                                {
                                    printf("Enter the index in the correct form\n");
                                    break;
                                }
                            }
                        }
                        if (isundoredo)
                        {
                            isundoredo = false;
                            break;
                        }
                        if (close || stepFinished)
                        {
                            break;
                        }
                    }
                    else
                    {
                        printf("Enter the index of one of your pieces\n");
                    }
                }
                else
                {
                    printf("Enter the index in the correct form\n");
                }
            }
        }
        if (!noplayerchange)
        {
            player = (player == 1) ? 2 : 1;
        }
        save(player);
        if (close)
        {
            break;
        }
    }
        removeUndo();
        if (close)
        {
            for (int i = 0; i < 80; i++)
            {
                printf("*");
            }
            printf("The game is saved continue it in any time\n");
            return 0;
        }
        for (int i = 0; i < 80; i++)
        {
            printf("*");
        }
        printf("\t\t\t      A B C D E F G H \n");
        printf("\t\t\t     _________________   \n");
        for (int i = 0; i < 8; i++)
        {
            printf("\t\t\t%d   ", i + 1);
            for (int j = 0; j < 8; j++)
            {
                if (j == 0)
                {
                    printf("| ");
                }
                if (chess[i][j] == '-')
                {
                    printf(". ");
                }
                else if(chess[i][j] == '.')
                {
                    printf("- ");
                }
                else
                {
                    printf("%c ", chess[i][j]);
                }
            }
            printf("|  %d\n", i + 1);
        }
        printf("\t\t\t     -----------------   \n");
        printf("\t\t\t      A B C D E F G H \n");
        if (stalemate(player))
        {
            printf("The game end in draw by stalemate\n");
            return 0;
        }
        if (checkmate(player))
        {
            if (player == 1)
            {
                printf("Black player wins by checkmate\n");
            }
            else
            {
                printf("White player wins by checkmate\n");
            }
        }
        return 0;
}
