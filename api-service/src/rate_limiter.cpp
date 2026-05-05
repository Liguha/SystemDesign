#include <algorithm>
#include "rate_limiter.hpp"       

using namespace std;

RateLimiter::RateLimiter(int capacity, int refill_period_seconds)
    : capacity_(capacity),  
      refill_period_seconds_(refill_period_seconds),        
      refill_rate_(static_cast<double>(capacity) / refill_period_seconds) {}        

bool RateLimiter::TryConsume(const string& identifier, int tokens) {
    lock_guard<mutex> lock(mutex_);
    TokenBucket& bucket = GetBucket(identifier);
    RefillTokens(bucket);
    
    if (bucket.tokens >= tokens) {
        bucket.tokens -= tokens;             
        return true;       
    }    
    return false;
}

int RateLimiter::GetRemainingTokens(const string& identifier) {
    lock_guard<mutex> lock(mutex_);          
    auto it = buckets_.find(identifier);    
    if (it == buckets_.end()) {         
        return capacity_;        
    }
    
    TokenBucket bucket = it->second;  
    RefillTokens(bucket);           
    return static_cast<int>(bucket.tokens);     
}

int RateLimiter::GetRetryAfterSeconds(const string& identifier) {
    lock_guard<mutex> lock(mutex_);
    auto it = buckets_.find(identifier);
    if (it == buckets_.end()) {         
        return 0;
    }
    TokenBucket bucket = it->second;
    auto now = chrono::steady_clock::now();       
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(     
        now - bucket.last_refill
    ).count() / 1000.0;      
    
    double new_tokens = bucket.tokens + (elapsed * refill_rate_);
    if (new_tokens >= 1.0) {
        return 0;
    }
    double tokens_needed = 1.0 - new_tokens;         
    int seconds_needed = static_cast<int>((tokens_needed / refill_rate_) + 1);      
    
    return max(1, seconds_needed);
}

void RateLimiter::Reset(const string& identifier) {
    lock_guard<mutex> lock(mutex_);
    buckets_.erase(identifier);
}

void RateLimiter::Clear() {
    lock_guard<mutex> lock(mutex_);
    buckets_.clear();
}

RateLimiter::TokenBucket& RateLimiter::GetBucket(const string& identifier) {
    auto it = buckets_.find(identifier);
    if (it == buckets_.end()) {
        auto now = chrono::steady_clock::now();
        buckets_[identifier] = TokenBucket{static_cast<double>(capacity_), now};
        return buckets_[identifier];
    }       
    return it->second;
}

void RateLimiter::RefillTokens(TokenBucket& bucket) {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(
        now - bucket.last_refill
    ).count() / 1000.0;
    
    double new_tokens = bucket.tokens + (elapsed * refill_rate_);
    bucket.tokens = min(static_cast<double>(capacity_), new_tokens);
    bucket.last_refill = now;
}