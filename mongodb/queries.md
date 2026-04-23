## 1. CREATE

### 1.1 Создание новой медицинской записи
```javascript
db.medical_records.insertOne({
  code: "REC-1704931800-654321",
  patient_id: "patient_110",
  created_by: "doctor_user",
  title: "Neurological examination",
  description: "Patient presented with occasional headaches. Neurological tests passed.",
  record_type: "checkup",
  severity: "low",
  tags: ["neurology", "headache", "2024"],
  attachments: [],
  medications: [
    {
      name: "Paracetamol",
      dosage: "500mg",
      frequency: "as needed",
      duration: 14
    }
  ],
  vital_signs: {
    blood_pressure: "120/80",
    heart_rate: 72,
    temperature: 36.6,
    respiratory_rate: 16
  },
  created_at: new Date(),
  updated_at: new Date(),
  is_deleted: false
});
```

### 1.2 Вставка нескольких записей одновременно
```javascript
db.medical_records.insertMany([
  {
    code: "REC-1704931800-654322",
    patient_id: "patient_111",
    created_by: "nina_smirnova",
    title: "Lab work",
    description: "Blood work completed. Results pending.",
    record_type: "test",
    severity: "low",
    tags: ["lab", "blood_work"],
    attachments: [],
    medications: [],
    vital_signs: {},
    created_at: new Date(),
    updated_at: new Date(),
    is_deleted: false
  },
  {
    code: "REC-1704931800-654323",
    patient_id: "patient_112",
    created_by: "oleg_kuznetsov",
    title: "Surgery follow-up",
    description: "Post-operative check. Healing normally.",
    record_type: "follow_up",
    severity: "medium",
    tags: ["surgery", "post_op"],
    attachments: [],
    medications: [],
    vital_signs: {},
    created_at: new Date(),
    updated_at: new Date(),
    is_deleted: false
  }
]);
```
---

## 2. READ

### 2.1 Получить запись по коду
```javascript
db.medical_records.findOne({ code: "REC-1704067800-123456" });
```

### 2.2 Получить все записи пациента
```javascript
db.medical_records.find({ 
  patient_id: "patient_100",
  is_deleted: false 
}).sort({ created_at: -1 });
```

### 2.3 Поиск записей врача
```javascript
db.medical_records.find({ 
  created_by: "doctor_user",
  is_deleted: false 
}).sort({ created_at: -1 });
```

### 2.4 Поиск записей по типу
```javascript
db.medical_records.find({ 
  record_type: "checkup",
  is_deleted: false 
});

db.medical_records.find({ 
  record_type: { $in: ["consultation", "follow_up"] },
  is_deleted: false 
});
```

### 2.5 Поиск по тегам
```javascript
db.medical_records.find({ 
  tags: { $in: ["vaccination", "preventive"] },
  is_deleted: false 
});

db.medical_records.find({ 
  tags: { $all: ["routine", "2024"] },
  is_deleted: false 
});
```

### 2.6 Поиск по серьезности
```javascript
db.medical_records.find({ 
  severity: { $eq: "critical" },
  is_deleted: false 
});

db.medical_records.find({ 
  severity: { $in: ["high", "critical"] },
  is_deleted: false 
});
```

### 2.7 Поиск по дате
```javascript
db.medical_records.find({
  created_at: { 
    $gte: new Date(new Date().getTime() - 7 * 24 * 60 * 60 * 1000)
  },
  is_deleted: false
});

db.medical_records.find({
  created_at: { 
    $lt: new Date(new Date().getTime() - 365 * 24 * 60 * 60 * 1000)
  },
  is_deleted: false
});
```

### 2.8 Поиск по вложенным полям
```javascript
db.medical_records.find({
  "vital_signs.heart_rate": { $gt: 80 },
  is_deleted: false
});

db.medical_records.find({
  "vital_signs.blood_pressure": "120/80",
  is_deleted: false
});

db.medical_records.find({
  "vital_signs.temperature": { $gt: 37.0 },
  is_deleted: false
});
```

### 2.9 Поиск по лекарствам
```javascript
db.medical_records.find({
  "medications.name": "Aspirin",
  is_deleted: false
});

db.medical_records.find({
  medications: { 
    $elemMatch: { 
      name: "Aspirin",
      dosage: { $gte: "500mg" }
    }
  },
  is_deleted: false
});
```

### 2.10 Подсчет документов
```javascript
db.medical_records.countDocuments({ 
  patient_id: "patient_100",
  is_deleted: false 
});

db.medical_records.countDocuments({ 
  record_type: "checkup",
  is_deleted: false 
});
```
---

## 3. UPDATE

### 3.1 Добавить запись к пациенту
```javascript
db.medical_records.insertOne({
  code: "REC-1704931800-654324",
  patient_id: "patient_100",
  created_by: "doctor_user",
  title: "New consultation",
  description: "Follow-up consultation",
  record_type: "consultation",
  severity: "low",
  tags: ["follow_up"],
  attachments: [],
  medications: [],
  vital_signs: {},
  created_at: new Date(),
  updated_at: new Date(),
  is_deleted: false
});
```

### 3.2 Обновить описание записи
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $set: {
      description: "Updated description with more details",
      updated_at: new Date()
    }
  }
);
```

### 3.3 Обновить несколько полей одновременно
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $set: {
      title: "Updated title",
      description: "Updated description",
      severity: "medium",
      updated_at: new Date()
    }
  }
);
```

### 3.4 Добавить лекарство к записи
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $push: {
      medications: {
        name: "Ibuprofen",
        dosage: "400mg",
        frequency: "twice daily",
        duration: 10
      }
    },
    $set: { updated_at: new Date() }
  }
);
```

### 3.5 Добавить вложенный файл
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $push: {
      attachments: {
        name: "updated_blood_test.pdf",
        type: "application/pdf",
        url: "some_link_to_file.pdf",
        uploaded_at: new Date()
      }
    },
    $set: { updated_at: new Date() }
  }
);
```

### 3.6 Удалить лекарство из записи
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $pull: { medications: { name: "Aspirin" } },
    $set: { updated_at: new Date() }
  }
);

db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $pull: {
      medications: {
        name: "Ibuprofen",
        dosage: "400mg"
      }
    },
    $set: { updated_at: new Date() }
  }
);
```

### 3.7 Удалить вложенный файл
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $pull: {
      attachments: { name: "blood_test.pdf" }
    },
    $set: { updated_at: new Date() }
  }
);
```

### 3.8 Добавить тег к записи
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $addToSet: { tags: "urgent" },
    $set: { updated_at: new Date() }
  }
);

db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $addToSet: { 
      tags: { $each: ["reviewed", "approved", "2024"] }
    },
    $set: { updated_at: new Date() }
  }
);
```

### 3.9 Мягкое удаление записи
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $set: {
      is_deleted: true,
      updated_at: new Date()
    }
  }
);
```

### 3.10 Обновить несколько записей одновременно
```javascript
db.medical_records.updateMany(
  {
    created_at: { $lt: new Date(new Date().getTime() - 365 * 24 * 60 * 60 * 1000) },
    is_deleted: false
  },
  {
    $addToSet: { tags: "archived" },
    $set: { updated_at: new Date() }
  }
);
```

### 3.11 Увеличить счётчик и изменить дату
```javascript
db.medical_records.updateOne(
  { 
    code: "REC-1704067800-123456",
    "medications.name": "Aspirin"
  },
  {
    $inc: { "medications.$.duration": 5 },
    $set: { updated_at: new Date() }
  }
);
```
---

## 4. DELETE

### 4.1 Мягкое удаление
```javascript
db.medical_records.updateOne(
  { code: "REC-1704067800-123456" },
  {
    $set: {
      is_deleted: true,
      updated_at: new Date()
    }
  }
);
```

### 4.2 Физическое удаление одной записи
```javascript
db.medical_records.deleteOne({ code: "REC-1704931800-654321" });
```

### 4.3 Физическое удаление нескольких записей
```javascript
db.medical_records.deleteMany({ patient_id: "patient_unknown" });
```
---

## 5. Примеры запросов из API

### 5.1 Получение записи по коду (GET /records/{code})
```javascript
db.medical_records.findOne({ code: "REC-1704067800-123456" });
```

### 5.2 Получение истории записей пациента (GET /patients/{id}/history)
```javascript
db.medical_records.find({
  patient_id: "patient_100",
  is_deleted: false
}).sort({ created_at: -1 });
```

### 5.3 Создание медицинской записи (POST /records)
```javascript
db.medical_records.insertOne({
  code: generateRecordCode(),
  patient_id: "patient_100",
  created_by: "doctor_user",
  title: "New record",
  description: "Description of the medical event",
  record_type: "checkup",
  severity: "low",
  tags: ["routine"],
  attachments: [],
  medications: [],
  vital_signs: {},
  created_at: new Date(),
  updated_at: new Date(),
  is_deleted: false
});
```
