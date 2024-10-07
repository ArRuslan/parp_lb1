#include <chrono>
#include <cstring>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <random>
//#include <format>
#include <x86intrin.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <wine/msvcrt/intrin.h>

void ConvertTimeTToSystemTime(SYSTEMTIME* stime, time_t* time){
    tm my_tm;
    localtime_r(time, &my_tm);
    memset(stime, 0, sizeof(SYSTEMTIME));
    stime->wYear = my_tm.tm_year + 1900;
    stime->wMonth = my_tm.tm_mon + 1;
    stime->wDay = my_tm.tm_mday;
}

#ifndef MAXFLOAT
#define MAXFLOAT 3.40282347e+38F
#endif

int _cpuid(uint32_t* info, uint32_t func_id) {
    __cpuid((int32_t*)info, func_id);
    return 0;
}
#else
#include <cpuid.h>

int _cpuid(uint32_t* info, uint32_t func_id) {
    return __get_cpuid(func_id, info, info + 1, info + 2, info + 3);
}
#endif

#define FILL_ZERO(NUM, ZEROS) std::setfill('0') << std::setw(ZEROS) << NUM


void task_1() {
    time_t last_time = 0x7FFFFFFF;
    const auto l_time = gmtime(&last_time);
    //std::cout << std::format("Year: {}", 1900 + l_time->tm_year);
    std::cout << "Year: " << 1900 + l_time->tm_year
        << ", Month: " << FILL_ZERO(l_time->tm_mon + 1, 2)
        << ", Day: " << FILL_ZERO(l_time->tm_mday, 2) << std::endl;

    std::cout << "Full time: " << FILL_ZERO(l_time->tm_mday, 2)
        << "." << FILL_ZERO(l_time->tm_mon + 1, 2)
        << "." << 1900 + l_time->tm_year
        << " " << FILL_ZERO(l_time->tm_hour, 2)
        << ":" << FILL_ZERO(l_time->tm_min, 2)
        << ":" << FILL_ZERO(l_time->tm_sec, 2)
        << std::endl;

#if defined(_WIN32) || defined(WIN32)
    SYSTEMTIME stime;
    ConvertTimeTToSystemTime(&stime, &last_time);
    printf("SYSTEMTIME: %4.4d.%2.2d.%2.2d\n", stime.wYear, stime.wMonth, stime.wDay);
#endif
}

time_t task_2_time() {
    const time_t start = time(nullptr);
    time_t finish = start;
    while (finish == start) {
        finish = time(nullptr);
    }

    return finish - start;
}

clock_t task_2_clock() {
    const clock_t start = clock();
    clock_t finish = start;
    do {
        finish = clock();
    } while (finish - start == 0);

    return finish - start;
}

uint64_t task_2_GetSystemTimeAsFileTime() {
    FILETIME start, finish;
    GetSystemTimeAsFileTime(&start);
    memcpy(&finish, &start, sizeof(FILETIME));

    do {
        GetSystemTimeAsFileTime(&finish);
    } while (finish.dwHighDateTime - start.dwHighDateTime == 0 && finish.dwLowDateTime - start.dwLowDateTime == 0);

    ULARGE_INTEGER result;
    result.LowPart = finish.dwLowDateTime - start.dwLowDateTime;
    result.HighPart = finish.dwHighDateTime - start.dwHighDateTime;

    return result.QuadPart;
}

uint64_t task_2_GetSystemTimePreciseAsFileTime() {
    FILETIME start, finish;
    GetSystemTimePreciseAsFileTime(&start);
    memcpy(&finish, &start, sizeof(FILETIME));

    do {
        GetSystemTimePreciseAsFileTime(&finish);
    } while (finish.dwHighDateTime - start.dwHighDateTime == 0 && finish.dwLowDateTime - start.dwLowDateTime == 0);

    ULARGE_INTEGER result;
    result.LowPart = finish.dwLowDateTime - start.dwLowDateTime;
    result.HighPart = finish.dwHighDateTime - start.dwHighDateTime;

    return result.QuadPart;
}

uint32_t task_2_GetTickCount() {
    uint32_t start = GetTickCount(), finish = start;

    do {
        finish = GetTickCount();
    } while (finish - start == 0);

    return finish - start;
}

uint64_t task_2_rdtsc() {
    uint64_t start = __rdtsc(), finish = start;

    do {
        finish = __rdtsc();
    } while (finish - start == 0);

    return finish - start;
}

int64_t task_2_QueryPerformanceCounter() {
    LARGE_INTEGER start, finish;
    QueryPerformanceCounter(&start);
    memcpy(&finish, &start, sizeof(LARGE_INTEGER));

    do {
        QueryPerformanceCounter(&finish);
    } while (finish.QuadPart - start.QuadPart == 0);

    return finish.QuadPart - start.QuadPart;
}

double task_2_omp() {
    const double start = omp_get_wtime();
    double finish = start;
    do {
        finish = omp_get_wtime();
    } while (finish - start == 0);

    return finish - start;
}

double task_2_chrono() {
    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point finish = start;
    while ((finish - start).count() == 0) {
        finish = std::chrono::high_resolution_clock::now();
    }
    return std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
}

void task_2() {
    std::cout << "time: " << task_2_time() << " seconds" << std::endl;
    std::cout << "clock: " << task_2_clock() << " milliseconds" << std::endl;
    std::cout << "GetSystemTimeAsFileTime: " << task_2_GetSystemTimeAsFileTime() << " nanoseconds?" << std::endl;
    std::cout << "GetSystemTimePreciseAsFileTime: " << task_2_GetSystemTimePreciseAsFileTime() << " nanoseconds?" << std::endl;
    std::cout << "GetTickCount: " << task_2_GetTickCount() << " milliseconds" << std::endl;
    std::cout << "__rdtsc: " << task_2_rdtsc() << " tacts" << std::endl;
    std::cout << "QueryPerformanceCounter: " << task_2_QueryPerformanceCounter() << " nanoseconds?" << std::endl;
    const auto chrono_res = task_2_chrono();
    std::cout << "std::chrono: " << std::fixed << std::setprecision(10) << chrono_res << " seconds (" << std::setprecision(0) << (chrono_res * 1000000000) << " nanoseconds)" << std::endl;
    const auto omp_res = task_2_omp();
    std::cout << "OpenMP: " << std::fixed << std::setprecision(10) << omp_res << " seconds (" << std::setprecision(0) << (omp_res * 1000000000) << " nanoseconds)" << std::endl;
}

uint64_t task_3_rdtsc(int* arr, int& result) {
    uint64_t start = __rdtsc();

    result = 0;
    for (int i = 0; i < 1000; i++) {
        result += arr[i];
    }

    return __rdtsc() - start;
}

int64_t task_3_QueryPerformanceCounter(int* arr, int& result) {
    LARGE_INTEGER start, finish;
    QueryPerformanceCounter(&start);

    result = 0;
    for (int i = 0; i < 1000; i++) {
        result += arr[i];
    }

    QueryPerformanceCounter(&finish);
    return finish.QuadPart - start.QuadPart;
}

void task_3() {
    int arr[1000];
    std::uniform_int_distribution<int> randint(-1024, 1024);
    std::default_random_engine eng;

    for (int& i : arr) {
        i = randint(eng);
    }

    int result;
    std::cout << "__rdtsc: " << task_3_rdtsc((int*)&arr, result) << " tacts, result: " << result << std::endl;
    std::cout << "QueryPerformanceCounter: " << task_3_QueryPerformanceCounter((int*)&arr, result) << " nanoseconds, result: " << result << std::endl;
}

double task_4_abs(int* arr, size_t size, int& result) {
    double start = omp_get_wtime();

    for (int i = 0; i < size; i++) {
        result += arr[i];
    }

    return omp_get_wtime() - start;
}

uint32_t task_4_rel(int* arr, size_t size, int& result) {
    uint32_t start = GetTickCount();

    uint32_t iters = 0;
    while (GetTickCount() - start < 2000) {
        result = 0;
        for (int i = 0; i < size; i++) {
            result += arr[i];
        }
        iters++;
    }

    return iters;
}

void task_4() {
    int* arr100 = new int[100000];
    int* arr200 = new int[200000];
    int* arr300 = new int[300000];
    std::uniform_int_distribution<int> randint(-1024, 1024);
    std::default_random_engine eng;

    for (int i = 0; i < 100000; i++)
        arr100[i] = randint(eng);
    for (int i = 0; i < 200000; i++)
        arr200[i] = randint(eng);
    for (int i = 0; i < 300000; i++)
        arr300[i] = randint(eng);

    int result;
    double ta1, ta2, ta3;
    std::cout << "absolute 100k: " << std::fixed << std::setprecision(6) << (ta1 = task_4_abs(arr100, 100000, result)) << " seconds, result: " << result << std::endl;
    std::cout << "absolute 200k: " << std::fixed << std::setprecision(6) << (ta2 = task_4_abs(arr200, 200000, result)) << " seconds, result: " << result << std::endl;
    std::cout << "absolute 300k: " << std::fixed << std::setprecision(6) << (ta3 = task_4_abs(arr300, 300000, result)) << " seconds, result: " << result << std::endl;
    std::cout << "T(200000)/T(100000) = " << std::fixed << std::setprecision(3) << ta2 / ta1 << std::endl;
    std::cout << "T(300000)/T(100000) = " << std::fixed << std::setprecision(3) << ta3 / ta1 << std::endl;

    uint32_t tr1, tr2, tr3;
    std::cout << "relative 100k: " << (tr1 = task_4_rel(arr100, 100000, result)) << " iterations, result: " << result << std::endl;
    std::cout << "relative 200k: " << (tr2 = task_4_rel(arr200, 200000, result)) << " iterations, result: " << result << std::endl;
    std::cout << "relative 300k: " << (tr3 = task_4_rel(arr300, 300000, result)) << " iterations, result: " << result << std::endl;
    std::cout << "T(200000)/T(100000) = " << std::fixed << std::setprecision(3) << (double)tr2 / tr1 << ", T(100000)/T(200000): "<< (double)tr1 / tr2 << std::endl;
    std::cout << "T(300000)/T(100000) = " << std::fixed << std::setprecision(3) << (double)tr3 / tr1 << ", T(100000)/T(300000): "<< (double)tr1 / tr3 << std::endl;

    delete[] arr100, arr200, arr300;
}

template <typename T>
void task_9_ijk(T** a, T** b, T** out, const size_t size) {
    //memset(out, 0, size * size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                out[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

template<typename T>
T** task_9_create_mat(size_t size) {
    T** result = new T*[size];
    for(int i = 0; i < size; i++)
        result[i] = new T[size];

    return result;
}

template<typename T>
void task_9_free_mat(T** mat, size_t size) {
    for(int i = 0; i < size; i++)
        delete[] mat[i];

    delete[] mat;
}

template<typename T>
void task_9_measure_int(const std::string& typeName, size_t mat_size) {
    auto** mat_a = task_9_create_mat<T>(mat_size);
    auto** mat_b = task_9_create_mat<T>(mat_size);
    auto** mat_c = task_9_create_mat<T>(mat_size);

    std::uniform_real_distribution<double> randint;
    std::default_random_engine eng;

    for(int i = 0; i < mat_size; i++) {
        for(int j = 0; j < mat_size; j++) {
            mat_a[i][j] = (T)randint(eng);
            mat_b[i][j] = (T)randint(eng);
        }
    }

    const double start = omp_get_wtime();
    task_9_ijk<T>(mat_a, mat_b, mat_c, mat_size);
    std::cout << mat_size << "x" << mat_size << " " << typeName << ": " << std::fixed << std::setprecision(6) << (omp_get_wtime() - start) << " seconds" << std::endl;

    task_9_free_mat<T>(mat_a, mat_size);
    task_9_free_mat<T>(mat_b, mat_size);
    task_9_free_mat<T>(mat_c, mat_size);
}

void task_9() {
    for(int i = 0; i < 3; i++) {
        int size = 512 * std::pow(2, i);

        task_9_measure_int<int8_t>("int8", size);
        task_9_measure_int<int16_t>("int16", size);
        task_9_measure_int<int32_t>("int32", size);
        task_9_measure_int<int64_t>("int64", size);
        task_9_measure_int<float>("float", size);
        task_9_measure_int<double>("double", size);
    }
}

int main() {
    /*task_1();
    std::cout << "\n";

    task_2();
    std::cout << "\n";

    task_3();
    std::cout << "\n";

    task_4();
    std::cout << "\n";*/

    task_9();

    return 0;
}
