#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"

#define NUM_R_BLOCK 16
#define NUM_S_BLOCK 32
#define NUM_SUM_BLOCK (NUM_R_BLOCK + NUM_S_BLOCK)
struct writeBlk
{
    unsigned char *blk;
    int size;
} ;
int insertIntoBlk(struct writeBlk *Blk, int X, int Y)
{
    if (Blk->size == 7)
    {
        printf("blk full!\n");
        return -1;
    }
    char strX[5] = {0};
    char strY[5] = {0};
    for (int i = 0; i < 4; i++)
    {
        int lsbX = X % 10;
        int lsbY = Y % 10;
        strX[3 - i] = lsbX;
        strY[3 - i] = lsbY;
        X = X / 10;
        Y = Y / 10;
    }
    for (int i = 0; i < 4; i++)
    {
        *(Blk->blk + Blk->size * 8 + i) = strX[i];
        *(Blk->blk + Blk->size * 8 + 4 + i) = strY[i];
    }
    Blk->size++;
    return 0;
}
int readFromBlk(unsigned char *blk,int index,int *X, int *Y)
{
    char str[5] = {0};
    if(index<7){
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + index * 8 + k);
        }
        *X = atoi(str);
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + index * 8 + 4 + k);
        }
        *Y = atoi(str);
        return 0;
    } else{
        printf("Index overflow\n");
        return -1;
    }
}
int getNextBlk(unsigned char *blk){
    char str[5] = {0};
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + 7 * 8 + 4 + k);
    }
    return atoi(str);
}
void printBlk(unsigned char* blk){
    int X = -1;
    int Y = -1;
    for(int j=0;j<7;j++){
        readFromBlk(blk,j,&X,&Y);
        printf("(X=%d, Y=%d) \n", X, Y);
    }
}
int linerSearch()
{
    Buffer buf; /* A buffer */

    int i = 0;

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* Get a new block in the buffer */
    /* Buffer is clean */
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.size = 0;

    unsigned char *blk;

    for (int i = 1; i <= NUM_R_BLOCK; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", i);
        int X = -1;
        int Y = -1;
        for(int j=0;j<7;j++){
            readFromBlk(blk,j,&X,&Y);
            if (X == 30)
            {
                if(insertIntoBlk(&Blk, X, Y) == -1){
                    printf("去你妈的垃圾实验\n");
                }
                /*
                int A =-1;int B = -1;
                readFromBlk(Blk.blk,0,&A,&B);
                printf("(%d,%d)",A,B);
                */
                /*for (int k = 0; k < Blk.size; k++)
                {
                    readFromBlk(Blk.blk,k,&X,&Y);
                    printf("(%d,%d)",X,Y);
                }*/
                //printBlk(Blk.blk);
                printf("(R=%d, S=%d) \n", X, Y);
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    
    writeBlockToDisk(Blk.blk,100,&buf);
    printf("numIO:%d\n", buf.numIO);
    blk = readBlockFromDisk(100,&buf);
    for(int k = 0;k<8;k++){
        printf("%d",*(blk+k));
    }
    printBlk(blk);
    freeBuffer(&buf);
}

int linerSearchB()
{
    Buffer buf; /* A buffer */

    int i = 0;

    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }

    /* Get a new block in the buffer */
    /* Buffer is clean */
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.size = 0;

    unsigned char *blk;

    for (int i = NUM_R_BLOCK+1; i <= NUM_SUM_BLOCK; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", i);
        int X = -1;
        int Y = -1;
        for(int j=0;j<7;j++){
            readFromBlk(blk,j,&X,&Y);
            if (X == 23)
            {
                if(insertIntoBlk(&Blk, X, Y) == -1){
                    printf("去你妈的垃圾实验\n");
                }
                printf("(C=%d, D=%d) \n", X, Y);
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    printf("numIO:%d\n", buf.numIO);
}
int main()
{
    linerSearch();
    //linerSearchB();
    return 0;
}