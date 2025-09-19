#include <iostream> // for output
#include <thread> // for threads
#include <vector> // for containers

void hello(int id, int total) {
  std::cout << "Hello from thread " << id << " of " << total << "\n";
}

int main() {
  const int N = 5;
  std::vector<std::thread> threads;
  threads.reserve(N); // reserve/create 5 spaces for vector, compiler preallocates in advance

  for (int i = 0; i < N; ++i)
    // general container/constructor function, puts something in container/thread and thread executes
    threads.emplace_back(hello, i, N);

  for (auto& t : threads) t.join(); // range loop - iterates through all threads
  return 0;
}