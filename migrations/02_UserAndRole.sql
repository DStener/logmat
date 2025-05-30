CREATE TABLE IF NOT EXISTS UserAndRole
(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user INTEGER,
    role INTEGER,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_by INTEGER DEFAULT NULL,
    deleted_at TIMESTAMP DEFAULT "1970-01-01 00:00:00.000000000",
    deleted_by INTEGER DEFAULT NULL,

    FOREIGN KEY(created_by) REFERENCES User(id),
    FOREIGN KEY(deleted_by) REFERENCES User(id),

    UNIQUE(user, role),

    FOREIGN KEY(user) REFERENCES User(id),
    FOREIGN KEY(role) REFERENCES Role(id)
);
