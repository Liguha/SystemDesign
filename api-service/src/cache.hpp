#pragma once
#include <string>           
#include <mutex>     
#include <optional>   
#include <chrono> 
#include <map>  

using namespace std;

class ServiceCache {
public:
    ServiceCache() = default;        
    ~ServiceCache() = default;       

    optional<string> Get(const string& key);             
    void Set(const string& key, const string& value, int ttl_seconds);           
    void Invalidate(const string& key);
    void InvalidatePattern(const string& pattern);          
    void Clear();
    size_t Size() const;      

private:
    struct CacheEntry {         
        string value;       
        chrono::steady_clock::time_point expires_at;             
    };      
    mutable mutex mutex_;  
    map<string, CacheEntry> cache_;      
    bool IsExpired(const chrono::steady_clock::time_point& expires_at) const;
    static bool PatternMatch(const string& key, const string& pattern);           
};  