#pragma once
#include <memory>
#include <vector>
#include <string>
#include <libpq-fe.h>

using namespace std;

struct PgResultDeleter {
    void operator()(PGresult* result) const noexcept {
        PQclear(result);
    }
};

struct PgConnectionDeleter {
    void operator()(PGconn* conn) const noexcept {
        if (conn) PQfinish(conn);
    }
};

using PgResultPtr = unique_ptr<PGresult, PgResultDeleter>;      
using PgConnectionPtr = unique_ptr<PGconn, PgConnectionDeleter>;

class PgClient {     
public:     
    PgClient();

    PgResultPtr ExecParams(const string& query,
                           int nParams,
                           const char* const* paramValues,
                           const int* paramLengths = nullptr,
                           const int* paramFormats = nullptr,        
                           const int* resultFormats = nullptr) const;

    string GetError() const noexcept;
    bool IsConnected() const noexcept;

private:
    PgConnectionPtr conn_;
};  
        