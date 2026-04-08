# Схема и данные базы данных
## Файлы
- `schema.sql` — SQL скрипт создания схемы БД (таблицы, индексы, ограничения)
- `data.sql` — SQL скрипт вставки тестовых данных
- `queries.sql` — SQL запросы для всех операций API
- `optimization.md` — Анализ индексов и оптимизация запросов

## Схема
### Таблицы
- **users**: Сотрудники (врачи, администраторы)
  - id (TEXT PRIMARY KEY)
  - login (TEXT UNIQUE)
  - password (TEXT)
  - first_name, last_name (TEXT)
  - role (TEXT CHECK IN ('admin', 'doctor'))
  - created_at (TIMESTAMPTZ)

- **patients**: Зарегистрированные пациенты
  - id (TEXT PRIMARY KEY)
  - first_name, last_name, patronymic (TEXT)
  - birth_date (DATE)
  - phone (TEXT)
  - registered_at (TIMESTAMPTZ)

- **medical_records**: Медицинские записи
  - code (TEXT PRIMARY KEY)
  - patient_id (TEXT FK -> patients)
  - created_by (TEXT FK -> users.login)
  - title, description (TEXT)
  - created_at (TIMESTAMPTZ)

### Индексы
- GIN trigram индексы для полнотекстового поиска по именам и логинам
- B-tree индексы на внешние ключи и patient_id

## Операции
Примеры всех операций в файле `queries.sql`.

## Оптимизация
Обоснование индексов и EXPLAIN планы смотрите в `optimization.md`.