# include < iostream >
# include < thread >
# include < vector >
# include < mutex >
# include < atomic >
# include < chrono >
constexpr int ITER = 100000; // number of iterations

void inc_no_lock(int& counter) {
  for(int i = 0; i < ITER; ++i) counter++; // data race !
}

// mutex = software lock
void inc_with_mutex(int& counter, std::mutex &m) {
  for (int i = 0; i < ITER; ++i) {
    std::lock_guard < std::mutex > lk(m);
    ++counter;
  }
}

// atomic - hardware creates lock; slightly more efficient, but doesn't matter for lightweight applications; easier syntax
void inc_atomic(std::atomic < int > &counter) {
  for (int i = 0; i < ITER; ++i) counter.fetch_add(1, std::memory_order_relaxed);
}

// I want this function to handle several/all types of functions (normal, lambda, etc.)
template <typename F> // template parameter - like Generics in Java - tells compiler to handle function type
int run_and_time(int T, F&& fn) { // runs code and times execution for you; F&& = generic type of function
  auto t0 = std::chrono::high_resolution_clock::now();
  std::vector < std::thread > ths;
  ths.reserve(T);
  for (int i = 0; i < T; ++i) ths.emplace_back(fn);
  for (auto& t : ths) t.join();
  auto t1 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main () { // T*# iterations should be correct result
  const int T = std::thread::hardware_concurrency() ?
                std::thread::hardware_concurrency() : 4;
  const int expected = T * ITER;
  
  { // No lock ( incorrect )
    int counter = 0 ;
    auto ms = run_and_time(T, [&]{ inc_no_lock(counter); }); // lambda function; can use either, but lambda saves coding time
    std::cout << " [ No lock ] counter = " << counter
              << " (expected " << expected << "), "
              << ms << " ms\n";
  }

  { // Mutex
  int counter = 0;
  std::mutex m;
  auto ms = run_and_time(T, [&]{ inc_with_mutex(counter, m); });
  std::cout << "[Mutex] counter = " << counter
            << " (expected " << expected << "), "
            << ms << " ms\n";
  }

  { // Atomic
  std::atomic<int> counter{0};
  auto ms = run_and_time(T, [&]{ inc_atomic(counter); });
  std::cout << " [Atomic] counter = " << counter.load() // get counter variable, protected by hardware atomic; counter alone also works, but compiler will see it's an atomic variable and add .load() implicitly
            << " (expected " << expected << " ), "
            << ms << " ms\n" ;
  }
  return 0 ;
}