#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

//页表项
struct Table {
    int pageNumber;    //页号
    int blockNumber;   //物理块号
    int state;         //状态位
    int accessFields;  //访问字段
    int isModify;      //修改位
    int storage;       //外存地址
};

//页表
class PageTable {
public:
    string owner;      //占有者
    int size;
    int tableNum;      //页表项数
    vector<struct Table> tables;

    PageTable() : tableNum(0) {}

    void add_table(int num) {
        for (int i = 0; i < num; i++) {
            struct Table table;
            table.pageNumber = i + tableNum;
            table.blockNumber = -1;
            table.state = 0;
            table.accessFields = -1;
            table.isModify = -1;
            table.storage = -1;
            tables.push_back(table);
        }
        tableNum += num;
    }

    void print() {
        cout << "页表" << endl;
        for (int i = 0; i < tableNum; i++) {
            printf("%-11s%-11s%-11s%-11s%-11s%-11s\n", "页号", "物理块号", "状态位", "访问字段", "修改位", "外存地址");
            printf("%-11d%-11d%-11d%-11d%-11d%-11d\n", tables[i].pageNumber, tables[i].blockNumber, tables[i].state,
                   tables[i].accessFields, tables[i].isModify, tables[i].storage);
        }
        cout << endl;
    }
};

//内存块分配项表
struct BlockTable {
    int blockNumber;   //内存块号
    int start;         //起始地址
    int end;           //结束地址
    int size;          //块长
    bool state;        //状态（是否被占用）1为被占用，0为未被占用
    string owner;      //占有者
    string content;    //内容
};

//内存
class RAM {
public:
    const static int size = 256;
    const static int blockNum = 64;
    const static int blockSize = 40;
    struct BlockTable blockTable[64];    //长度为64的内存块分配项表
    vector<class PageTable> pageTables;

    RAM();

    void allocation(string processName, int size, string content, int run[], int runSize);

    void recovery(string processName);

    int find(string processName, string str);

    void print_process();

    void print_ram();
};

//构造函数，用于初始化
RAM::RAM(void) {
    for (int i = 0; i < 64; i++) {
        blockTable[i].blockNumber = i;
        blockTable[i].start = i * blockSize;
        blockTable[i].end = (i + 1) * blockSize - 1;
        blockTable[i].size = blockSize;
        blockTable[i].state = 0;
        blockTable[i].owner = "";
    }
}

//用于为进程分配空间
void RAM::allocation(string processName, int size, string content, int run[], int runSize) {
    cout << "占有进程名：" << processName << endl;
    //TODO Byte
    cout << "占用空间：" << size << "KB" << endl;
    //TODO 40
    int tableNum = (size - 1) / 40+ 1, freeBlock, num, max;
    class PageTable pageTable;
    pageTable.owner = processName;
    pageTable.size = size;
    pageTable.add_table(tableNum);
    pageTables.push_back(pageTable);
    num = pageTables.size() - 1;

    for (int i = 0; i < runSize; i++) {
        //TODO 40
                //TODO 40
         string str;
if (run[i] * 40< size) {
    int substrLength = min(40, size - run[i] * 40); // 确保子字符串长度不超过剩余长度
    str = content.substr(run[i] * 40, substrLength);
    cout << "提取的子字符串：" << str << endl;
} 
        bool full = true;
        max = -1;
        cout << "进程访问页面" << run[i] << "，";
        if (!pageTables[num].tables[run[i]].state) {
            cout << "未命中，";
            freeBlock = find(processName, str);
            if (freeBlock != -1)
                full = false;
            if (full) {
                int max_j, max_k;
                cout << "内存块已满，进行全局置换LRU，";
                for (int j = 0; j < pageTables.size(); j++) {
                    for (int k = 0; k < pageTables[j].tableNum; k++) {
                        if (pageTables[j].tables[k].accessFields > max) {
                            max = pageTables[j].tables[k].accessFields;
                            max_j = j;
                            max_k = k;
                        }
                    }
                }
                cout << "置换第" << pageTables[max_j].tables[max_k].blockNumber << "内存块" << endl;
                freeBlock = pageTables[max_j].tables[max_k].blockNumber;
                blockTable[freeBlock].state = 1;
                blockTable[freeBlock].owner = processName;
                blockTable[freeBlock].content = str;
                pageTables[max_j].tables[max_k].blockNumber = -1;
                pageTables[max_j].tables[max_k].state = 0;
                pageTables[max_j].tables[max_k].accessFields = -1;
            }
            pageTables[num].tables[run[i]].state = 1;
            pageTables[num].tables[run[i]].blockNumber = freeBlock;
            pageTables[num].tables[run[i]].accessFields = 0;
            pageTables[num].tables[run[i]].isModify = 0;
            pageTables[num].tables[run[i]].storage = 0;
        } else {
            cout << "命中" << endl;
            pageTables[num].tables[run[i]].accessFields = -1;
        }
        for (int j = 0; j < pageTables.size(); j++) {
            for (int k = 0; k < pageTables[j].tableNum; k++) {
                if (pageTables[j].tables[k].state == 1)
                    pageTables[j].tables[k].accessFields++;
            }
        }
    }
    pageTables[num].print();
    print_ram();
}

//用于回收内存
void RAM::recovery(string processName) {
    vector<class PageTable>::iterator it;
    for (it = pageTables.begin(); it != pageTables.end(); it++) {
        if ((*it).owner == processName) {
            for (int i = 0; i < (*it).tables.size(); i++) {
                blockTable[(*it).tables[i].blockNumber].state = 0;
                blockTable[(*it).tables[i].blockNumber].owner = "";
                blockTable[(*it).tables[i].blockNumber].content = "";
            }
            pageTables.erase(it);
            cout << "已回收进程" << processName << endl;
            return;
        }
    }
    cout << "该进程不存在" << endl;
}

//用于打印内存中进程的有关信息
void RAM::print_process() {
    vector<class PageTable>::iterator it;
    for (it = pageTables.begin(); it != pageTables.end(); it++) {
        (*it).print();
    }
}

//用于寻找空闲内存块
int RAM::find(string processName, string str) {
    for (int i = 0; i < blockNum; i++) {
        if (!blockTable[i].state) {
            blockTable[i].state = 1;
            blockTable[i].owner = processName;
            blockTable[i].content = str;
            cout << "分配在" << i << "号内存块" << endl;
            return i;
        }
    }
    return -1;
}


//用于打印内存块有关信息
void RAM::print_ram() {
    cout << "内存状态" << endl;
    for (int i = 0; i < blockNum; i++) {
        printf("%-11s%-11s%-11s%-11s%-11s%-11s%-11s\n", "内存块号", "起始地址", "结束地址", "块长", "状态", "占有者",
               "内容");
        printf("%-11d%-11d%-11d%-11d%-11d%-11s%-11s\n", blockTable[i].blockNumber, blockTable[i].start,
               blockTable[i].end, blockTable[i].size, blockTable[i].state, blockTable[i].owner.c_str(),
               blockTable[i].content.c_str());
    }
}

