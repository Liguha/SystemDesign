# Event-Driven архитектура для Medical Records
## 1. Анализ событий и команд
### События
- `user.created` — создан пользователь.
- `patient.registered` — зарегистрирован пациент.
- `record.created` — создана медицинская запись.

### Команды
- `CreateUser` (`POST /users`)
- `RegisterPatient` (`POST /patients`)
- `CreateMedicalRecord` (`POST /records`)

## 2. Компоненты архитектуры
### Производители событий
- `UserHandler` публикует `user.created`.
- `PatientHandler` публикует `patient.registered`.
- `RecordHandler` публикует `record.created`.

### Потребители событий
- `EventLogConsumer` принимает события и сохраняет их в MongoDB коллекцию `event_logs`.

### Брокер сообщений
- Exchange: `medical.events`
- Тип exchange: `topic`
- Очередь: `medical.events.log`
- Binding: `event.#`

## 3. Поток событий
1. Клиент отправляет команду через HTTP API.
2. API Gateway выполняет запись в PostgreSQL или MongoDB.
3. После успешной операции формируется событие.
4. `EventPublisher` публикует событие в RabbitMQ.
5. `EventLogConsumer` получает сообщение и сохраняет его в MongoDB.

## 4. CQRS
### Разделение write и read
- Write-модель: `POST /users`, `POST /patients`, `POST /records`.
- Read-модель: `GET /users/{login}`, `GET /users/search`, `GET /patients/search`, `GET /records/{code}`, `GET /patients/{id}/history`, `GET /events`.

## 5. Реализация
- RabbitMQ запускается через `rabbitmq/docker-compose.yml`.
- В API реализованы `EventPublisher` и `EventLogConsumer`.
- События публикуются после успешных операций записи.
- `EventLogConsumer` сохраняет event log в MongoDB.