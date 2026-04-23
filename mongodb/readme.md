## Структура файлов

- `schema_design.md` - Описание проектирования документной модели MongoDB
- `data.js` - Тестовые документы для medical_records
- `queries.md` - MongoDB запросы
- `validation.js` - Скрипт валидации схем с использованием $jsonSchema

## Развертывание и подключение (Docker)

```bash
#  из корневой папки
docker compose up -d mongo
```

```bash
docker compose exec mongo mongosh
```

Оболочка MongoSH позволяет исполнять отдельные запросы в интерактивном режиме.