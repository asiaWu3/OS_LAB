#include <iostream>
#include <string>
#include <sstream>
#include <deque>
#include <ctime>
#include <cstring>

using namespace std;


class FS {
public:
    FS() {
        root = new FCB("root", "DIR", getTimeStr(), true, nullptr, nullptr, nullptr);
        ptr = root;
    }

    ~FS() {}

    struct FCB {

        string _name;
        string _type;
        string _content;
        bool open;
        int _pointer; // 磁盘地址

        string _createtime;

        FCB *_next, *_child, *_parent;


        bool _free = true;// 判断文件是否被打开

        FCB(string name, string type, string createtime, bool free, FCB *next, FCB *child, FCB *parent,
            string content = "") {
            _name = name;
            _type = type;
            _createtime = createtime;
            _free = free;
            _next = next;
            _child = child;
            _parent = parent;
            _content = content;


        }
    } *ptr, *root;//ptr为当前地址，root为根目录地址

    string getTimeStr() {
        time_t curTime;
        time(&curTime);
        return ctime(&curTime);
    }

    //打开文件 设置标志位
    void openFile(FCB *path, string fileName) {
        FCB *p = path;
        while (p != nullptr) {
            if (p->_name == fileName && p->_type == "FILE" && p->_free != true) {
                p->_free = false;

            } else if (p->_name == fileName && p->_type == "FILE" && p->_free != false) {
                cout << "文件已在运行，无法再次运行" << endl;
            }
        }

    }

    void cd(string name) {
        if (name == "..") {
            if (ptr == root) {
                cout << "当前已是根目录" << endl;
                return;
            }
            ptr = ptr->_parent;
            return;
        }
        FCB *p = ptr->_child;
        while (p != nullptr) {
            if (p->_name == name) {
                ptr = p;
                return;
            }
            p = p->_next;
        }
        cout << "目录不存在" << endl;
        return;
    }

    bool deleteFile(string filename) {  //删除文件
        FCB *cur = ptr;
        FCB *child = cur->_child;
        cur = cur->_child;
        bool isFirst = true;
        while (cur != nullptr) {
            if (cur->_name == filename && cur->_child == nullptr && cur->_type == "FILE" && cur->_free == true) {
                if (isFirst)
                    ptr->_child = cur->_next;
                else {
                    child->_next = cur->_next;
                }
                delete cur;
                return true;
            } else if (cur->_name == filename && cur->_child == nullptr && cur->_type == "FILE" &&
                       cur->_free == false) {
                cout << "文件进程正在运行，无法删除";
                return false;
            }
            if (!isFirst) {
                child = child->_next;
            }

            isFirst = false;
            cur = cur->_next;
        }
        cout << "文件不存在" << endl;
    }


    bool createDir(string dirName)   //创建文件夹
    {
        FCB *cur = ptr;
        if (cur->_child == nullptr) {
            cur->_child = new FCB(dirName, "DIR", getTimeStr(), true, nullptr, nullptr, cur);
            return true;
        }

        FCB *child = cur->_child;
        cur = cur->_child;

        if (child->_name == dirName) {
            cout << "文件名已存在" << endl;
            return false;
        }

        while (child->_next != nullptr) {
            child = child->_next;
            if (child->_name == dirName) {
                cout << "文件名已存在" << endl;
                return false;
            }
        }

        while (cur->_next != nullptr) {
            cur = cur->_next;
        }
        cur->_next = new FCB(dirName, "DIR", getTimeStr(), true, nullptr, nullptr, ptr);
        return true;
    }


    bool createFile(string fileName, int point) {  //创建文件
        FCB *cur = ptr;
        if (cur->_child == nullptr) {
            cur->_child = new FCB(fileName, "FILE", getTimeStr(), true, nullptr, nullptr, ptr);
            cur->_child->_pointer = point;
            return true;
        }

        FCB *child = cur->_child;
        cur = cur->_child;

        if (child->_name == fileName) {
            cout << "文件名已存在" << endl;
            return false;
        }

        while (child->_next != nullptr) {
            child = child->_next;
            if (child->_name == fileName) {
                cout << "文件名已存在" << endl;
                return false;
            }
        }

        while (cur->_next != nullptr) {
            cur = cur->_next;
        }
        cur->_next = new FCB(fileName, "FILE", getTimeStr(), true, nullptr, nullptr, ptr);
        cur->_next->_pointer = point;
        return true;
    }


    //删除文件夹
    bool deleteDir(string dirName) {
        FCB *cur = ptr;
        FCB *child = cur->_child;
        cur = cur->_child;

        bool isFirst = true;
        while (cur != nullptr) {
            if (cur->_name == dirName && cur->_child == nullptr && cur->_type == "DIR") {
                if (isFirst)
                    ptr->_child = cur->_next;
                else {
                    child->_next = cur->_next;
                }
                delete cur;
                return true;
            } else if (cur->_name == dirName && cur->_child != nullptr && cur->_type == "DIR") {
                cout << "文件夹不为空" << endl;
                return false;
            }
            if (!isFirst) {
                child = child->_next;
            }
            isFirst = false;
            cur = cur->_next;
        }
        cout << "文件夹不存在" << endl;
        return false;
    }


    //返回文件的FCB
    FCB *getFCB(string filename) {
        FCB *cur = ptr;
        while (cur != nullptr) {
            if (cur->_name == filename) {
                return cur;
            }
            if (cur != nullptr) {
                cur = cur->_next;
            }
        }
        cout << "文件不存在" << endl;
        return nullptr;
    }


    //显示当前目录下所有文件/文件夹
    void ls() {
        cout<<"";
        printf("%-20s%-20s%-20s\n", "文件名", "文件类型", "创建时间");
        FCB *cur = ptr;
        cur = cur->_child;
        bool flag = false;
        while (cur != nullptr) {
            printf("%-20s%-20s%-20s\n", cur->_name.c_str(), cur->_type.c_str(), cur->_createtime.c_str());
            cur = cur->_next;
            flag = true;
        }
        if (!flag)
            cout <<endl;
    }


    //得到磁盘地址pointer
    int getPointer(FCB *path, string name) {
        FCB *res = getFCB(name);
        if (res != nullptr) {
            return res->_pointer;
        }
        return -1;
    }

    void pwd() {
        FCB *cur = ptr;
        string res;
        while (cur != root) {
            res = "/" + ptr->_name + res;
            cur = cur->_parent;
        }
        if (res == "") {
            res = "/";
        }
        cout << res << endl;
    }
};


