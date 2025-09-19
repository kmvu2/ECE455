#include <iostream>
#include <vector>
#include <thread>
#include <numeric>
#include <random>
#include <chrono>

void partial_sum(const std::vector<int> &data, size_t start, size_t end, long long &out) {
  out = std::accumulate(data.begin() + start, data.begin() + end, 0LL);
}

int main() {
  const size_t N = 10000000;
  const int T = std::thread::hardware_concurrency() ? // get available number of threads
                std::thread::hardware_concurrency() : 4; // if for some reason fails, use 4 threads

  // random number generation
  std::vector<int> data(N);
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(1, 100);
  for (auto& x : data) x = dist(rng); // randomize each int in data

  // Baseline (single-threaded)
  auto t0 = std::chrono::high_resolution_clock::now();
  long long baseline = std::accumulate(data.begin(), data.end(), 0LL);
  auto t1 = std::chrono::high_resolution_clock::now();

  // Parallel
  std::vector<long long> partials(T, 0); // T = size, 0 = value
  std::vector<std::thread> threads;
  threads.reserve(T);
  size_t chunk = N/T; // amount of work per thread

  // each thread executes chunk of work
  auto p0 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < T; ++i) {
    size_t s = i*chunk;                   // start of chunk
    size_t e = (i == T-1) ? N : s+chunk;  // end of chunk; if last thread, get everything left (N)
    threads.emplace_back(partial_sum, std::cref(data), s, e, std::ref(partials[i])); // each thread sums a chunk, placed in partial
  }
  for (auto& th : threads) th.join();
  long long total = std::accumulate(partials.begin(), partials.end(), 0LL); // sum up all work
  auto p1 = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> t_base = t1 - t0;
  std::chrono::duration<double> t_par = p1 - p0;

  std::cout << "Baseline sum: " << baseline
            << " Time: " << t_base.count() << " s\n";
  std::cout << "Parallel sum: " << total
            << " Time: " << t_par.count() << " s\n";

  return 0;
}