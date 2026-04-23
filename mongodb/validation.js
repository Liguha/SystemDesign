db.createCollection("medical_records", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: [
        "code",
        "patient_id",
        "created_by",
        "title",
        "description",
        "created_at",
        "updated_at"
      ],
      properties: {
        _id: {
          bsonType: "objectId",
          description: "The unique identifier for the document"
        },
        code: {
          bsonType: "string",
          pattern: "^REC-[0-9]{10}-[0-9]{6}$",
          description: "Unique record code in format REC-TIMESTAMP-RANDOM, required"
        },
        patient_id: {
          bsonType: "string",
          pattern: "^patient_[0-9]+$",
          description: "Reference to patient ID in PostgreSQL, required"
        },
        created_by: {
          bsonType: "string",
          minLength: 1,
          maxLength: 100,
          description: "Login of the doctor who created the record, required"
        },
        title: {
          bsonType: "string",
          minLength: 1,
          maxLength: 256,
          description: "Title of the medical record, required"
        },
        description: {
          bsonType: "string",
          minLength: 1,
          maxLength: 5000,
          description: "Detailed description of the medical event, required"
        },
        record_type: {
          enum: ["checkup", "surgery", "test", "prescription", "diagnosis", "vaccination", "consultation", "follow_up"],
          description: "Category of the medical record"
        },
        severity: {
          enum: ["low", "medium", "high", "critical"],
          description: "Severity level of the record"
        },
        tags: {
          bsonType: "array",
          items: {
            bsonType: "string",
            minLength: 1,
            maxLength: 50
          },
          description: "Array of tags for categorization and search"
        },
        attachments: {
          bsonType: "array",
          items: {
            bsonType: "object",
            required: ["name", "type", "url", "uploaded_at"],
            properties: {
              name: {
                bsonType: "string",
                minLength: 1,
                maxLength: 256,
                description: "Filename of the attachment"
              },
              type: {
                bsonType: "string",
                description: "MIME type of the attachment"
              },
              url: {
                bsonType: "string",
                description: "URL or path to the attachment"
              },
              uploaded_at: {
                bsonType: "date",
                description: "Timestamp when the attachment was uploaded"
              }
            }
          },
          description: "Array of attached documents and test results"
        },
        medications: {
          bsonType: "array",
          items: {
            bsonType: "object",
            required: ["name"],
            properties: {
              name: {
                bsonType: "string",
                minLength: 1,
                maxLength: 100,
                description: "Name of the medication"
              },
              dosage: {
                bsonType: "string",
                minLength: 1,
                maxLength: 100,
                description: "Dosage of the medication"
              },
              frequency: {
                bsonType: "string",
                minLength: 1,
                maxLength: 100,
                description: "Frequency of administration"
              },
              duration: {
                bsonType: "int",
                minimum: 1,
                maximum: 365,
                description: "Duration in days"
              }
            }
          },
          description: "Array of prescribed medications"
        },
        vital_signs: {
          bsonType: "object",
          properties: {
            blood_pressure: {
              bsonType: "string",
              pattern: "^[0-9]{2,3}/[0-9]{2,3}$",
              description: "Blood pressure in format SYS/DIAS"
            },
            heart_rate: {
              bsonType: "int",
              minimum: 30,
              maximum: 200,
              description: "Heart rate in beats per minute"
            },
            temperature: {
              bsonType: "double",
              minimum: 35.0,
              maximum: 42.0,
              description: "Body temperature in Celsius"
            },
            respiratory_rate: {
              bsonType: "int",
              minimum: 8,
              maximum: 40,
              description: "Respiratory rate in breaths per minute"
            }
          },
          description: "Object containing vital signs"
        },
        created_at: {
          bsonType: "date",
          description: "Creation timestamp, required"
        },
        updated_at: {
          bsonType: "date",
          description: "Last update timestamp, required"
        },
        is_deleted: {
          bsonType: "bool",
          description: "Soft delete flag"
        }
      },
      additionalProperties: false
    }
  }
});