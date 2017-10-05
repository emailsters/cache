#include "object_pool.h"
#include <iostream>

struct MyStruct 
{
    int a;
    int b;
};

void Mytravel(const MyStruct* st) 
{
    std::cout<<"helloworld"<<std::endl;
}

int main() 
{
    ObjectPool<MyStruct> pool(3);
    pool.ApplyResource();

    int index =  pool.Alloc();
    MyStruct* st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;

    index =  pool.Alloc();
    st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;

    index =  pool.Alloc();
    st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;

    index =  pool.Alloc();
    st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;

    index =  pool.Alloc();
    st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;

    pool.Free(2);
    index =  pool.Alloc();
    st = pool.GetObject(index);
    std::cout<<"index:"<<index<<std::endl;
    std::cout<<"pointer:"<<st<<std::endl;
    std::cout<<"-------------"<<std::endl;
    pool.Travelse(Mytravel);
    pool.ReleaseResource();
    return 0;
}
