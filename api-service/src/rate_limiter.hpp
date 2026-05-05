#pragma once
#include <map>
#include <chrono>
#include <mutex>
#include <string>

using namespace std;

class RateLimiter {
public:
    RateLimiter(int capacity, int refill_period_seconds);
    bool TryConsume(const string& identifier, int tokens = 1);
    int GetRemainingTokens(const string& identifier);
    int GetRetryAfterSeconds(const string& identifier);
    void Reset(const string& identifier);
    void Clear();

private:
    struct TokenBucket {
        double tokens;
        chrono::steady_clock::time_point last_refill;
    };
    const int capacity_;
    const int refill_period_seconds_;
    const double refill_rate_;
    mutable mutex mutex_;
    map<string, TokenBucket> buckets_;
    TokenBucket& GetBucket(const string& identifier);
    void RefillTokens(TokenBucket& bucket);
};
    