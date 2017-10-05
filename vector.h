#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <stdlib.h>
#include <string.h>
#include <iostream>

template <typename T>
class Vector {
 public:
  Vector();
  ~Vector();
    void PushBack(const T& t);
    void PopBack();

    T operator[](int n) {
        return elements_[n];
    }

    class Iterator {
     public:
        explicit Iterator(Vector<T>* vector) {
            vector_ = vector;
            index_ = 0;
        }

        ~Iterator() {}
        bool HasNext();
        T Next();
        void Reset() {
            index_ = 0;
        }
        int index_;
        Vector<T>* vector_;
    };

 private:
    void DoCopy(T* t);
    T* elements_;
    int count_;
    int capacity_;
};

const int kDefaultVectorSize = 100;
const int kExpandStep = 2;

template <typename T>
Vector<T>::Vector()
        : count_(0), capacity_(kDefaultVectorSize) {
    elements_ = new T[kDefaultVectorSize*sizeof(T)];
}

template <typename T>
Vector<T>::~Vector() {
    delete[] elements_;
}

template <typename T>
void Vector<T>::PushBack(const T& t) {
    if (count_ >= capacity_) {
        int next_mem_size = kExpandStep * capacity_ * sizeof(t);
        T* new_elements = new T(next_mem_size);
        if (!new_elements) {
            return;
        }
        capacity_ = kExpandStep * capacity_;
        DoCopy(new_elements);
        delete[] elements_;
        elements_ = new_elements;
    }
    memcpy(elements_ + count_, &t, sizeof(t));
    ++count_;
}

template <typename T>
void Vector<T>::PopBack() {
    --count_;
}

template <typename T>
void Vector<T>::DoCopy(T* new_elements) {
    memcpy(new_elements, elements_, sizeof(T) * count_);
}

template <typename T>
bool Vector<T>::Iterator::HasNext() {
    if (index_ < vector_->count_) {
        return true;
    }
    return false;
}

template <typename T>
T Vector<T>::Iterator::Next() {
    return vector_->elements_[index_++];
}
#endif  // _VECTOR_H_

