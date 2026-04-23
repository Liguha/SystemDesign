db.medical_records.insertMany([
  {
    _id: ObjectId(),
    code: "REC-1704067800-123456",
    patient_id: "patient_100",
    created_by: "doctor_user",
    title: "Initial checkup",
    description: "Patient is ok. ",
    record_type: "checkup",
    severity: "low",
    tags: ["routine", "checkup", "2024", "preventive"],
    attachments: [
      {
        name: "blood_test.pdf",
        type: "application/pdf",
        url: "some_link.pdf",
        uploaded_at: new Date("2024-01-15T10:30:00Z")
      }
    ],
    medications: [
      {
        name: "Aspirin",
        dosage: "500g",
        frequency: "sometimes...",
        duration: 30
      }
    ],
    vital_signs: {
      blood_pressure: "120/80",
      heart_rate: 72,
      temperature: 36.6,
      respiratory_rate: 16
    },
    created_at: new Date("2024-01-15T10:30:00Z"),
    updated_at: new Date("2024-01-15T10:30:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704154200-234567",
    patient_id: "patient_100",
    created_by: "doctor_user",
    title: "Follow-up visit",
    description: "Patient ok.",
    record_type: "follow_up",
    severity: "low",
    tags: ["follow_up", "medication_adjustment", "2024"],
    attachments: [],
    medications: [
      {
        name: "Aspirin",
        dosage: "250mg",
        frequency: "twice ",
        duration: 14
      },
      {
        name: "Vitamin C",
        dosage: "1000mg",
        frequency: "once",
        duration: 14
      }
    ],
    vital_signs: {
      blood_pressure: "118/78",
      heart_rate: 70,
      temperature: 36.5,
      respiratory_rate: 16
    },
    created_at: new Date("2024-01-22T14:15:00Z"),
    updated_at: new Date("2024-01-22T14:15:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704240600-345678",
    patient_id: "patient_101",
    created_by: "nina_smirnova",
    title: "Annual physical",
    description: "All ok.",
    record_type: "checkup",
    severity: "low",
    tags: ["annual", "physical", "healthy", "2024"],
    attachments: [
      {
        name: "ecg_result.pdf",
        type: "application/pdf",
        url: "l1nk_t0_file.pdf",
        uploaded_at: new Date("2024-01-29T09:00:00Z")
      },
      {
        name: "chest_xray.jpg",
        type: "image/jpeg",
        url: "link_xray_p101.jpg",
        uploaded_at: new Date("2024-01-29T09:15:00Z")
      }
    ],
    medications: [],
    vital_signs: {
      blood_pressure: "115/75",
      heart_rate: 69,
      temperature: 36.7,
      respiratory_rate: 16
    },
    created_at: new Date("2024-01-29T11:00:00Z"),
    updated_at: new Date("2024-01-29T11:00:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704327000-456789",
    patient_id: "patient_102",
    created_by: "oleg_kuznetsov",
    title: "Blood pressure check",
    description: "Ok again.",
    record_type: "test",
    severity: "low",
    tags: ["blood_pressure", "hypertension_screening", "preventive"],
    attachments: [],
    medications: [],
    vital_signs: {
      blood_pressure: "122/82",
      heart_rate: 75,
      temperature: 36.7,
      respiratory_rate: 17
    },
    created_at: new Date("2024-02-05T15:30:00Z"),
    updated_at: new Date("2024-02-05T15:30:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704413400-567890",
    patient_id: "patient_103",
    created_by: "pavel_sokolov",
    title: "Vaccination",
    description: "Vaccinated.",
    record_type: "vaccination",
    severity: "low",
    tags: ["vaccination", "seasonal", "preventive", "2024"],
    attachments: [
      {
        name: "vaccine_certificate.pdf",
        type: "application/pdf",
        url: "link_vaccine_cert_p103.pdf",
        uploaded_at: new Date("2024-02-12T10:00:00Z")
      }
    ],
    medications: [
      {
        name: "Acetaminophen",
        dosage: "500mg",
        frequency: "sometimes",
        duration: 3
      }
    ],
    vital_signs: {
      blood_pressure: "120/80",
      heart_rate: 72,
      temperature: 36.8,
      respiratory_rate: 16
    },
    created_at: new Date("2024-02-12T10:15:00Z"),
    updated_at: new Date("2024-02-12T10:15:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704499800-678901",
    patient_id: "patient_104",
    created_by: "elena_kovaleva",
    title: "Allergy exam",
    description: "Almost ok.", 
    record_type: "diagnosis",
    severity: "medium",
    tags: ["allergy", "pollen", "seasonal", "dermatology"],
    attachments: [
      {
        name: "allergy_test_results.pdf",
        type: "application/pdf",
        url: "link_allergy_test_p104.pdf",
        uploaded_at: new Date("2024-02-19T14:00:00Z")
      }
    ],
    medications: [
      {
        name: "Loratadine",
        dosage: "10mg",
        frequency: "once daily",
        duration: 90
      },
      {
        name: "Topical steroid cream",
        dosage: "1%",
        frequency: "twice daily as needed",
        duration: 30
      }
    ],
    vital_signs: {
      blood_pressure: "121/81",
      heart_rate: 73,
      temperature: 36.7,
      respiratory_rate: 16
    },
    created_at: new Date("2024-02-19T14:30:00Z"),
    updated_at: new Date("2024-02-19T14:30:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704586200-789012",
    patient_id: "patient_105",
    created_by: "maria_fedorova",
    title: "MRI result",
    description: "No abnormalities - he is ok!",
    record_type: "test",
    severity: "low",
    tags: ["mri", "imaging", "radiology", "normal"],
    attachments: [
      {
        name: "mri_scan.dcm",
        type: "application/dicom",
        url: "link_mri_p105.dcm",
        uploaded_at: new Date("2024-02-26T10:00:00Z")
      },
      {
        name: "radiologist_report.pdf",
        type: "application/pdf",
        url: "link_radiology_report_p105.pdf",
        uploaded_at: new Date("2024-02-26T11:30:00Z")
      }
    ],
    medications: [],
    vital_signs: {
      blood_pressure: "119/79",
      heart_rate: 71,
      temperature: 36.6,
      respiratory_rate: 16
    },
    created_at: new Date("2024-02-26T12:00:00Z"),
    updated_at: new Date("2024-02-26T12:00:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704672600-890123",
    patient_id: "patient_106",
    created_by: "sergey_ivanov",
    title: "Dental review",
    description: "Cavity treatment suggested.",
    record_type: "consultation",
    severity: "medium",
    tags: ["dental", "cavity", "referral", "dentistry"],
    attachments: [
      {
        name: "dental_exam_notes.pdf",
        type: "application/pdf",
        url: "link_dental_exam_p106.pdf",
        uploaded_at: new Date("2024-03-04T15:00:00Z")
      }
    ],
    medications: [
      {
        name: "Ibuprofen",
        dosage: "400mg",
        frequency: "twice daily as needed",
        duration: 7
      }
    ],
    vital_signs: {
      blood_pressure: "120/80",
      heart_rate: 72,
      temperature: 36.6,
      respiratory_rate: 16
    },
    created_at: new Date("2024-03-04T15:20:00Z"),
    updated_at: new Date("2024-03-04T15:20:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704759000-901234",
    patient_id: "patient_107",
    created_by: "anna_popova",
    title: "Dermatology consult",
    description: "Dry skin condition observed.",
    record_type: "consultation",
    severity: "low",
    tags: ["dermatology", "dry_skin", "skincare", "topical"],
    attachments: [
      {
        name: "skin_condition_photos.zip",
        type: "application/zip",
        url: "link_skin_photos_p107.zip",
        uploaded_at: new Date("2024-03-11T10:30:00Z")
      }
    ],
    medications: [
      {
        name: "Hydrating cream",
        dosage: "Apply liberally",
        frequency: "twice daily",
        duration: 60
      },
      {
        name: "Moisturizing lotion",
        dosage: "Apply as needed",
        frequency: "daily",
        duration: 90
      }
    ],
    vital_signs: {
      blood_pressure: "118/78",
      heart_rate: 70,
      temperature: 36.6,
      respiratory_rate: 16
    },
    created_at: new Date("2024-03-11T11:00:00Z"),
    updated_at: new Date("2024-03-11T11:00:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704845400-012345",
    patient_id: "patient_108",
    created_by: "ivan_orlov",
    title: "Cardiology check",
    description: "Arrhythmia not detected .",
    record_type: "checkup",
    severity: "low",
    tags: ["cardiology", "heart", "ecg", "normal"],
    attachments: [
      {
        name: "ecg_trace.pdf",
        type: "application/pdf",
        url: "link_ecg_p108.pdf",
        uploaded_at: new Date("2024-03-18T09:00:00Z")
      },
      {
        name: "stress_test_report.pdf",
        type: "application/pdf",
        url: "link_stress_test_p108.pdf",
        uploaded_at: new Date("2024-03-18T11:00:00Z")
      }
    ],
    medications: [],
    vital_signs: {
      blood_pressure: "120/80",
      heart_rate: 68,
      temperature: 36.6,
      respiratory_rate: 16
    },
    created_at: new Date("2024-03-18T12:00:00Z"),
    updated_at: new Date("2024-03-18T12:00:00Z"),
    is_deleted: false
  },
  {
    _id: ObjectId(),
    code: "REC-1704931800-123457",
    patient_id: "patient_109",
    created_by: "doctor_user",
    title: "Orthopedic examination",
    description: "pain reported",
    record_type: "consultation",
    severity: "medium",
    tags: ["orthopedic", "knee", "physical_therapy", "xray"],
    attachments: [
      {
        name: "knee_xray.jpg",
        type: "image/jpeg",
        url: "link_knee_xray_p109.jpg",
        uploaded_at: new Date("2024-03-25T14:00:00Z")
      }
    ],
    medications: [
      {
        name: "Naproxen",
        dosage: "500mg",
        frequency: "twice daily with food",
        duration: 14
      }
    ],
    vital_signs: {
      blood_pressure: "122/82",
      heart_rate: 74,
      temperature: 36.7,
      respiratory_rate: 17
    },
    created_at: new Date("2024-03-25T14:30:00Z"),
    updated_at: new Date("2024-03-25T14:30:00Z"),
    is_deleted: false
  }
]);

db.medical_records.createIndex({ "code": 1 }, { unique: true });
db.medical_records.createIndex({ "patient_id": 1 });
db.medical_records.createIndex({ "created_by": 1 });
db.medical_records.createIndex({ "created_at": -1 });
db.medical_records.createIndex({ "tags": 1 });
db.medical_records.createIndex({ "patient_id": 1, "created_at": -1 });
db.medical_records.createIndex({ "created_by": 1, "created_at": -1 });
db.medical_records.createIndex({ "title": "text", "description": "text" });