-- Create new user, used in: user_handler
INSERT INTO users (id, login, password, first_name, last_name, role, created_at)
VALUES ('user_julia_ivanova', 'julia_ivanova', 'julia123', 'Julia', 'Ivanova', 'doctor', NOW());

-- Find user by login, used in: auth_handler, user_handler
SELECT id, login, first_name, last_name, role, EXTRACT(EPOCH FROM created_at)::BIGINT AS created_at
FROM users
WHERE login = 'doctor_user';

-- Search users by name/login mask, used in: user_handler
SELECT id, login, first_name, last_name, role, EXTRACT(EPOCH FROM created_at)::BIGINT AS created_at
FROM users
WHERE login ILIKE '%petrov%'
   OR first_name ILIKE '%petrov%'
   OR last_name ILIKE '%petrov%'
   OR CONCAT_WS(' ', first_name, last_name) ILIKE '%petrov%'
ORDER BY login;

-- Register patient, used in: patient_handler 
INSERT INTO patients (id, first_name, last_name, patronymic, birth_date, phone, registered_at)
VALUES ('patient_110', 'Petr', 'Kanev', 'Anatolyevich', '1987-06-06', '+7-921-111-10-10', NOW());

-- Search patient by name, used in: patient_handler
SELECT id, first_name, last_name, patronymic, birth_date, phone, EXTRACT(EPOCH FROM registered_at)::BIGINT AS registered_at
FROM patients
WHERE CONCAT_WS(' ', first_name, last_name) ILIKE '%ivanov%'
   OR CONCAT_WS(' ', last_name, first_name) ILIKE '%ivanov%'
ORDER BY last_name, first_name;