#include <iostream>
#include <vector.h>
int main()
{
    Vector<int> v;
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    Vector<int>::Iterator itor(&v);
    while (itor.HasNext()) {
        std::cout<<itor.Next()<<std::endl;
    }
    std::cout<<"-------------------------"<<std::endl;
    v.PushBack(10);
    v.PushBack(3);
    v.PushBack(39);
    v.PushBack(44);
    itor.Reset();
    while (itor.HasNext()) {
        std::cout<<itor.Next()<<std::endl;
    }
    v.PopBack();
    itor.Reset();
    std::cout<<"----------------------------"<<std::endl;
    while (itor.HasNext()) {
        std::cout<<itor.Next()<<std::endl;
    }
    return 0;
}
