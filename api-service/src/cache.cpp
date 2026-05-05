#include "cache.hpp"  

using namespace std;    
    
optional<string> ServiceCache::Get(const string& key) {       
    lock_guard<mutex> lock(mutex_);
    auto it = cache_.find(key);         
    if (it == cache_.end()) {   
        return nullopt;        
    }
         
    if (IsExpired(it->second.expires_at)) {       
        cache_.erase(it);    
        return nullopt;        
    }
    return it->second.value;    
}

void ServiceCache::Set(const string& key, const string& value, int ttl_seconds) {      
    lock_guard<mutex> lock(mutex_);    
    auto now = chrono::steady_clock::now();   
    auto expires_at = now + chrono::seconds(ttl_seconds);    
    cache_[key] = CacheEntry{value, expires_at};  
}

void ServiceCache::Invalidate(const string& key) {      
    lock_guard<mutex> lock(mutex_);    
    cache_.erase(key);          
}
     
void ServiceCache::InvalidatePattern(const string& pattern) {     
    lock_guard<mutex> lock(mutex_);     
    for (auto it = cache_.begin(); it != cache_.end(); ) {
        if (PatternMatch(it->first, pattern)) {
            it = cache_.erase(it);
        } else {
            it++;
        }
    }
}

void ServiceCache::Clear() {
    lock_guard<mutex> lock(mutex_);
    cache_.clear();   
}
          
size_t ServiceCache::Size() const {  
    lock_guard<mutex> lock(mutex_);     
    return cache_.size();        
}

bool ServiceCache::IsExpired(const chrono::steady_clock::time_point& expires_at) const {   
    auto now = chrono::steady_clock::now();        
    return now >= expires_at;            
}

bool ServiceCache::PatternMatch(const string& key, const string& pattern) {         
    size_t star_pos = pattern.find('*');
    if (star_pos == string::npos) 
        return key == pattern;
    if (star_pos == 0) {
        string suffix = pattern.substr(1);      
        if (suffix.empty()) return true;
        return key.find(suffix) != string::npos;
    }
    string prefix = pattern.substr(0, star_pos);        
    return key.find(prefix) == 0;
}
