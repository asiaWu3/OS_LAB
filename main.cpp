#include <iostream>
#include <thread>
#include <mutex>
#include "file.h"
#include "mem.h"
#include "ram.h"

using namespace std;
mutex create_mu;
mutex run_mu;
mutex mu;
FS tempFS;
Disk tempDS;
RAM tempram;
FS::FCB *path1 = tempFS.ptr;


void f_create() {
    create_mu.lock();
    int size;              //byte
    string content;
    string name;
    cin>>name>>size>>content;
    int pointer;
    //磁盘块里塞东西
    pointer = tempDS.Create_file(name, size, content);

    if (pointer != 0) {
        //在目录系统创建文件(子节点)
        tempFS.createFile(name, pointer);
    } else {
        cout << "f_create fail" << endl;
    }

    create_mu.unlock();
}

void f_delete() {
    create_mu.lock();
    string name;
    cin >> name;
    int pointer;
    //目录中删除
    tempFS.deleteFile(name);
//    else Delete_file(pointer);
    create_mu.unlock();
}

void f_run() {
    run_mu.lock();
    string name;
    cout << "输入文件名:";
    cin >> name;
    FS::FCB *path = path1;
    int pointer;
    string content;
    int size;
    pointer = tempFS.getPointer(path, name);
    if (pointer != -1) {
        content = tempDS.Get_content(pointer);
        size = content.length();
        int a[10] = {1, 0, 2, 3, 1, 2, 0, 1, 2, 1};
        tempram.allocation(name, size, content, a, 10);
    } else {
        printf("not found\n");
    }
    run_mu.unlock();
}

int main() {
    string opt;
    while (1) {
        cout << "input operation:(ls/touch/cd/mkdir/rm/run/disk/exit)" << endl;
        cout<<">>> ";
        cin >> opt;
        if (opt == "ll") {
            //列出当前目录下所有文件、文件夹
            tempFS.ls();
        } else if (opt == "cd") {
            //切换目录 ..上一级
            string dir_name;
            cin >> dir_name;
            tempFS.cd(dir_name);
        }
        else if (opt == "pwd") {
            tempFS.pwd();
        } else if (opt == "touch") {
            //创建文件
            f_create();
            tempFS.ls();
            string cd_dir_name;
        } else if (opt == "mkdir") {
            //创建文件夹
            string dir_name;
            cin >> dir_name;
            tempFS.createDir(dir_name);
        } else if (opt == "rm") {
            //删除文件
            // TODO 磁盘删除
            f_delete();
        } else if(opt == "rmdir") {
            string dirName;
            cin>>dirName;
            tempFS.deleteDir(dirName);
        }
        else if (opt == "run") {
            //运行文件
            f_run();
        } else if (opt == "disk") {
            tempDS.print();
        }
            //TODO 显示内存占用情况
            //TODO 关闭进程、回收内存
        else break;
    }

}
