# RabbitMQ для Medical API
- `docker-compose.yml` — локальный Docker Compose для RabbitMQ.
- `event_driven_design.md` — описание архитектуры Event-Driven.
- `event_catalog.md` — каталог событий с payload и маршрутизацией.

## Обзор
RabbitMQ используется как брокер сообщений для доставки событий из API-сервиса в event log.
События публикуются из API Gateway и потребляются компонентом `EventLogConsumer`.

## Интеграция с API
В `api-service/config/config.yaml` настроен компонент `rabbitmq`:
- `secdist_alias: rabbitmq`
- `min_pool_size`, `max_pool_size`
- `use_secure_connection: false`

API-сервис подключает:
- `EventPublisher` — публикует события в exchange `medical.events`.
- `EventLogConsumer` — читает события из очереди `medical.events.log`.

## Тестирование
- Проверка выполняется через `api-service/tests/run.sh` (тест `test_event_bus`).
- Тест проверяет создание записи, публикацию события и его доступность через `/events`.