#include <iostream>
#include"Connection.h"
#include"CommonConnectionPool.h"
using namespace std;





int main01()
{
    /*Connection conn;
    char sql[1024] = { 0 };
    sprintf(sql,
        "insert into user(name, age, sex) values ('%s', '%d', '%s')",
        "zhangsan", 20, "m");
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
    conn.update(sql);*/

    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
    clock_t begin = clock();

    thread t1([] { 
        for (int i = 0; i < 2500; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql,
                "insert into user(name, age, sex) values ('%s', '%d', '%s')",
                "zhangsan", 20, "m");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
        });

    thread t2([] {
        for (int i = 0; i < 2500; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql,
                "insert into user(name, age, sex) values ('%s', '%d', '%s')",
                "zhangsan", 20, "m");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
        });

    thread t3([] {
        for (int i = 0; i < 2500; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql,
                "insert into user(name, age, sex) values ('%s', '%d', '%s')",
                "zhangsan", 20, "m");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
        });

    thread t4([] {
        for (int i = 0; i < 2500; i++) {
            Connection conn;
            char sql[1024] = { 0 };
            sprintf(sql,
                "insert into user(name, age, sex) values ('%s', '%d', '%s')",
                "zhangsan", 20, "m");
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
            conn.update(sql);
        }
        });


    t1.join();
    t2.join();
    t3.join();
    t4.join();
    clock_t end = clock();
    cout << (end - begin) << "ms" << endl;

    return 0;
}
