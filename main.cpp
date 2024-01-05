#include <iostream>
#include <thread>
#include <mutex>
#include "file.h"
#include "mem.h"
#include "ram.h"

using namespace std;
mutex create_mu;
mutex run_mu;
FS tempFS;
Disk tempDS;
RAM tempRAM;


void f_create() {
    create_mu.lock();
    int size;              //byte
    string content;
    string name;
    cin >> name >> size >> content;
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
    //目录中删除
    int pointer = tempFS.deleteFile(name);
    if (pointer != -1) {
        //磁盘中删除
//        Delete_file(pointer);
    }
    create_mu.unlock();
}

void f_run() {
    run_mu.lock();
    string name;
    cin >> name;
    int pointer;
    string content;
    int size;
    pointer = tempFS.getPointer(name);
    if (pointer != -1) {
        //判断是否已打开
        if (tempFS.openFile(name)) {
            content = tempDS.Get_content(pointer);
            size = content.length();
            int a[14] = {1, 0, 2, 3, 1, 2, 4, 1, 2, 5, 3, 7, 2, 6};
            tempRAM.allocation(name, size, content, a, 14);
        }
    }
    run_mu.unlock();
}

int main() {
    string opt;
    while (1) {
        cout << "input operation:(ll/cd/pwd/touch/mkdir/rm/rmdir/run/showdisk/showram/exit)" << endl;
        cout << ">>> ";
        cin >> opt;
        if (opt == "ll") {
            //列出当前目录下所有文件、文件夹
            tempFS.ls();
        } else if (opt == "cd") {
            //切换目录 ..上一级
            string dir_name;
            cin >> dir_name;
            tempFS.cd(dir_name);
        } else if (opt == "pwd") {
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
        } else if (opt == "rmdir") {
            string dirName;
            cin >> dirName;
            tempFS.deleteDir(dirName);
        } else if (opt == "run") {
            //运行文件
            f_run();
            //显示磁盘占用情况
        } else if (opt == "showdisk") {
            tempDS.print();
            //显示内存占用情况
        } else if (opt == "showram") {
            tempRAM.print_ram();
            //
        } else if (opt == "recovery") {
            string processName;
            cin >> processName;
            tempRAM.recovery(processName);
            tempFS.closeFile(processName);
        } else if (opt == "exit") {
            cout << "exit..." << endl;
            break;
        } else {
            cout << "命令不合法" << endl;
        }
    }
}
