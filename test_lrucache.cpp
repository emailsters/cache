#include "cache.h"
#include <stdio.h>
#include <stdlib.h>

struct Key 
{
    int a;
    int b;
    std::string openid;
    bool operator==(const Key& key) const 
    {
        return (a == key.a && b == key.b && openid == key.openid);
    }
};

struct HashFunc 
{
    size_t operator()(const Key& key) const 
    {
        return key.a+key.b;//+key.openid[1]+key.openid[2]; 
    } 
};

struct Value 
{
    char url[24];
};

int main() 
{
    //LruCache<Key, Value, HashFunc> cache(3);
    PreAllocLruCache<Key, Value, HashFunc> cache(3);
    cache.ApplyResource();
    Key key;
    key.a = 3;
    key.b = 4;
    key.openid = "openid1";
    Value value;
    snprintf(value.url, sizeof(value.url), "url1");
    cache.Put(key, value);
    key.openid = "openid1";
    snprintf(value.url, sizeof(value.url), "url1");
    cache.Put(key, value);
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid2";
    snprintf(value.url, sizeof(value.url), "url2");
    cache.Put(key, value);
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid3";
    snprintf(value.url, sizeof(value.url), "url3");
    cache.Put(key, value);
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid4";
    snprintf(value.url, sizeof(value.url), "url4");
    cache.Put(key, value);
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid3";
    const Value* v = cache.Get(key);
    std::cout<<v<<std::endl;
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid4";
    v = cache.Get(key);
    std::cout<<v->url<<std::endl;
    std::cout<<"total:"<<cache.Total()<<std::endl;

    key.openid = "openid1";
    v = cache.Get(key);
    std::cout<<v->url<<std::endl;
    std::cout<<"total:"<<cache.Total()<<std::endl;

    return 0;
}
