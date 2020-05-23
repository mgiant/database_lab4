#include <stdlib.h>
#include <stdio.h>

struct tuple
{
    int x;
    int y;
};
int comp(const void*a,const void*b)
{
return ((struct tuple*)a)->x-((struct tuple*)b)->x;
}
int main(){
    struct tuple arr[10];
    for(int i=0;i<10;i++){
        arr[i].x = 10-i;
        arr[i].y = 999;
    }
    qsort(arr,10,sizeof(struct tuple),comp);
    for(int i=0;i<10;i++){
        printf("(%d,%d),",arr[i].x,arr[i].y);
    }
}