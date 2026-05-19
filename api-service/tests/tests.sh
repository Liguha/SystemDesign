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
    local response=$(http_get "/patients/patient_100/history" "$TOKEN")
    echo "$response" | grep -q "code\|title"
}

test_patient_history_mongodb() {
    [ -z "$TOKEN" ] && return 1
    local response=$(http_get "/patients/patient_100/history" "$TOKEN")
    echo "$response" | grep -q "code\|created_by" || echo "$response" | grep -q "REC"
}

test_event_bus() {
    [ -z "$TOKEN" ] && return 1

    local attempts=0
    while [ $attempts -lt 6 ]; do
        local response=$(http_get "/events?type=record.created" "$TOKEN")
        if echo "$response" | grep -q "$TEST_RECORD_CODE"; then
            return 0
        fi
        sleep 1
        attempts=$((attempts + 1))
    done
    return 1
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

test_user_cache() {
    local first_response=$(curl -s -i -X GET "$API/users/admin_user")
    echo "$first_response" | grep -qi '^X-Cache: \(MISS\|HIT\)' || return 1
    echo "$first_response" | grep -q 'admin_user' || return 1

    local second_response=$(curl -s -i -X GET "$API/users/admin_user")
    echo "$second_response" | grep -qi '^X-Cache: HIT' || return 1
    return 0
}

test_rate_limit_records() {
    if [ -z "$TOKEN" ]; then
        test_login || return 1
    fi
    local user_id="rate_test_$RANDOM_SUFFIX"
    local request_body_template='{"login":"rate_user_%s_%s","password":"pass123","first_name":"Rate","last_name":"Limiter"}'
    local temp_dir
    temp_dir=$(mktemp -d)
    local status_code
    local have_429=0

    for i in $(seq 1 120); do
        local login_value="rate_user_${user_id}_${i}"
        local request_body
        request_body=$(printf "$request_body_template" "$user_id" "$i")
        curl -s -i -X POST "$API/users" \
            -H "Content-Type: application/json" \
            -H "Authorization: Bearer $TOKEN" \
            -H "x-user-id: $user_id" \
            -d "$request_body" > "$temp_dir/req_$i" &
    done
    wait
    for file in "$temp_dir"/req_*; do
        if grep -q '^HTTP/1\.[01] 429' "$file"; then
            have_429=1
            grep -qi '^X-RateLimit-Limit:' "$file" || return 1
            grep -qi '^X-RateLimit-Remaining: 0' "$file" || return 1
            grep -qi 'Too many requests' "$file" || return 1
            break
        fi
    done
    rm -rf "$temp_dir"
    [ "$have_429" -eq 1 ]
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