#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"

#define NUM_R_BLOCK 16
#define NUM_S_BLOCK 32
#define NUM_SUM_BLOCK (NUM_R_BLOCK + NUM_S_BLOCK)
struct writeBlk
{
    unsigned char *blk;
    unsigned int size;
    unsigned int blkNum; //预计写回的块地址
};
struct tuple
{
    int x;
    int y;
};
int comp(const void *a, const void *b)
{
    return ((struct tuple *)a)->x - ((struct tuple *)b)->x;
}
int insertIntoBlk(struct writeBlk *Blk, int X, int Y, Buffer *pbuf)
{
    if (Blk->size == 7)
    {
        char nextBlk[5] = {0};
        itoa(Blk->blkNum + 1, nextBlk, 10);
        for (int i = 0; i < 4; i++)
        {
            *(Blk->blk + Blk->size * 8 + i) = nextBlk[i];
        }
        writeBlockToDisk(Blk->blk, Blk->blkNum, pbuf);
        printf("\n当前块已满,结果写入块%d\n", Blk->blkNum);
        freeBlockInBuffer(Blk->blk, pbuf);
        Blk->blkNum++;
        Blk->blk = getNewBlockInBuffer(pbuf);
        Blk->size = 0;
    }
    char strX[5] = {0};
    char strY[5] = {0};
    itoa(X, strX, 10);
    itoa(Y, strY, 10);
    for (int i = 0; i < 4; i++)
    {
        *(Blk->blk + Blk->size * 8 + i) = strX[i];
        *(Blk->blk + Blk->size * 8 + 4 + i) = strY[i];
    }
    Blk->size++;
    return 0;
}
int readFromBlk(unsigned char *blk, int index, int *X, int *Y)
{
    char str[5] = {0};
    if (index < 7)
    {
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
        return *X;
    }
    else
    {
        printf("Index overflow\n");
        return -1;
    }
}
int getNextBlk(unsigned char *blk)
{
    char str[5] = {0};
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + 7 * 8 + 4 + k);
    }
    return atoi(str);
}
void printBlk(unsigned char *blk)
{
    int X = -1;
    int Y = -1;
    for (int j = 0; j < 7; j++)
    {
        readFromBlk(blk, j, &X, &Y);
        printf("(X=%d, Y=%d) \n", X, Y);
    }
}

int linerSearch()
{
    printf("--------------------------------\n");
    printf("基于线性搜素的排序选择算法 R.A=30\n");
    printf("--------------------------------\n");
    Buffer buf; /* A buffer */
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
    Blk.blkNum = 100;
    int X = -1;
    int Y = -1;
    unsigned char *blk;
    for (int i = 1; i <= NUM_R_BLOCK; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", i);

        for (int j = 0; j < 7; j++)
        {
            readFromBlk(blk, j, &X, &Y);
            if (X == 30)
            {
                insertIntoBlk(&Blk, X, Y, &buf);
                printf("(A=%d, B=%d) \n", X, Y);
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    printf("\n结果写入块%d\n", Blk.blkNum);
    printf("numIO:%d\n", buf.numIO);

    printf("满足选择条件的元组一共%d个\n", Blk.size);
    freeBuffer(&buf);
}

int linerSearchB()
{
    printf("--------------------------------\n");
    printf("基于线性搜素的排序选择算法 S.C=23\n");
    printf("--------------------------------\n");
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

    for (int i = NUM_R_BLOCK + 1; i <= NUM_SUM_BLOCK; i++)
    {
        if ((blk = readBlockFromDisk(i, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", i);
        int X = -1;
        int Y = -1;
        for (int j = 0; j < 7; j++)
        {
            readFromBlk(blk, j, &X, &Y);
            if (X == 23)
            {
                insertIntoBlk(&Blk, X, Y, &buf);
                printf("(C=%d, D=%d) \n", X, Y);
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    writeBlockToDisk(Blk.blk, 99, &buf);
    printf("\n结果写入块%d\n", 99);
    printf("满足选择条件的元组一共%d个\n", Blk.size);
    printf("numIO:%d\n", buf.numIO);

    freeBuffer(&buf);
}

int TPMMS_R()
{
    Buffer buf;
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
    Blk.blkNum = 201;

    unsigned char *blk;
    //对R关于R.A进行块内排序,并写回 IO次数 = 32
    //
    struct tuple arrR[28];
    struct tuple tmp;
    //读入一个组并内部排序
    for (int outer = 0; outer < 4; outer++)
    {
        for (int inner = 1; inner <= NUM_R_BLOCK / 4; inner++)
        {
            int blkNum = outer * 4 + inner;
            if ((blk = readBlockFromDisk(blkNum, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
            printf("读入数据块%d\n", blkNum);
            for (int j = 0; j < 7; j++)
            {
                readFromBlk(blk, j, &(tmp.x), &(tmp.y));
                arrR[(inner - 1) * 7 + j].x = tmp.x;
                arrR[(inner - 1) * 7 + j].y = tmp.y;
            }
            freeBlockInBuffer(blk, &buf);
        }
        qsort(arrR, 28, sizeof(struct tuple), comp);
        for (int i = 0; i < 28; i++)
        {
            insertIntoBlk(&Blk, arrR[i].x, arrR[i].y, &buf);
        }
    }
    writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    printf("写入块%d\n", Blk.blkNum);
    for (int i = 201; i <= 216; i++)
    {
        blk = readBlockFromDisk(i, &buf);
        printf("块%d", i);
        printBlk(blk);
        freeBlockInBuffer(blk, &buf);
    }
    int innerIndex[4] = {0};
    int blkIndex[4] = {0};
    unsigned char *mergeBlks[4];
    for (int i = 0; i < 4; i++)
    {
        int blkNum = i * 4 + 201;
        if ((mergeBlks[i] = readBlockFromDisk(blkNum, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("读入数据块%d\n", blkNum);
        for (int j = 0; j < 7; j++)
        {
            readFromBlk(mergeBlks[i], j, &(tmp.x), &(tmp.y));
            arrR[i * 7 + j] = tmp;
        }
    }
    Blk.size = 0;
    Blk.blkNum = 301;
    for (int i = 0; i < 112; i++)
    {
        tmp.x = 100; // max
        int minIndex = 0;
        for (int j = 0; j < 4; j++)
        {
            if (blkIndex[j] < 4 && tmp.x > arrR[innerIndex[j]+j*7].x)
            {
                tmp = arrR[innerIndex[j]+j*7];
                minIndex = j;
            }
        }
        insertIntoBlk(&Blk, tmp.x, tmp.y,&buf);
        //正常情况,递增index
        if (innerIndex[minIndex] < 6)
        {
            innerIndex[minIndex]++;
        }
        //一路用完,并还有下一路.读取下一路
        else if (blkIndex[minIndex] < 3)
        {
            blkIndex[minIndex] ++;
            innerIndex[minIndex] = 0;
            int blkNum = blkIndex[minIndex] + minIndex * 4 + 201;
            freeBlockInBuffer(mergeBlks[minIndex],&buf);
            if ((mergeBlks[minIndex] = readBlockFromDisk(blkNum, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
            printf("读入数据块%d\n", blkNum);
            for (int j = 0; j < 7; j++)
            {
                readFromBlk(mergeBlks[minIndex], j, &(tmp.x), &(tmp.y));
                arrR[minIndex * 7 + j] = tmp;
            }
            //blkIndex[index] + index*4 + 301
        }
        else
        {
            //寿终正寝
            blkIndex[minIndex] = 4;
        }
    }
    writeBlockToDisk(Blk.blk,316,&buf);
    for (int i = 301; i <= 316; i++)
    {
        blk = readBlockFromDisk(i, &buf);
        printf("块%d", i);
        printBlk(blk);
        freeBlockInBuffer(blk, &buf);
    }
    return 0;
}
int TPMMS_S()

int main()
{
    //linerSearch();
    //linerSearchB();
    TPMMS();
    return 0;
}