#pragma once
#include <string>
#include <memory>        
#include <vector>         
#include <userver/storages/mongo/pool.hpp>           
#include <userver/formats/bson.hpp>   

using namespace std;

class MongoClient {
public:
    MongoClient(userver::storages::mongo::PoolPtr pool);
    ~MongoClient() = default;

    string InsertOne(const string& collection_name, const string& json_doc);
    string FindOne(const string& collection_name, const string& filter_json);        
    vector<string> FindMany(const string& collection_name, const string& filter_json);       
    bool UpdateOne(const string& collection_name, const string& filter_json, const string& update_json);
    bool DeleteOne(const string& collection_name, const string& filter_json);  
    int64_t CountDocuments(const string& collection_name, const string& filter_json);         
    
    bool Ping() const noexcept;             
    string GetError() const noexcept;    
    bool IsConnected() const noexcept;

private:
    userver::storages::mongo::PoolPtr pool_;     
    mutable string last_error_;
};  
