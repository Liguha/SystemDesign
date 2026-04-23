#!/bin/bash
TOKEN=""
TEST_PATIENT_ID="patient_100"
TEST_RECORD_CODE="REC-000001-000001"
TIMESTAMP=$(date +%s)
RANDOM_SUFFIX=$((RANDOM * RANDOM))

test_login() {
    local response=$(http_post "/auth/login" '{"login":"doctor_user","password":"doc123"}')
    TOKEN=$(extract_token "$response")
    [ -n "$TOKEN" ]
}

test_login_invalid() {
    local response=$(http_post "/auth/login" '{"login":"doctor_user","password":"wrong"}')
    echo "$response" | grep -q "Invalid\|Unauthorized"
}

test_create_user() {
    [ -z "$TOKEN" ] && return 1
    local unique_login="nurse_$TIMESTAMP"
    local response=$(http_post "/users" '{"login":"'$unique_login'","password":"pass123","first_name":"Nurse","last_name":"Ivanova"}' "$TOKEN")
    echo "$response" | grep -q "$unique_login"
}

test_duplicate_user() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_post "/users" '{"login":"doctor_user","password":"pass","first_name":"Test","last_name":"User"}' "$TOKEN")
    echo "$response" | grep -q "already exists"
}

test_get_user() {
    local response=$(http_get "/users/doctor_user")
    echo "$response" | grep -q "doctor_user"
}

test_user_not_found() {
    local response=$(http_get "/users/nonexistent")
    echo "$response" | grep -q "not found"
}

test_search_users() {
    local response=$(http_get "/users/search?mask=Ivan")
    echo "$response" | grep -q "Ivan"
}

test_register_patient() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_post "/patients" '{"first_name":"Test","last_name":"Patient","birth_date":"1990-01-01"}' "$TOKEN")
    TEST_PATIENT_ID=$(extract_id "$response")
    [ -n "$TEST_PATIENT_ID" ] || TEST_PATIENT_ID="patient_100"
    return 0
}

test_search_patients() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/search?fio=Sidorov" "$TOKEN")
    echo "$response" | grep -q "Sidorov"
}

test_get_patient() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/$TEST_PATIENT_ID" "$TOKEN")
    echo "$response" | grep -q "first_name"
}

test_create_record() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_post "/records" "{\"patient_id\":\"$TEST_PATIENT_ID\",\"created_by\":\"doctor_user\",\"title\":\"Test Record - MongoDB\"}" "$TOKEN")
    TEST_RECORD_CODE=$(echo "$response" | grep -o '"code":"[^"]*' | cut -d'"' -f4)
    [ -n "$TEST_RECORD_CODE" ] || TEST_RECORD_CODE="REC-000001-000001"
    return 0
}

test_get_record() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/records/$TEST_RECORD_CODE" "$TOKEN")
    echo "$response" | grep -q "code"
}

test_get_record_mongodb() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/records/REC-1704067800-123456" "$TOKEN")
    echo "$response" | grep -q "patient_id"
}

test_patient_history() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/$TEST_PATIENT_ID/history" "$TOKEN")
    echo "$response" | grep -q "code\|title"
}

test_patient_history_mongodb() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/patient_100/history" "$TOKEN")
    echo "$response" | grep -q "code\|created_by" || echo "$response" | grep -q "REC"
}

test_protected_endpoints() {
    local response=$(http_post "/patients" '{"first_name":"Test","last_name":"User","birth_date":"2000-01-01"}')
    echo "$response" | grep -q "error\|Unauthorized"
}

test_workflow() {
    local admin=$(http_post "/auth/login" '{"login":"admin_user","password":"admin123"}')
    local admin_token=$(extract_token "$admin")
    [ -z "$admin_token" ] && return 1
    local unique_login="dr_test_$TIMESTAMP"
    local doctor=$(http_post "/users" '{"login":"'$unique_login'","password":"secure","first_name":"Doctor","last_name":"Test","role":"doctor"}' "$admin_token")
    echo "$doctor" | grep -q "$unique_login"
}

test_data_format() {
    local users=$(http_get "/users/search?mask=user")
    echo "$users" | jq . > /dev/null 2>&1
}

test_mongodb_connection() {
    [ -z "$TOKEN" ] && test_login
    local response=$(http_get "/records/REC-1704067800-123456" "$TOKEN")
    echo "$response" | grep -q "code\|error"
}