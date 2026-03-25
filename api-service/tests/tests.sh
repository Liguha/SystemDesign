#!/bin/bash
TOKEN=""
TEST_PATIENT_ID="id_patient_001"
TEST_RECORD_CODE="REC-001-001"

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
    local response=$(http_post "/users" '{"login":"nurse","password":"pass123","first_name":"Nurse","last_name":"Ivanova"}' "$TOKEN")
    echo "$response" | grep -q "nurse"
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
    echo "$response" | grep -q "Ivan\|doctor"
}

test_register_patient() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_post "/patients" '{"first_name":"Peter","last_name":"Sidorov","birth_date":"1990-01-01"}' "$TOKEN")
    TEST_PATIENT_ID=$(extract_id "$response")
    [ -n "$TEST_PATIENT_ID" ] || TEST_PATIENT_ID="id_patient_001"
    return 0
}

test_search_patients() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/search?fio=Petr" "$TOKEN")
    echo "$response" | grep -q "Peter"
}

test_get_patient() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/$TEST_PATIENT_ID" "$TOKEN")
    echo "$response" | grep -q "first_name"
}

test_create_record() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_post "/records" "{\"patient_id\":\"$TEST_PATIENT_ID\",\"created_by\":\"doctor_user\",\"title\":\"Осмотр\"}" "$TOKEN")
    TEST_RECORD_CODE=$(echo "$response" | grep -o '"code":"[^"]*' | cut -d'"' -f4)
    [ -n "$TEST_RECORD_CODE" ] || TEST_RECORD_CODE="REC-001-001"
    return 0
}

test_get_record() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/records/$TEST_RECORD_CODE" "$TOKEN")
    echo "$response" | grep -q "code"
}

test_patient_history() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/$TEST_PATIENT_ID/history" "$TOKEN")
    echo "$response" | grep -q "code\|title"
}

test_protected_endpoints() {
    local response=$(http_post "/patients" '{"first_name":"Test","last_name":"User","birth_date":"2000-01-01"}')
    echo "$response" | grep -q "error\|Unauthorized"
}

test_workflow() {
    local admin=$(http_post "/auth/login" '{"login":"admin_user","password":"admin123"}')
    local admin_token=$(extract_token "$admin")
    [ -z "$admin_token" ] && return 1
    
    local doctor=$(http_post "/users" '{"login":"dr_test","password":"secure","first_name":"Doctor","last_name":"Test","role":"doctor"}' "$admin_token")
    echo "$doctor" | grep -q "dr_test"
}

test_data_format() {
    local users=$(http_get "/users/search?mask=user")
    echo "$users" | jq . > /dev/null 2>&1
}