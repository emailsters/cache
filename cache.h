#ifndef CACHE_H_
#define CACHE_H_

#include <tr1/unordered_map>
#include <iostream>
#include "object_pool.h"

template <typename DataKey, typename DataValue, typename HashFunc>
class LruCache
{
public:
    explicit LruCache(int capacity);
    ~LruCache();
    const DataValue* Get(const DataKey& key);
    void Put(const DataKey& key, const DataValue& value);
    
    int Total()
    {
        return map_.size();
    }
    // do not expire currently
    // void Expire();
    
private:
    struct ListStorage
    {
        DataKey key;
        DataValue value;
        // uint64_t expire;
    };
    struct ListNode
    {
        ListStorage storage;
        ListNode* prev;
        ListNode* next;
    };
    
    void DeleteLast();
    void MoveToFirst(ListNode* node);
    
    int capacity_;
    int usage_;
    
    ListNode* head_;
    ListNode* tail_;
    typedef std::tr1::unordered_map<DataKey, ListNode*, HashFunc> HashMap;
    typedef typename
        std::tr1::unordered_map<DataKey, ListNode*, HashFunc>::iterator HashMapItor;
    HashMap map_;
};

template <typename DataKey, typename DataValue, typename HashFunc>
LruCache<DataKey, DataValue, HashFunc>::LruCache(int capacity)
    : capacity_(capacity), usage_(0), head_(NULL), tail_(NULL)
{
}

template <typename DataKey, typename DataValue, typename HashFunc>
LruCache<DataKey, DataValue, HashFunc>::~LruCache()
{
    ListNode* list = head_;
    while (list != NULL)
    {
        ListNode* node = list->next;
        delete list;
        list = node;
    }
    map_.clear();
}

template <typename DataKey, typename DataValue, typename HashFunc>
const DataValue* LruCache<DataKey, DataValue, HashFunc>::Get(
    const DataKey& key)
{
    HashMapItor itor = map_.find(key);
    if (itor == map_.end())
    {
        return NULL;
    }
    MoveToFirst(itor->second);
    return &itor->second->storage.value;
}

template <typename DataKey, typename DataValue, typename HashFunc>
void LruCache<DataKey, DataValue, HashFunc>::Put(const DataKey& key,
                                                 const DataValue& value)
{
    if (map_.size() >= capacity_)
    {
        // Expire();
        DeleteLast();
    }
    HashMapItor itor = map_.find(key);
    if (itor != map_.end())
    {
        // replace value
        itor->second->storage.value = value;
        return;
    }
    ListNode* node = new ListNode();
    if (!node)
    {
        return;
    }
    node->storage.key = key;
    node->storage.value = value;
    // time(&node->storage.expire);
    node->prev = NULL;
    node->next = NULL;
    if (head_ == NULL)
    {
        head_ = tail_ = node;
    }
    else
    {
        tail_->next = node;
        node->prev = tail_;
        tail_ = node;
    }
    map_.insert(std::make_pair(key, tail_));
}

template <typename DataKey, typename DataValue, typename HashFunc>
void LruCache<DataKey, DataValue, HashFunc>::DeleteLast()
{
    if (tail_ == NULL)
    {
        return;
    }
    HashMapItor itor = map_.find(tail_->storage.key);
    if (itor == map_.end())
    {
        return;
    }
    map_.erase(itor);
    ListNode* node = tail_->prev;
    delete tail_;
    node->next = NULL;
    tail_ = node;
}

template <typename DataKey, typename DataValue, typename HashFunc>
void LruCache<DataKey, DataValue, HashFunc>::MoveToFirst(ListNode* node)
{
    ListNode* prev = node->prev;
    if (prev == NULL)
    {
        return;
    }
    prev->next = node->next;
    if (node->next)
    {
        node->next->prev = prev;
    }
    node->next = head_;
    node->prev = NULL;
    head_->prev = node;
    head_ = node;
}

template <typename DataKey, typename DataValue, typename HashFunc>
class PreAllocLruCache
{
public:
    explicit PreAllocLruCache(int capacity);
    ~PreAllocLruCache();
    int ApplyResource();
    const DataValue* Get(const DataKey& key);
    void Put(const DataKey& key, const DataValue& value);
    int Total()
    {
        return map_.size();
    }
    
private:
    struct ListStorage
    {
        DataKey key;
        DataValue value;
        int pool_idx;
    };
    struct ListNode
    {
        ListStorage storage;
        ListNode* prev;
        ListNode* next;
    };
    
    void DeleteLast();
    void MoveToFirst(ListNode* node);
    
    int capacity_;
    int usage_;
    
    ListNode* head_;
    ListNode* tail_;
    typedef std::tr1::unordered_map<DataKey, ListNode*, HashFunc> HashMap;
    typedef typename
        std::tr1::unordered_map<DataKey, ListNode*, HashFunc>::iterator HashMapItor;
    HashMap map_;
    ObjectPool<ListNode>* obj_pool_;
};

template <typename DataKey, typename DataValue, typename HashFunc>
PreAllocLruCache<DataKey, DataValue, HashFunc>::PreAllocLruCache(int capacity)
    : capacity_(capacity), usage_(0), head_(NULL), tail_(NULL)
{
    obj_pool_ = new ObjectPool<ListNode>(capacity);
}

template <typename DataKey, typename DataValue, typename HashFunc>
PreAllocLruCache<DataKey, DataValue, HashFunc>::~PreAllocLruCache()
{
    if (obj_pool_)
    {
        obj_pool_->ReleaseResource();
    }
    map_.clear();
}

template <typename DataKey, typename DataValue, typename HashFunc>
int PreAllocLruCache<DataKey, DataValue, HashFunc>::ApplyResource()
{
    if (obj_pool_ == NULL)
    {
        return -1;
    }
    return obj_pool_->ApplyResource(); 
}

template <typename DataKey, typename DataValue, typename HashFunc>
const DataValue* PreAllocLruCache<DataKey, DataValue, HashFunc>::Get(
    const DataKey& key)
{
    HashMapItor itor = map_.find(key);
    if (itor == map_.end())
    {
        return NULL;
    }
    MoveToFirst(itor->second);
    return &itor->second->storage.value;
}

template <typename DataKey, typename DataValue, typename HashFunc>
void PreAllocLruCache<DataKey, DataValue, HashFunc>::Put(const DataKey& key,
                                                 const DataValue& value)
{
    if (map_.size() >= capacity_)
    {
        // Expire();
        DeleteLast();
    }
    HashMapItor itor = map_.find(key);
    if (itor != map_.end())
    {
        // replace value
        itor->second->storage.value = value;
        return;
    }
    int idx = obj_pool_->Alloc();
    if (idx < 0)
    {
        return;
    }
    ListNode* node = obj_pool_->GetObject(idx);
    if (!node)
    {
        return;
    }
    node->storage.key = key;
    node->storage.value = value;
    node->storage.pool_idx = idx;
    node->prev = NULL;
    node->next = NULL;
    if (head_ == NULL)
    {
        head_ = tail_ = node;
    }
    else
    {
        tail_->next = node;
        node->prev = tail_;
        tail_ = node;
    }
    map_.insert(std::make_pair(key, tail_));
}

template <typename DataKey, typename DataValue, typename HashFunc>
void PreAllocLruCache<DataKey, DataValue, HashFunc>::DeleteLast()
{
    if (tail_ == NULL)
    {
        return;
    }
    HashMapItor itor = map_.find(tail_->storage.key);
    if (itor == map_.end())
    {
        return;
    }
    map_.erase(itor);
    ListNode* node = tail_->prev;
    obj_pool_->Free(tail_->storage.pool_idx);
    node->next = NULL;
    tail_ = node;
}

template <typename DataKey, typename DataValue, typename HashFunc>
void PreAllocLruCache<DataKey, DataValue, HashFunc>::MoveToFirst(ListNode* node)
{
    ListNode* prev = node->prev;
    if (prev == NULL)
    {
        return;
    }
    prev->next = node->next;
    if (node->next)
    {
        node->next->prev = prev;
    }
    node->next = head_;
    node->prev = NULL;
    head_->prev = node;
    head_ = node;
}

#endif

