#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <numeric>
#include <cstdlib>
#include <iomanip>

using namespace std;

int n = 10000000;

struct ThreadInfo {
    const vector<int>* nums;
    int left;
    int right;
    long long result;
};

void calculate_sum(ThreadInfo* info) {
    info->result = 0;
    for (int i = info->left; i < info->right; i++) {
        info->result += (*info->nums)[i];
    }
}

void run_multithreaded_test(int thread_cnt, const vector<int>& nums, long long expected_sum) {
    auto start_time = chrono::high_resolution_clock::now();
    vector<thread> workers;
    vector<ThreadInfo> thread_data(thread_cnt);
    int block_size = n / thread_cnt;
    for (int i = 0; i < thread_cnt; i++){
        thread_data[i].nums = &nums;
        thread_data[i].left = i * block_size;
        if (i == thread_cnt - 1)
            thread_data[i].right = n;
        else
            thread_data[i].right = (i + 1) * block_size;
        workers.push_back(thread(calculate_sum, &thread_data[i]));
    }
    long long sum_multi = 0;
    for (int i = 0; i < thread_cnt; i++){
        workers[i].join();
        sum_multi += thread_data[i].result;
    }
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_multi = end_time - start_time;

    cout << "Threads: " << thread_cnt << endl;
    cout << "Sum: " << sum_multi << endl;
    cout << "Time: " << fixed << setprecision(6) << duration_multi.count() << " sec" << endl;

    if (sum_multi == expected_sum)
        cout << "Status: SUCCESS (Sums match)" << endl;
    else
        cout << "Status: ERROR (Sums do not match!)" << endl;
    cout << "------------------------------------------\n";
}

int main(){

    cout << "Generating array of size " << n << "...\n";
    vector<int> nums(n);
    srand(time(0));
    for (int i = 0; i < n; i++){
        nums[i] = rand() % 100;
    }
    auto start_time = chrono::high_resolution_clock::now();
    long long sum_single = 0;
    for (int val : nums){
        sum_single += val;
    }
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_single = end_time - start_time;

    cout << "Single-Threaded Sum: " << sum_single << endl;
    cout << "Time: " << fixed << setprecision(6) << duration_single.count() << " sec\n";
    cout << "------------------------------------------\n";

    vector<int> test_counts = {2, 4, 8};    
    for (int t : test_counts) {
        run_multithreaded_test(t, nums, sum_single);
    }
    return 0;
}