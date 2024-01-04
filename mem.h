#include <iostream>
#include <cstring>

using namespace std;

//将磁盘分块，一共有1024块，每一块是40byte
struct DiskBlock {
    int blockNum;
    string owner;
    int state;
    string content;
};

//File Allocation Table 文件分配表
struct FAT {
    int blockNum; // 该文件用的DiskBlock块数
    int nextBlock; // 下一块？？
};

//空闲的块区
typedef struct freeBlock {
    //数量？
    int blockNum;
    //空闲块的数量
    int freeNum;

    int data[100];
    //下一个空闲块区
    freeBlock *nextFree;
} freeBlock;

class Disk {
public:
    const static int size = 40960; //TODO 40kb
    const static int blockNum = 1024;
    const static int commonBlockNum = 900;
    const static int exchangeBlockNum = 124;
    const static int blockSize = 40/*byte*/;  //TODO 40
    struct DiskBlock commonBlock[900]; //文件区 900块 存用户文件
    struct DiskBlock exchangeBlock[124]; //交换区 124块 内存放不下的进程放在这 可以理解为没用
    struct FAT commonFat[900];
    struct FAT exchangeFat[124];
    freeBlock *free = new freeBlock;

    // 构造方法，先进这
    Disk();

    int Create_file(string name, int size, string content);

    string Get_content(int pointer);

    void print();
};

Disk::Disk(void) {
    for (int i = 0; i < commonBlockNum; i++) {
        //分配编号
        commonBlock[i].blockNum = i;
        //分配所属用户 咱们做的是单用户系统，感觉没用
        commonBlock[i].owner = "";
        //状态
        commonBlock[i].state = 0;
        //内容
        commonBlock[i].content = "";
        //分配表初始化每一块
        commonFat[i].blockNum = i;
        //没有下一块相连的
        commonFat[i].nextBlock = -1;
    }
    freeBlock *fb = new freeBlock;
    fb = free;
    fb->blockNum = -1;
    fb->freeNum = 100;
    for (int i = 0; i < 100; i++) {
        fb->data[i] = i;
    }
    for (int i = 1; i < 9; i++) {
        freeBlock *next = new freeBlock;
        next->blockNum = fb->data[0];
        next->freeNum = 100;
        for (int j = 0; j < 100; j++) {
            next->data[j] = i * 100 + j;
        }
        fb->nextFree = next;
        fb = fb->nextFree;
    }
}

int Disk::Create_file(string name, int size, string content) {
    int blockCount = (size - 1) / blockSize + 1;
    freeBlock *p = new freeBlock;
    p = free;
    for (int i = 0; i < blockCount; i++) {
        commonBlock[p->data[p->freeNum - i - 1]].state = 1;
        commonBlock[p->data[p->freeNum - i - 1]].owner = name;
        commonBlock[p->data[p->freeNum - i - 1]].content = content.substr(i * 40, 40); //TODO 40
        commonFat[p->data[p->freeNum - i - 1]].nextBlock = p->data[p->freeNum - i - 2];
    }
    commonFat[p->data[p->freeNum - blockCount]].nextBlock = -1;
    p->freeNum -= blockCount;
    return p->data[p->freeNum + blockCount - 1];
}

string Disk::Get_content(int pointer) {
    string content = "";
    while (1) {
        content += commonBlock[pointer].content;
        if (commonFat[pointer].nextBlock == -1) break;
        pointer = commonFat[pointer].nextBlock;
    }
    return content;
}

void Disk::print() {
    for (int i = 0; i < commonBlockNum; i++) {
        printf("%-11s%-11s%-11s%-11s\n", "blockNum", "owner", "state", "content");
        printf("%-11d%-11s%-11d%-11s\n", commonBlock[i].blockNum, commonBlock[i].owner.c_str(), commonBlock[i].state,
               commonBlock[i].content.c_str());
    }

}
