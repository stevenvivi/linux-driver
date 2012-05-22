#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct _memory_block
{
	struct _memory_block *pNext;
	struct _memory_block *pPrev;
	char *data;
}MemoryBlock

typedef struct _memory_pool
{
	unsigned int blockCount;
	unsigned int blockCountStep;
	unsigned int blockSize;
	unsigned int freeCount;
	MemoryBlcok *freeHead;
	MemoryBlock *freeTail;
	MemoryBlock *usedHead;
	MemoryBlock *usedTail;
	char *pBlockMemoryHead;
	char *pDataMemoryHead;
}Memorypool;

int Xpool_ini(unsigned int blockCount, unsigned int blockSize);
int Xpool_destory(void);
void *Xpool_alloc(unsigned int size);
int Xpool_free(void *ptr);

static int Xpool_block(unsigned int blockCount, unsigend int blockSize);
static MemoryPool memory;

int Xpool_init(unsigned int blockCount, unsigned int blockSize)
{
	Memorypool *p = &memory;
	p->blockCount = blcokCount;
	p->blockSize = blockSize;
	p->freeCount = blockCount;
	p->blockCountStep = 100;
	p->freeHead = p->freeTail = NULL;
	p->usedHead = p->usedTail = NULL;
	
	
	Xpool_block(blockCount, blockSize);  /*申请块并把新申请的块连接到空闲块后面*/
	return 0;
}

static int Xpool_block(unsigned int blockCount, unsigned int blockSize)
{
	Mempool *p = &memory;
	
	MemoryBlock *pPree = NULL;
	
	p->pBlockMemoryHead = (char *)malloc(sizeof(MemoryBlock) *blockCount);
	
	p->pDataMemoryHead = (char *)malloc((blockSize + sizeof(MemoryBlock *)) * blockCount);//分配一大块连续的内存空间存放供用户使用的空间
	
	
	for (unsigned int i = 0; i<blockCount; i++)
	{
		pFree = (MemoryBlock *)(p->pBlockMemoryHead + (sizeof(MemoryBlock * i));
		pFree->data = p->pDataMemoryHead + ((blockSize + sizeof(MemoryBlock *))*i);
		pFree->pNext = NULL;
		pFree->pPrev = p->freeTail;
		
		if(p->freeHead == NULL)
		{
			p->freeHead = p->freeTail = pFree;
		}else{
			p->freeTail->pNext = pFree;
			p->freeTail = pFree;
		}
	}
		return 0;
}

int Xpool_destroy(void)
{
MemoryPool *p = &memory;
 
//释放内存块所占的内存
free(p->pBlockMemoryHead);
 
//释放数据区域所占的内存
free(p->pDataMemoryHead);
 
return 0;
}
 
//申请内存
void* Xpool_alloc(unsigned int size)
{
MemoryPool *p = &memory;
MemoryBlock *block = NULL;
 
if(p->freeHead == NULL){//没有可用空间
Xpool_block(p->blockCountStep, p->blockSize);
}
 
block = p->freeHead;//获取表头内存块
p->freeHead = block->pNext;//将空闲块的链表头
p->freeCount--;//空闲块数量减一
block->pNext = NULL;
block->pPrev = p->usedTail;//这个块的上个块是已使用块的最后一个块
 
//第一次使用内存？
if(p->usedHead == NULL){
p->usedHead = p->usedTail = block;//，则已使用的头和尾都指向这个块
}else{//不是第一次
p->usedTail->pNext = block;
p->usedTail = block;
}
//留下data里一个指针的空间，用于保存与数据关联的块地址
 
block->data = (char *)block;
return (char *)block->data + sizeof(MemoryBlock *);
}
 
//回收内存
int Xpool_free(void *ptr)
{
MemoryPool *p = &memory;
char *realptr = (char *)ptr - sizeof(MemoryBlock *); //数据块真实的起始地址
MemoryBlock *block = (MemoryBlock *)realptr;
 
if(block == NULL){
return NULL;
}
 
if(block->pPrev == NULL){//如果是头
p->usedHead = block->pNext;
if(p->usedHead != NULL){
p->usedHead->pPrev = NULL;
}
}else if(block->pNext == NULL){//如果是尾
p->usedTail = block->pPrev;
if(p->usedTail != NULL){
p->usedTail->pNext = NULL;
}
}else{//中间的
block->pPrev->pNext = block->pNext;
block->pNext->pPrev = block->pPrev;
}
 
//重置参数
block->pPrev = p->freeTail;
block->pNext = NULL;
block->data = realptr;
 
//加到空闲块链表
p->freeTail->pNext = block;
p->freeTail = block;
p->freeCount++;
 
return 0;
}
 
int main(int argc, char *argv[])
{
Xpool_init(10, 96);
char *p = (char *)Xpool_alloc(20);
Xpool_free(p);
Xpool_destroy();
return 0;
}