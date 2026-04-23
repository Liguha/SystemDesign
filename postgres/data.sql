INSERT INTO users (id, login, password, first_name, last_name, role, created_at) VALUES
('user_admin_user', 'admin_user', 'admin123', 'Admin', 'System', 'admin', NOW()),
('user_doctor_user', 'doctor_user', 'doc123', 'Ivan', 'Petrov', 'doctor', NOW()),
('user_nina_smirnova', 'nina_smirnova', 'nina123', 'Nina', 'Smirnova', 'doctor', NOW()),
('user_oleg_kuznetsov', 'oleg_kuznetsov', 'oleg123', 'Oleg', 'Kuznetsov', 'doctor', NOW()),
('user_anna_popova', 'anna_popova', 'anna123', 'Anna', 'Popova', 'admin', NOW()),
('user_sergey_ivanov', 'sergey_ivanov', 'sergey123', 'Sergey', 'Ivanov', 'doctor', NOW()),
('user_maria_fedorova', 'maria_fedorova', 'maria123', 'Maria', 'Fedorova', 'doctor', NOW()),
('user_pavel_sokolov', 'pavel_sokolov', 'pavel123', 'Pavel', 'Sokolov', 'doctor', NOW()),
('user_elena_kovaleva', 'elena_kovaleva', 'elena123', 'Elena', 'Kovaleva', 'admin', NOW()),
('user_ivan_orlov', 'ivan_orlov', 'ivan123', 'Ivan', 'Orlov', 'doctor', NOW());

INSERT INTO patients (id, first_name, last_name, patronymic, birth_date, phone, registered_at) VALUES
('patient_100', 'Peter', 'Sidorov', 'Ivanovich', '1980-05-15', '+1-999-123-45-67', NOW()),
('patient_101', 'Olga', 'Ivanova', 'Sergeevna', '1990-07-21', '+7-912-555-01-01', NOW()),
('patient_102', 'Alexey', 'Smirnov', 'Petrovich', '1975-03-12', '+7-913-222-02-02', NOW()),
('patient_103', 'Maria', 'Kuznetsova', 'Alexeevna', '1985-01-30', '+7-914-333-03-03', NOW()),
('patient_104', 'Dmitry', 'Popov', 'Ivanovich', '1992-11-05', '+7-915-444-04-04', NOW()),
('patient_105', 'Elena', 'Fedorova', 'Pavlovna', '1978-09-17', '+7-916-555-05-05', NOW()),
('patient_106', 'Sergey', 'Sokolov', 'Nikolaevich', '1983-02-27', '+7-917-666-06-06', NOW()),
('patient_107', 'Anna', 'Kovaleva', 'Dmitrievna', '1995-12-12', '+7-918-777-07-07', NOW()),
('patient_108', 'Ivan', 'Orlov', 'Alekseevich', '1988-08-08', '+7-919-888-08-08', NOW()),
('patient_109', 'Nina', 'Novikova', 'Vladimirovna', '1991-04-04', '+7-920-999-09-09', NOW());