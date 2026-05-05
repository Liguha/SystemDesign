# Оптимизация производительности

В ходе оптимизации было добавлено:
- In-memory кеширование с TTL для `GET /users/{login}`, `GET /users/search` и `GET /patients/search`.
- Кеширование истории пациента для `GET /patients/{id}/history`.
- Активная инвалидация кеша при `POST /users`, `POST /patients` и `POST /records`.
- Rate limiter (алгоритм token bucket) для `POST /users`, `POST /patients` и `POST /records`.
- Заголовки `X-Cache` и `X-RateLimit-*` для мониторинга.

Реализация доступна в исходных файлах:
- `api-service/src/cache.hpp`, `api-service/src/cache.cpp`
- `api-service/src/rate_limiter.hpp`, `api-service/src/rate_limiter.cpp`
- `api-service/src/handlers/user_handler.cpp`
- `api-service/src/handlers/patient_handler.cpp`
- `api-service/src/handlers/record_handler.cpp`

Помимо этого, в `api-service/tests/tests.sh` были добавлены соответствующие тесты.