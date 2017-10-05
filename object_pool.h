#ifndef OBJECT_POOL_H_
#define OBJECT_POOL_H_
#include <iostream>

template <typename T>
class ObjectPool
{
public:
    ObjectPool(int capacity);
    ~ObjectPool();
    int ApplyResource();
    void ReleaseResource();
    int Alloc();
    void Free(int index);
    typedef void (*TravelFunc)(const T* t);
    void Travelse(TravelFunc func);
    T* GetObject(int index);
    int Capacity() const
    {
        return capacity_;
    }
    int Used() const
    {
        return used_;
    }
    
private:
    enum ListNodeFlag
    {
        kFreed = 0,
        kUsed = 1,
    };
    
    struct NodeHead
    {
        int prev;
        int next;
        int index;
        int flag;
        NodeHead()
            : prev(-1), next(-1), index(-1) {}
    };
    struct ListNode
    {
        NodeHead head;
        T value;
    };
    
    void AddNodeToUsedList();
    void AddNodeToFreedList(ListNode* node);
    
    ListNode* free_list_;
    ListNode* used_list_;
    int used_;
    int free_;
    int capacity_;
    ListNode* block_start_;
};

template <typename T>
ObjectPool<T>::ObjectPool(int capacity)
    : free_list_(NULL), used_list_(NULL), used_(0), free_(0),
      capacity_(capacity), block_start_(NULL)
{
}

template <typename T>
ObjectPool<T>::~ObjectPool() {}

template <typename T>
int ObjectPool<T>::ApplyResource()
{
    int mem_total = capacity_ * sizeof(ListNode);
    block_start_ = new ListNode[mem_total];
    if (block_start_ == NULL)
    {
        return -1;
    }
    
    free_list_ = block_start_;
    
    for (int i = 0; i < capacity_; ++i)
    {
        ListNode* node = free_list_ + i;
        node->head.index = i;
        node->head.flag = kFreed;
        node->head.next = i + 1;
        node->head.prev = i - 1;
    }
    
    free_list_[capacity_ - 1].head.next = -1;
    free_ = capacity_;
    return 0;
}

template <typename T>
void ObjectPool<T>::ReleaseResource()
{
    delete[] block_start_;
    free_list_ = NULL;
    used_list_ = NULL;
    block_start_ = NULL;
    used_ = 0;
    free_ = 0;
}

template <typename T>
int ObjectPool<T>::Alloc()
{
    if (free_list_ == NULL)
    {
        return -1;
    }
    
    ListNode* list_node = free_list_;
    list_node->head.flag = kUsed;
    AddNodeToUsedList();
    return list_node->head.index;
}

template <typename T>
T* ObjectPool<T>::GetObject(int index)
{
    if (index >= capacity_ || index < 0)
    {
        return NULL;
    }
    
    ListNode* node = &block_start_[index];
    if (node->head.flag == kFreed)
    {
        return NULL;
    }
    
    return &node->value;
}

template <typename T>
void ObjectPool<T>::Free(int index)
{
    if (index >= capacity_ || index < 0)
    {
        return;
    }
    
    ListNode* current_used_node = &block_start_[index];
    ListNode* next_used_node = NULL;
    ListNode* prev_used_node = NULL;
    
    if (current_used_node->head.next != -1)
    {
        next_used_node = &block_start_[current_used_node->head.next];
    }
    
    if (current_used_node->head.prev != -1)
    {
        prev_used_node = &block_start_[current_used_node->head.prev];
    }
    
    if (next_used_node)
    {
        next_used_node->head.prev = current_used_node->head.prev;
    }
    
    if (prev_used_node)
    {
        prev_used_node->head.next = current_used_node->head.next;
    }
    else
    {
        used_list_ = next_used_node;
    }
    
    AddNodeToFreedList(current_used_node);
}

template <typename T>
void ObjectPool<T>::AddNodeToFreedList(ListNode* node)
{
    node->head.prev = -1;
    
    if (free_list_)
    {
        node->head.next = free_list_->head.index;
    }
    else
    {
        node->head.next = -1;
    }
    
    free_list_ = node;
    ++free_;
    --used_;
}

template <typename T>
void ObjectPool<T>::AddNodeToUsedList()
{
    ListNode* next_free_node = NULL;
    if (free_list_->head.next != -1)
    {
        next_free_node = &block_start_[free_list_->head.next];
        next_free_node->head.prev = -1;
    }
    
    ListNode* current_free_node = free_list_;
    if (used_list_)
    {
        current_free_node->head.next = used_list_->head.index;
        used_list_->head.prev = current_free_node->head.index;
        used_list_ = current_free_node;
    }
    else
    {
        used_list_ = current_free_node;
        used_list_->head.prev = -1;
        used_list_->head.next = -1;
    }
    
    free_list_ = next_free_node;
    ++used_;
    --free_;
}

template <typename T>
void ObjectPool<T>::Travelse(TravelFunc func)
{
    if (func == NULL)
    {
        return;
    }
    
    ListNode* node = used_list_;
    while (node != NULL)
    {
        func(&node->value);
        std::cout << "index:" << node->head.index << "prev:" << node->head.prev <<
                  "next:" << node->head.next << std::endl;
        ListNode* next = NULL;
        
        if (node->head.next != -1)
        {
            next = &block_start_[node->head.next];
        }
        
        node = next;
    }
}

#endif
