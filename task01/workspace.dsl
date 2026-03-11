workspace "Medical Records System" "Система управления медицинскими записями" {

    model {
        doctor = person "Врач" "Специалист по клинической работе."
        admin = person "Администратор" "Специалист по кадрам и регистрации."
        
        emailSystem = softwareSystem "Email Service" "Внешняя система уведомлений." "External"

        medicalSystem = softwareSystem "Medical Management System" {
            gateway = container "API Gateway" "Высокопроизводительный роутинг и Auth." "Go/Gin"
            
            identityService = container "Identity Service" "Безопасное управление доступом." "Java/Spring Boot"
            patientService = container "Patient Service" "Гибкий реестр пациентов." "Python/FastAPI"
            recordService = container "Medical Record Service" "Обработка сложных мед. данных." "C#/.NET"
            
            userDb = container "User Database" "Реляционное хранилище аккаунтов." "PostgreSQL" "Database"
            patientDb = container "Patient Database" "Реляционное хранилище анкет." "PostgreSQL" "Database"
            recordDb = container "Record Database" "NoSQL хранилище истории болезни." "MongoDB" "Database"
        }

        # Связи с указанием протоколов
        doctor -> gateway "REST API" "HTTPS/JSON"
        admin -> gateway "REST API" "HTTPS/JSON"
        
        gateway -> identityService "Авторизация" "gRPC"
        gateway -> patientService "Поиск пациентов" "gRPC"
        gateway -> recordService "Работа с записями" "gRPC"
        
        identityService -> userDb "JDBC"
        patientService -> patientDb "SQL/TCP"
        recordService -> recordDb "Mongo Wire Protocol"
        
        recordService -> emailSystem "Уведомления" "SMTP"
    }

    views {
        systemContext medicalSystem "SystemContext" {
            include *
            autoLayout lr
        }

        container medicalSystem "Containers" {
            include *
            autoLayout lr
        }

        dynamic medicalSystem "CreateMedicalRecord" "Сценарий: Создание медицинской записи для пациента" {
            doctor -> gateway "POST /records (PatientID, Data)"
            gateway -> identityService "Проверка прав врача"
            gateway -> patientService "Проверка существования пациента"
            gateway -> recordService "Сохранение записи"
            recordService -> recordDb "Вставка документа"
            recordService -> gateway "201 Created (RecordCode)"
            gateway -> doctor "Отображение кода записи"
        }

        styles {
            element "Element" {
                shape RoundedBox
                background #1168bd
                color #ffffff
            }
            element "External" {
                background #999999
            }
            element "Database" {
                shape Cylinder
            }
        }
    }
}