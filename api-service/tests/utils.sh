#!/bin/bash
API="${API:-http://localhost:8080}"

http_get() {
    local url=$1
    local token=$2
    if [ -n "$token" ]; then
        curl -s -X GET "$API$url" -H "Authorization: Bearer $token"
    else
        curl -s -X GET "$API$url"
    fi
}

http_post() {
    local url=$1
    local data=$2
    local token=$3
    
    local cmd="curl -s -X POST \"$API$url\" -H \"Content-Type: application/json\" -d '$data'"
    if [ -n "$token" ]; then
        cmd="$cmd -H \"Authorization: Bearer $token\""
    fi
    eval "$cmd"
}

http_delete() {
    local url=$1
    local token=$2
    curl -s -X DELETE "$API$url" -H "Authorization: Bearer $token"
}

extract_token() {
    echo "$1" | grep -o '"token":"[^"]*' | cut -d'"' -f4
}

extract_id() {
    echo "$1" | grep -o '"id":"[^"]*' | head -1 | cut -d'"' -f4
}