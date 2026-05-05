# Кеширование и ограничение запросов

## 1. Анализ производительности
Частые запросы:
- `GET /users/{login}` - частый доступ для авторизации и профиля.
- `GET /users/search?mask=X` - поиск сотрудников.
- `GET /patients/search?fio=X` - поиск пациента перед записью.
- `GET /patients/{id}/history` - история пациента.
- `POST /records` - создание записи с проверкой пациента.

В данных запросах наиболее медленные операции - это:
- PostgreSQL ILIKE-поиск в `users` и `patients`.
- `CONCAT_WS + ILIKE` для ФИО.
- MongoDB FindMany для истории пациента.
- SELECT patient перед INSERT в `records`.

Цели:
- p95 < 200ms для поиска.
- p95 < 50ms для доступа по ID.
- Пропускная способность ~ 1000 запросов/мин.

## 2. Кеширование
Кешируем соответствующие GET запросы:
- `GET /users/{login}`
- `GET /users/search?mask=X`
- `GET /patients/search?fio=X`
- `GET /patients/{id}/history`

Стратегия кеширования - **Cache-Aside**:
1. Проверить кеш.
2. Если есть — вернуть.
3. Если нет — сделать запрос в БД.
4. Записать в кеш и вернуть.

TTL для указанные кешериуемых данных:
- user по логину — 600 секунд.
- users search — 600 секунд.
- patients search — 900 секунд.
- patient history — 600 секунд.

Инвалидация:
- `POST /users` -> `users:search:*`
- `POST /patients` -> `patients:search:*`
- `POST /records` -> `patients:history:{id}`

Каждый ответ включает заголовок `X-Cache: HIT` или `X-Cache: MISS`.

## 3. Ограничение запросов
Ограничиваем write-пути:
- `POST /users` — 100/мин.
- `POST /patients` — 200/мин.
- `POST /records` — 100/мин.
- `POST /auth/login` — рекомендованно 50/мин.

Выбран алгоритм **Token Bucket** c параметрами:
- Ёмкость: 100 токенов
- Пополнение: 100 токенов / 60 сек
- Цена запроса: 1 токен

При превышении лимита:
- HTTP 429 Too Many Requests
- `X-RateLimit-Limit`
- `X-RateLimit-Remaining`
- `X-RateLimit-Reset`
- `Retry-After`

## 4. Влияние на производительность
Кеш снижает обращения к БД и ускоряет ответы при частом cache-hit.
Ограниечение запросов защищает от всплесков и уменьшает количество медленных write-запросов.

Возможные метрики для мониторинга производительности:
- `Cache Hit Rate = hits / (hits + misses) * 100%`
- Перцентили (обычно p50/p95/p99) для времени ответа
- Количество ограниченных (за счёт rate limiter) запросов
- Размер кеша в памяти

## 5. Реализация
Реализовано в:
- `api-service/src/cache.hpp`
- `api-service/src/cache.cpp`
- `api-service/src/rate_limiter.hpp`
- `api-service/src/rate_limiter.cpp`
- `api-service/src/handlers/user_handler.cpp`
- `api-service/src/handlers/patient_handler.cpp`
- `api-service/src/handlers/record_handler.cpp`

Добавлены соответствующие тесты в:
- `api-service/tests/tests.sh`