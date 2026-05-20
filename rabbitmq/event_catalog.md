# Каталог событий
## Общая информация
Все события проходят через RabbitMQ exchange `medical.events` и доставляются в очередь `medical.events.log`.
Exchange имеет тип `topic`, что позволяет фильтровать события по шаблону `event.#`.

## Структура сообщения
```json
{
  "event_type": "record.created",
  "event_id": "evt_...",
  "timestamp": "2026-05-19T12:00:00.000Z",
  "source": "api-service",
  "version": "1.0",
  "payload": { ... }
}
```

## `user.created`
- Название: `user.created`
- Производитель: API Gateway (`UserHandler`)
- Потребитель: `EventLogConsumer`
- Гарантия доставки: `at-least-once`
- Routing key: `event.user.created`
- Payload:
  - `id` - идентификатор пользователя
  - `login` - логин
  - `first_name` - имя
  - `last_name` - фамилия
  - `role` - роль пользователя
  - `created_at` - метка времени создания

## `patient.registered`
- Название: `patient.registered`
- Производитель: API Gateway (`PatientHandler`)
- Потребитель: `EventLogConsumer`
- Гарантия доставки: `at-least-once`
- Routing key: `event.patient.registered`
- Payload:
  - `id` - идентификатор пациента
  - `first_name` - имя
  - `last_name` - фамилия
  - `patronymic` - отчество
  - `birth_date` - дата рождения
  - `phone` - телефон
  - `registered_at` - метка времени регистрации

## `record.created`
- Название: `record.created`
- Производитель: API Gateway (`RecordHandler`)
- Потребитель: `EventLogConsumer`
- Гарантия доставки: `at-least-once`
- Routing key: `event.record.created`
- Payload:
  - `code` - уникальный код записи
  - `patient_id` - идентификатор пациента
  - `created_by` - логин врача, создавшего запись
  - `title` - заголовок записи
  - `created_at` - время создания записи

## Потребители
- `EventLogConsumer` сохраняет события в MongoDB коллекцию `event_logs`.

## RabbitMQ конфигурация
- Exchange: `medical.events`
- Тип exchange: `topic`
- Routing key: `event.<event_type>`
- Queue: `medical.events.log`
- Binding key: `event.#`