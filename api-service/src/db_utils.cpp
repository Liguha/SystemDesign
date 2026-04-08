#include <string>
#include <stdexcept>
#include <cstdlib>
#include "db_utils.hpp"

using namespace std;      

static const char* DefaultDatabaseUrl() {
    return "host=postgres port=5432 dbname=medical user=postgres password=postgres";
}

PgClient::PgClient() {
    const char* conninfo = getenv("DATABASE_URL");
    if (!conninfo || *conninfo == '\0') {
        conninfo = DefaultDatabaseUrl();
    }
    conn_.reset(PQconnectdb(conninfo));
    if (!conn_ || PQstatus(conn_.get()) != CONNECTION_OK) {   
        auto error = conn_ ? PQerrorMessage(conn_.get()) : "failed to allocate PGconn";
        throw runtime_error(string("Postgres connection failed: ") + error);  
    }
}

PgResultPtr PgClient::ExecParams(const string& query,
                                 int nParams,
                                 const char* const* paramValues,
                                 const int* paramLengths,
                                 const int* paramFormats,   
                                 const int* resultFormats) const {
    PGresult* result = PQexecParams(conn_.get(),
                                    query.c_str(),
                                    nParams,
                                    nullptr,
                                    paramValues,    
                                    paramLengths,
                                    paramFormats,
                                    0);
    return PgResultPtr(result);
}

string PgClient::GetError() const noexcept {
    return conn_ ? PQerrorMessage(conn_.get()) : "no connection";  
}

bool PgClient::IsConnected() const noexcept {
    return conn_ && PQstatus(conn_.get()) == CONNECTION_OK;
}     