CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE TABLE users (
    id TEXT PRIMARY KEY,
    login TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    role TEXT NOT NULL CHECK (role IN ('admin', 'doctor')),
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TABLE patients (
    id TEXT PRIMARY KEY,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    patronymic TEXT,
    birth_date DATE NOT NULL,
    phone TEXT,
    registered_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_users_login_text ON users (login);
CREATE INDEX idx_users_login_trgm ON users USING gin (lower(login) gin_trgm_ops);
CREATE INDEX idx_users_name_trgm ON users USING gin ((lower(first_name) || ' ' || lower(last_name)) gin_trgm_ops);
CREATE INDEX idx_patients_name_trgm ON patients USING gin ((lower(first_name) || ' ' || lower(last_name)) gin_trgm_ops);