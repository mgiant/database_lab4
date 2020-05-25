#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"
#include <string.h>
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
void printBlk(unsigned char *blk);
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
        if (writeBlockToDisk(Blk->blk, Blk->blkNum, pbuf) != 0)
        {
            printf("Writing Block %d Failed!\n", Blk->blkNum);
            return -1;
        }
        printf("当前块已满,结果写入块%d\n", Blk->blkNum);
        //printBlk(Blk->blk);
        //freeBlockInBuffer(Blk->blk, pbuf);
        Blk->blkNum++;
        Blk->blk = getNewBlockInBuffer(pbuf);
        memset(Blk->blk,0,64);
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
int getXFromBlk(unsigned char *blk, int index)
{
    char str[5] = {0};
    if (index < 7)
    {
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + index * 8 + k);
        }
        return atoi(str);
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
        str[k] = *(blk + 7 * 8 + k);
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
void sort(struct tuple arr[],int len)
{
    for(int i=0;i<len;i++)
    {
        for(int j=i+1;j<len;j++)
        {
            if(arr[i].x > arr[j].x)
            {
                struct tuple tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}
int linarSearch()
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
    if (writeBlockToDisk(Blk.blk, Blk.blkNum, &buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
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

// 1/17,201/217,301/317,16/32
int TPMMS(int originBlkStart, int unmergedBlkStart, int resultBlkStart, int numOfBlk, int numOfGroup)
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
    Blk.blkNum = unmergedBlkStart;
    int groupSize = numOfBlk / numOfGroup;

    struct tuple arr[groupSize * 7];
    struct tuple tmp;
    unsigned char *blk;

    // 组内排序,并写入磁盘
    for (int outer = 0; outer < numOfGroup; outer++)
    {
        // 读入一个组并内部排序
        for (int inner = 0; inner < groupSize; inner++)
        {
            int blkNum = outer * groupSize + inner + originBlkStart;
            if ((blk = readBlockFromDisk(blkNum, &buf)) == NULL)
            {
                perror("Reading Block Failed!\n");
                return -1;
            }
            printf("读入数据块%d\n", blkNum);
            //printBlk(blk);
            for (int j = 0; j < 7; j++)
            {
                readFromBlk(blk, j, &(tmp.x), &(tmp.y));
                arr[inner * 7 + j].x = tmp.x;
                arr[inner * 7 + j].y = tmp.y;
            }
            freeBlockInBuffer(blk, &buf);
        }
        for (int i = 0; i < groupSize * 7; i++)
        {
            //printf("(%d,%d)\n",arr[i].x, arr[i].y);
        }
        sort(arr,groupSize*7);
        //qsort(arr, groupSize * 7, sizeof(struct tuple), comp);
        for (int i = 0; i < groupSize * 7; i++)
        {
            //printf("(%d,%d)\n",arr[i].x, arr[i].y);
            insertIntoBlk(&Blk, arr[i].x, arr[i].y, &buf);
        }
    }
    writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    printf("写入块%d\n", Blk.blkNum);

    // 组间归并
    for (int i = unmergedBlkStart; i < unmergedBlkStart + numOfBlk; i++)
    {
        blk = readBlockFromDisk(i, &buf);
        //printf("块%d", i);
        //printBlk(blk);
        freeBlockInBuffer(blk, &buf);
    }

    int innerIndex[numOfGroup];
    int blkIndex[numOfGroup];
    unsigned char *mergeBlks[numOfGroup];
    for (int i = 0; i < numOfGroup; i++)
    {
        innerIndex[i] = 0;
        blkIndex[i] = 0;
    }
    for (int i = 0; i < numOfGroup; i++)
    {
        int blkNum = i * groupSize + unmergedBlkStart;
        mergeBlks[i] = readBlockFromDisk(blkNum, &buf);
        printf("读入数据块%d\n", blkNum);
        for (int j = 0; j < 7; j++)
        {
            readFromBlk(mergeBlks[i], j, &(tmp.x), &(tmp.y));
            arr[i * 7 + j] = tmp;
        }
    }
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.size = 0;
    Blk.blkNum = resultBlkStart;
    for (int i = 0; i < numOfBlk * 7; i++)
    {
        tmp.x = 100; // max
        int minIndex = 0;
        for (int j = 0; j < numOfGroup; j++)
        {
            if (blkIndex[j] < groupSize && tmp.x > arr[innerIndex[j] + j * 7].x)
            {
                tmp = arr[innerIndex[j] + j * 7];
                minIndex = j;
            }
        }
        //printf("写入(%d,%d)size:%d\n", tmp.x, tmp.y, Blk.size);
        insertIntoBlk(&Blk, tmp.x, tmp.y, &buf);

        //正常情况,递增index
        innerIndex[minIndex]++;

        //当前块读到了末尾
        if (innerIndex[minIndex] == 7)
        {
            if (blkIndex[minIndex] < groupSize - 1)
            {
                unsigned char *blk = NULL;
                blkIndex[minIndex]++;
                innerIndex[minIndex] = 0;
                int blkNum = blkIndex[minIndex] + minIndex * groupSize + unmergedBlkStart;
                printf("buf size:%d\n",buf.numFreeBlk);
                freeBlockInBuffer(mergeBlks[minIndex],&buf);
                printf("读入数据块%d\n", blkNum);
                mergeBlks[minIndex] = readBlockFromDisk(blkNum, &buf);
                
                printf("buf size:%d\n",buf.numFreeBlk);
                for (int j = 0; j < 7; j++)
                {
                    readFromBlk(mergeBlks[minIndex], j, &(tmp.x), &(tmp.y));
                    arr[minIndex * 7 + j].x = tmp.x;
                    arr[minIndex * 7 + j].y = tmp.y;
                }
                //freeBlockInBuffer(blk, &buf);
            }
            else
            {
                //寿终正寝
                blkIndex[minIndex] = groupSize;
            }
        }
    }
    writeBlockToDisk(Blk.blk, resultBlkStart + numOfBlk - 1, &buf);
    printf("写入块%d", resultBlkStart + numOfBlk - 1);
    for (int i = resultBlkStart; i < resultBlkStart + numOfBlk; i++)
    {
        blk = readBlockFromDisk(i, &buf);
        printf("块%d", i);
        printBlk(blk);
        freeBlockInBuffer(blk, &buf);
    }
    return 0;
}

int createIndex(unsigned int blkStart, unsigned int numOfBlk, Buffer *pbuf)
{
    unsigned char *blk = NULL;
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(pbuf);
    Blk.size = 0;
    Blk.blkNum = 350;

    struct tuple tmp;
    for (int blkNum = blkStart; blkNum < blkStart + numOfBlk; blkNum++)
    {
        if ((blk = readBlockFromDisk(blkNum, pbuf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        readFromBlk(blk, 0, &(tmp.x), &(tmp.y));
        tmp.y = blkNum;
        insertIntoBlk(&Blk, tmp.x, tmp.y, pbuf);
        freeBlockInBuffer(blk, pbuf);
    }
    writeBlockToDisk(Blk.blk, Blk.blkNum, pbuf);
    printf("写入块%d", Blk.blkNum);
    return 350;
}

int searchInBlk(unsigned int blkNum, struct writeBlk *Blk, Buffer *pbuf)
{
    printf("Search in blk %d\n", blkNum);
    int find = 0;
    unsigned char *blk = NULL;
    if ((blk = readBlockFromDisk(blkNum, pbuf)) == NULL)
    {
        printf("Read block error\n");
        return -1;
    }
    struct tuple tmp;
    for (int j = 0; j < 7; j++)
    {
        readFromBlk(blk, j, &(tmp.x), &(tmp.y));
        if (tmp.x == 30)
        {
            find++;
            insertIntoBlk(Blk, tmp.x, tmp.y, pbuf);
            printf("(A=%d, B=%d) \n", tmp.x, tmp.y);
        }
    }
    freeBlockInBuffer(blk, pbuf);
    return find;
}
int indexSearch()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.size = 0;
    Blk.blkNum = 120;

    //int indexFileAddr = createIndex(301, 16, &buf);
    unsigned int indexFileAddr = 350;
    unsigned char *blk = NULL;
    if ((blk = readBlockFromDisk(indexFileAddr, &buf)) == NULL)
    {
        printf("%x", blk);
        perror("Reading Block Failed!\n");
        return -1;
    }
    struct tuple Left;
    struct tuple Right;
    int find = 0;
    for (int i = 0; i < 7; i++)
    {
        readFromBlk(blk, i, &(Left.x), &(Left.y));
        if (i == 6)
        {
            indexFileAddr = getNextBlk(blk);
            //printf("get next blk %d",indexFileAddr);
            freeBlockInBuffer(blk, &buf);
            blk = readBlockFromDisk(indexFileAddr, &buf);
            i = -1;
        }
        readFromBlk(blk, i + 1, &(Right.x), &(Right.y));
        if (Left.x <= 30 && Right.x == 30)
        {
            find += searchInBlk(Left.y, &Blk, &buf);
        }
        if (Left.x <= 30 && 30 < Right.x)
        {
            find += searchInBlk(Left.y, &Blk, &buf);
            break;
        }
    }
    if (find > 0)
    {
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
        printf("结果写入块%d\n", Blk.blkNum);
        printf("\n共找到%d个元组\n", find);
        printf("IO次数%d\n", buf.numIO);
    }
    return find;
}
int projection()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.blkNum = 180;
    Blk.size = 0;
    unsigned int blkStartAddr = 301;
    unsigned char *blk = NULL;
    int LastX = -1;
    int count = 0;
    for (unsigned int blkNum = blkStartAddr; blkNum < blkStartAddr + NUM_R_BLOCK; blkNum++)
    {
        //printf("\n读入块%d\n", blkNum);
        blk = readBlockFromDisk(blkNum, &buf);
        int curX = 0;
        int tmpY = 0;
        for (int j = 0; j < 7; j++)
        {
            curX = readFromBlk(blk, j, &curX, &tmpY);
            //printf("(a=%d,b=%d)", tmpX, tmpY);
            if (curX != LastX)
            {
                count++;
                printf("(A=%d)\n", curX);
                insertIntoBlk(&Blk, curX, -1, &buf);
                LastX = curX;
            }
        }
        freeBlockInBuffer(blk, &buf);
    }
    printf("结果写入块%d\n", Blk.blkNum);
    if (count > 0)
    {
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    }
    printf("满足条件的元组一共有%d个\n", count);
    return 0;
}
int getNextRight(unsigned char *blk, int blkStart, int left, int *x, Buffer *pbuf, int numOfBlk)
{
    int right = 0;
    if (left % 7 == 0)
    {
        freeBlockInBuffer(blk, pbuf);
        blk = readBlockFromDisk(blkStart + left / 7, pbuf);
    }
    *x = getXFromBlk(blk, left % 7);
    for (right = left + 1; right < numOfBlk * 7; right++)
    {
        int index = right % 7;
        int nblk = right / 7;
        //到了一块的末尾
        if (index == 0)
        {
            freeBlockInBuffer(blk, pbuf);
            blk = readBlockFromDisk(blkStart + nblk, pbuf);
        }
        if (getXFromBlk(blk, index) != *x)
        {
            break;
        }
    }
    return right;
}

int writeJoinToDisk(int rLeft, int rRight, int sLeft, int sRight, struct writeBlk *Blk, Buffer *pbuf)
{
    int rOffset = 301;
    int sOffset = 317;
    unsigned char *rBlk = readBlockFromDisk(rLeft / 7 + rOffset, pbuf);
    unsigned char *sBlk = readBlockFromDisk(sLeft / 7 + sOffset, pbuf);
    for (int i = rLeft; i < rRight; i++)
    {
        if (i % 7 == 0)
        {
            freeBlockInBuffer(rBlk, pbuf);
            rBlk = readBlockFromDisk(i / 7 + rOffset, pbuf);
        }
        for (int j = sLeft; j < sRight; j++)
        {
            if (j % 7 == 0)
            {
                freeBlockInBuffer(sBlk, pbuf);
                sBlk = readBlockFromDisk(j / 7 + sOffset, pbuf);
            }
            if (Blk->size == 3)
            {
                char nextBlk[5] = {0};
                itoa(Blk->blkNum + 1, nextBlk, 10);
                for (int i = 48; i < 56; i++)
                {
                    *(Blk->blk + i) = 0;
                }
                for (int i = 0; i < 4; i++)
                {
                    *(Blk->blk + 7 * 8 + i) = nextBlk[i];
                }
                printf("写入块%d\n", Blk->blkNum);
                writeBlockToDisk(Blk->blk, Blk->blkNum, pbuf);
                Blk->blk = getNewBlockInBuffer(pbuf);
                Blk->blkNum++;
                Blk->size = 0;
            }
            for (int k = 0; k < 8; k++)
            {
                //printf("%c",rBlk[(i%7)*8+k]);
                //printf("%c",sBlk[(j%7)*8+k]);
                (Blk->blk + Blk->size * 16)[k] = *(rBlk + (i % 7) * 8 + k);
                *(Blk->blk + Blk->size * 16 + 8 + k) = *(sBlk + (j % 7) * 8 + k);
            }
            Blk->size++;
        }
    }
    freeBlockInBuffer(rBlk, pbuf);
    freeBlockInBuffer(sBlk, pbuf);
}
int sortMergeJoin()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.blkNum = 401;
    Blk.size = 0;

    int rBlkStart = 301;
    int sBlkStart = 317;
    int rLeft = 0;
    int rRight = 0;
    int sLeft = 0;
    int sRight = 0;
    int count = 0;
    unsigned char *rBlk = readBlockFromDisk(rBlkStart, &buf);
    unsigned char *sBlk = readBlockFromDisk(sBlkStart, &buf);
    int rA = -1;
    int sC = -1;
    while (rLeft < NUM_R_BLOCK * 7 && sLeft < NUM_S_BLOCK * 7)
    {
        rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
        sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);

        while (rA != sC && rRight <= NUM_R_BLOCK * 7 && sRight <= NUM_S_BLOCK * 7)
        {
            if (rA < sC)
            {
                rLeft = rRight;
                rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
                continue;
            }
            else
            {
                sLeft = sRight;
                sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
                continue;
            }
        }
        if (rA == sC)
        {
            printf("%d,%d\n", rA, sC);
            count += (sRight - sLeft) * (rRight - rLeft);
            writeJoinToDisk(rLeft, rRight, sLeft, sRight, &Blk, &buf);
            sLeft = sRight;
            rLeft = rRight;
        }
    }
    if(count){
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    }
    printf("总共连接%d次\n", count);
    return 0;
}

int writeTuplesTODisk(int left, int right, int addrStart, struct writeBlk *Blk, Buffer *pbuf)
{
    unsigned char *blk = readBlockFromDisk(addrStart + left / 7, pbuf);
    struct tuple tmp;
    int count = 0;
    for (int i = left; i < right; i++)
    {
        if (i == left || i % 7 == 0)
        {
            freeBlockInBuffer(blk, pbuf);
            blk = readBlockFromDisk(addrStart + i / 7, pbuf);
        }
        readFromBlk(blk, i % 7, &(tmp.x), &(tmp.y));
        insertIntoBlk(Blk, tmp.x, tmp.y, pbuf);
    }
    freeBlockInBuffer(blk, pbuf);
    return right - left;
}

int writeUnionToDisk(int rLeft, int rRight, int sLeft, int sRight, struct writeBlk *Blk, Buffer *pbuf)
{
    int rOffset = 301;
    int sOffset = 317;
    unsigned char *rBlk = readBlockFromDisk(rLeft / 7 + rOffset, pbuf);
    unsigned char *sBlk = readBlockFromDisk(sLeft / 7 + sOffset, pbuf);
    int count = 0;
    int j;
    for (int i = rLeft; i < rRight; i++)
    {
        if (i % 7 == 0)
        {
            freeBlockInBuffer(rBlk, pbuf);
            rBlk = readBlockFromDisk(i / 7 + rOffset, pbuf);
        }
        int same = 0;
        struct tuple tmpR;
        struct tuple tmpS;
        readFromBlk(rBlk, i % 7, &(tmpR.x), &(tmpR.y));
        for (j = sLeft; j < sRight; j++)
        {
            if (j % 7 == 0 || j == sLeft)
            {
                freeBlockInBuffer(sBlk, pbuf);
                sBlk = readBlockFromDisk(j / 7 + sOffset, pbuf);
            }
            readFromBlk(sBlk, j % 7, &(tmpS.x), &(tmpS.y));
            //printf("(X=%d,Y=%d)(X=%d,Y=%d)\n",tmpR.x,tmpR.y,tmpS.x,tmpS.y);
            if (tmpR.y == tmpS.y)
            {
                printf("same:(%d,%d)\n", tmpR.x, tmpR.y);
                same = 1;
                break;
            }
        }
        if (same == 0)
        {
            count++;
            insertIntoBlk(Blk, tmpR.x, tmpR.y, pbuf);
        }
    }
    count += writeTuplesTODisk(sLeft, sRight, sOffset, Blk, pbuf);
    freeBlockInBuffer(rBlk, pbuf);
    freeBlockInBuffer(sBlk, pbuf);
    return count;
}
int sortBasedUnion()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.blkNum = 511;
    Blk.size = 0;

    int rBlkStart = 301;
    int sBlkStart = 317;
    int rLeft = 0;
    int rRight = 0;
    int sLeft = 0;
    int sRight = 0;
    int count = 0;
    unsigned char *rBlk = readBlockFromDisk(rBlkStart, &buf);
    unsigned char *sBlk = readBlockFromDisk(sBlkStart, &buf);
    int rA = -1;
    int sC = -1;
    while (rLeft < NUM_R_BLOCK * 7 && sLeft < NUM_S_BLOCK * 7)
    {
        if (rA < sC)
        {
            count += writeTuplesTODisk(rLeft, rRight, rBlkStart, &Blk, &buf);
            rLeft = rRight;
            rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
            continue;
        }
        if (rA > sC)
        {
            count += writeTuplesTODisk(sLeft, sRight, sBlkStart, &Blk, &buf);
            sLeft = sRight;
            sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
            continue;
        }
        if (rA == sC)
        {
            //printf("RL:%d,RR:%d,SL:%d,SR:%d\n", rLeft, rRight, sLeft, sRight);
            count += writeUnionToDisk(rLeft, rRight, sLeft, sRight, &Blk, &buf);
            sLeft = sRight;
            rLeft = rRight;
            rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
            sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
        }
    }
    if (rLeft < NUM_R_BLOCK * 7)
    {
        count += writeTuplesTODisk(rLeft, NUM_R_BLOCK * 7, rBlkStart, &Blk, &buf);
    }
    if (sLeft < NUM_S_BLOCK * 7)
    {
        count += writeTuplesTODisk(sLeft, NUM_S_BLOCK * 7, sBlkStart, &Blk, &buf);
    }
    if(count){
        printf("写入块%d\n",Blk.blkNum);
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    }
    
    printf("满足条件的元组共有%d个\n", count);
    return 0;
}

int writeIntersectionToDisk(int rLeft, int rRight, int sLeft, int sRight, struct writeBlk *Blk, Buffer *pbuf)
{
    int rOffset = 301;
    int sOffset = 317;
    unsigned char *rBlk = readBlockFromDisk(rLeft / 7 + rOffset, pbuf);
    unsigned char *sBlk = readBlockFromDisk(sLeft / 7 + sOffset, pbuf);
    int count = 0;
    int j;
    for (int i = rLeft; i < rRight; i++)
    {
        if (i % 7 == 0)
        {
            freeBlockInBuffer(rBlk, pbuf);
            rBlk = readBlockFromDisk(i / 7 + rOffset, pbuf);
        }

        for (j = sLeft; j < sRight; j++)
        {
            if (j % 7 == 0 || j == sLeft)
            {
                freeBlockInBuffer(sBlk, pbuf);
                sBlk = readBlockFromDisk(j / 7 + sOffset, pbuf);
            }
            struct tuple tmpR;
            struct tuple tmpS;
            readFromBlk(rBlk, i % 7, &(tmpR.x), &(tmpR.y));
            readFromBlk(sBlk, j % 7, &(tmpS.x), &(tmpS.y));
            //printf("(X=%d,Y=%d)(X=%d,Y=%d)\n",tmpR.x,tmpR.y,tmpS.x,tmpS.y);
            if (tmpR.y == tmpS.y)
            {
                count++;
                insertIntoBlk(Blk, tmpR.x, tmpR.y, pbuf);
                printf("(X=%d,Y=%d)\n", tmpR.x, tmpR.y);
            }
        }
    }
    freeBlockInBuffer(rBlk, pbuf);
    freeBlockInBuffer(sBlk, pbuf);
    return count;
}
int sortBasedIntersection()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.blkNum = 550;
    Blk.size = 0;

    int rBlkStart = 301;
    int sBlkStart = 317;
    int rLeft = 0;
    int rRight = 0;
    int sLeft = 0;
    int sRight = 0;
    int count = 0;
    unsigned char *rBlk = readBlockFromDisk(rBlkStart, &buf);
    unsigned char *sBlk = readBlockFromDisk(sBlkStart, &buf);
    int rA = -1;
    int sC = -1;
    while (rLeft < NUM_R_BLOCK * 7 && sLeft < NUM_S_BLOCK * 7)
    {
        rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
        sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);

        while (rA != sC && rRight <= NUM_R_BLOCK * 7 && sRight <= NUM_S_BLOCK * 7)
        {
            if (rA < sC)
            {
                rLeft = rRight;
                rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
            }
            else
            {
                sLeft = sRight;
                sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
            }
        }
        if (rA == sC)
        {
            count += writeIntersectionToDisk(rLeft, rRight, sLeft, sRight, &Blk, &buf);
            sLeft = sRight;
            rLeft = rRight;
        }
    }
    printf("结果写入块%d\n", Blk.blkNum);
    if (count)
    {
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
        printf("满足条件的元组共有%d个\n", count);
    }
    return 0;
}

int writeDifferenceToDisk(int rLeft, int rRight, int sLeft, int sRight, struct writeBlk *Blk, Buffer *pbuf)
{
    int rOffset = 301;
    int sOffset = 317;
    unsigned char *rBlk = readBlockFromDisk(rLeft / 7 + rOffset, pbuf);
    unsigned char *sBlk = readBlockFromDisk(sLeft / 7 + sOffset, pbuf);
    int count = 0;
    int j;
    for (int i = sLeft; i < sRight; i++)
    {
        if (i % 7 == 0)
        {
            freeBlockInBuffer(sBlk, pbuf);
            sBlk = readBlockFromDisk(i / 7 + sOffset, pbuf);
        }
        int same = 0;
        struct tuple tmpR;
        struct tuple tmpS;
        
        for (j = rLeft; j < rRight; j++)
        {
            if (j % 7 == 0 || j == rLeft)
            {
                freeBlockInBuffer(rBlk, pbuf);
                rBlk = readBlockFromDisk(j / 7 + rOffset, pbuf);
            }
            readFromBlk(sBlk, i % 7, &(tmpS.x), &(tmpS.y));
            readFromBlk(rBlk, j % 7, &(tmpR.x), &(tmpR.y));
            //printf("(X=%d,Y=%d)(X=%d,Y=%d)\n",tmpR.x,tmpR.y,tmpS.x,tmpS.y);
            if (tmpR.y == tmpS.y)
            {
                printf("same:(%d,%d)\n", tmpR.x, tmpR.y);
                same = 1;
                break;
            }
        }
        if (same == 0)
        {
            count++;
            insertIntoBlk(Blk, tmpS.x, tmpS.y, pbuf);
        }
    }
    freeBlockInBuffer(sBlk,pbuf);
    freeBlockInBuffer(rBlk,pbuf);
    return count;
}
int sortBasedDifference()
{
    Buffer buf;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    struct writeBlk Blk;
    Blk.blk = getNewBlockInBuffer(&buf);
    Blk.blkNum = 601;
    Blk.size = 0;

    int rBlkStart = 301;
    int sBlkStart = 317;
    int rLeft = 0;
    int rRight = 0;
    int sLeft = 0;
    int sRight = 0;
    int count = 0;
    unsigned char *rBlk = readBlockFromDisk(rBlkStart, &buf);
    unsigned char *sBlk = readBlockFromDisk(sBlkStart, &buf);
    int rA = -1;
    int sC = -1;
    while (rLeft < NUM_R_BLOCK * 7 && sLeft < NUM_S_BLOCK * 7)
    {
        if (rA < sC)
        {
            rLeft = rRight;
            rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
            continue;
        }
        if (rA > sC)
        {
            count += writeTuplesTODisk(sLeft, sRight, sBlkStart, &Blk, &buf);
            sLeft = sRight;
            sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
            continue;
        }
        if (rA == sC)
        {
            //printf("RL:%d,RR:%d,SL:%d,SR:%d\n", rLeft, rRight, sLeft, sRight);
            count += writeDifferenceToDisk(rLeft, rRight, sLeft, sRight, &Blk, &buf);
            sLeft = sRight;
            rLeft = rRight;
            rRight = getNextRight(rBlk, rBlkStart, rLeft, &rA, &buf, NUM_R_BLOCK);
            sRight = getNextRight(sBlk, sBlkStart, sLeft, &sC, &buf, NUM_S_BLOCK);
        }
    }
    if (sLeft < NUM_S_BLOCK * 7)
    {
        count += writeTuplesTODisk(sLeft, NUM_S_BLOCK * 7, sBlkStart, &Blk, &buf);
    }
    if(count){
        printf("结果写入块%d\n",Blk.blkNum);
        writeBlockToDisk(Blk.blk, Blk.blkNum, &buf);
    }
    
    printf("满足条件的元组共有%d个\n", count);
    return 0;
}
int main()
{
    //linarSearch();
    //linerSearchB();
    //TPMMS(1, 201, 301, 16,2);
    //TPMMS(17, 217, 317, 32,4);
    //indexSearch();
    //projection();
    //sortMergeJoin();
    //sortBasedIntersection();
    //sortBasedUnion();
    sortBasedDifference();
    return 0;
}