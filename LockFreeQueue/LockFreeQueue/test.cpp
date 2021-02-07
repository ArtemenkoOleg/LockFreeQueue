#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "LockFreeQueue.h"

using namespace std;

void test_can_create_and_empty();
void test_can_push_and_size_changes();
void test_can_push_and_pop_correct_elements();
void test_can_push_multi_thread();
void test_can_pop_multi_thread();
void test_compare_time_push();

int main() {
  test_can_create_and_empty();
  test_can_push_and_size_changes();
  test_can_push_and_pop_correct_elements();
  test_can_push_multi_thread();
  test_can_pop_multi_thread();
  test_compare_time_push();
  cout << "All tests are passed!" << '\n';
  return 0;
}

void test_can_create_and_empty() {
  LockFreeQueue<int> q;

  assert(q.empty());
  assert(q.size() == 0);
}

void test_can_push_and_size_changes() {
  LockFreeQueue<int> q;
  q.push(1);
  q.push(2);
  q.push(3);
  q.push(4);
  q.push(5);

  assert(q.size() == 5);
}

void test_can_push_and_pop_correct_elements() {
  LockFreeQueue<int> q;
  q.push(1);
  q.push(2);
  q.push(3);

  auto one = q.pop();
  assert(1 == *one);
  assert(q.size() == 2);
  q.pop();
  auto three = q.pop();
  assert(3 == *three);

  assert(q.empty());
  assert(q.size() == 0);
}

void test_can_push_multi_thread() {
  LockFreeQueue<int> q;
  vector<thread> threads;
  const int THREAD_NUM = 100;
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.push_back(thread(&LockFreeQueue<int>::push, &q, i));
  }
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.at(i).join();
  }

  assert(q.size() == THREAD_NUM);  
}

void test_can_pop_multi_thread() {
  LockFreeQueue<int> q;
  const int THREAD_NUM = 100;

  for (int i = 0; i < THREAD_NUM; i++) {
    q.push(i);
  }

  vector<thread> threads;
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.push_back(thread(&LockFreeQueue<int>::pop, &q));
  }
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.at(i).join();
  }

  assert(q.empty());
  assert(q.size() == 0);
}

void test_compare_time_push() {
  const int THREAD_NUM = 10;
  LockFreeQueue<int> q1, q2;

  auto start = chrono::system_clock::now();
  for (int i = 0; i < THREAD_NUM; i++) {
    q1.push(i);
  }
  auto end = chrono::system_clock::now();
  auto elapsed = end - start;
  cout << "Time for push one thread version: " << elapsed.count() << '\n';

  vector<thread> threads;
  start = chrono::system_clock::now();
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.push_back(thread(&LockFreeQueue<int>::push, &q2, i));
  }
  for (int i = 0; i < THREAD_NUM; i++) {
    threads.at(i).join();
  }
  end = chrono::system_clock::now();
  elapsed = end - start;
  cout << "Time for push multi thread version: " << elapsed.count() << '\n';
}
