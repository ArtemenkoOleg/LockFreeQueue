#pragma once
#include <atomic>
#include <memory>

/*
Шаблонный класс, который реализует неблокирующую очередь 
*/
template <typename T>
class LockFreeQueue {
 private:
  size_t size_;

  struct node {
    std::shared_ptr<T> data;
    node* next;
    node() : next(nullptr) {}
  };
  std::atomic<node*> head;
  std::atomic<node*> tail;
  node* pop_head() {
    node* const old_head = head.load();
    if (old_head == tail.load()) {
      return nullptr;
    }
    head.store(old_head->next);    
    return old_head;
  }

 public:
  LockFreeQueue() : head(new node), tail(head.load()), size_(0) {}
  LockFreeQueue(const LockFreeQueue& other) = delete;
  LockFreeQueue& operator=(const LockFreeQueue& other) = delete;

  ~LockFreeQueue() {
    while (node* const old_head = head.load()) {
      head.store(old_head->next);
      delete old_head;
    }
  }

  bool empty() { return size_ == 0; }
  size_t size() { return size_; }

  std::shared_ptr<T> pop() {
    node* old_head = pop_head();
    if (!old_head) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> const res(old_head->data);
    delete old_head;
    size_--;
    return res;
  }

  void push(T new_value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
    node* p = new node;
    node* const old_tail = tail.load();
    old_tail->data.swap(new_data);
    old_tail->next = p;
    tail.store(p);
    size_++;
  }
};