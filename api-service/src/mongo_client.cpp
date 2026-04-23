#include <stdexcept>    
#include "mongo_client.hpp"
#include <userver/formats/bson.hpp>      
#include <userver/formats/json.hpp>  
#include <userver/formats/bson/serialize.hpp>            

using namespace std;

MongoClient::MongoClient(userver::storages::mongo::PoolPtr pool) : pool_(std::move(pool)) {}

bool MongoClient::Ping() const noexcept { return !!pool_; }
bool MongoClient::IsConnected() const noexcept { return !!pool_; }       
string MongoClient::GetError() const noexcept { return last_error_; }         

string MongoClient::InsertOne(const string& collection_name, const string& json_doc) {
    try {
        auto collection = pool_->GetCollection(collection_name);     
        auto doc = userver::formats::bson::FromJsonString(json_doc);         
        collection.InsertOne(doc);
        if (doc.HasMember("_id")) {
            return userver::formats::bson::ToCanonicalJsonString(doc["_id"]);
        }
        return "success";
    } catch (const exception& ex) {
        last_error_ = ex.what();
        throw runtime_error(string("InsertOne failed: ") + ex.what());
    }
}

string MongoClient::FindOne(const string& collection_name, const string& filter_json) {
    try {
        auto collection = pool_->GetCollection(collection_name);
        auto filter = userver::formats::bson::FromJsonString(filter_json);
        auto doc = collection.FindOne(filter);  
        
        if (!doc) return "";                 
        return userver::formats::bson::ToCanonicalJsonString(*doc);
    } catch (const exception& ex) {
        last_error_ = ex.what();
        return "";     
    }
}

vector<string> MongoClient::FindMany(const string& collection_name, const string& filter_json) {
    vector<string> results;         
    try {
        auto collection = pool_->GetCollection(collection_name);      
        auto filter = userver::formats::bson::FromJsonString(filter_json);  
        auto cursor = collection.Find(filter);       
        
        for (const auto& doc : cursor) {            
            results.push_back(userver::formats::bson::ToCanonicalJsonString(doc));
        }
    } catch (const exception& ex) {
        last_error_ = ex.what();
    }
    return results;
}

bool MongoClient::UpdateOne(const string& collection_name, const string& filter_json, const string& update_json) {
    try {
        auto collection = pool_->GetCollection(collection_name);        
        auto filter = userver::formats::bson::FromJsonString(filter_json);           
        auto update = userver::formats::bson::FromJsonString(update_json);     
        
        auto result = collection.UpdateOne(filter, update);
        return result.ModifiedCount() > 0 || result.MatchedCount() > 0;
    } catch (const exception& ex) {
        last_error_ = ex.what();
        return false;
    }
}

bool MongoClient::DeleteOne(const string& collection_name, const string& filter_json) {
    try {
        auto collection = pool_->GetCollection(collection_name);
        auto filter = userver::formats::bson::FromJsonString(filter_json);
        
        auto result = collection.DeleteOne(filter);
        return result.DeletedCount() > 0;
    } catch (const exception& ex) {
        last_error_ = ex.what();
        return false;
    }
}

int64_t MongoClient::CountDocuments(const string& collection_name, const string& filter_json) {
    try {
        auto collection = pool_->GetCollection(collection_name);
        auto filter = userver::formats::bson::FromJsonString(filter_json);               
        return collection.Count(filter);
    } catch (const exception& ex) {
        last_error_ = ex.what();             
        return 0;
    }       
}        