#!/bin/bash
SCRIPT_DIR="$(dirname "$0")"
source "$SCRIPT_DIR/utils.sh"
source "$SCRIPT_DIR/tests.sh"

main() {
    echo "Testing ..."
    
    if ! curl -s "$API/users/admin_user" > /dev/null 2>&1; then
        echo "API unavailable at $API"
        exit 1
    fi
    
    local passed=0
    local failed=0
    
    declare -a tests=(
        "test_login"
        "test_login_invalid"
        "test_create_user"
        "test_duplicate_user"
        "test_get_user"
        "test_user_not_found"
        "test_search_users"
        "test_register_patient"
        "test_search_patients"
        "test_get_patient"
        "test_create_record"
        "test_get_record"
        "test_patient_history"
        "test_protected_endpoints"
        "test_workflow"
        "test_data_format"
    )
    
    for test_func in "${tests[@]}"; do
        echo -n "Run $test_func ... "
        
        if $test_func > /dev/null 2>&1; then
            echo "ok"
            ((passed++))
        else
            echo "error"
            ((failed++))
        fi
    done
    
    echo "Passed: $passed"
    echo "Failed:   $failed"
}

main "$@"
