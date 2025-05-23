CREATE TABLE IF NOT EXISTS File
(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR NOT NULL,
    description VARCHAR,
    tags VARCHAR,
    size INTEGER,
    _path VARCHAR NOT NULL,
    _avatar_path VARCHAR,

    --- SERVICE_FIELDS ---
	created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	created_by INTEGER DEFAULT NULL,
	deleted_at TIMESTAMP DEFAULT "1970-01-01 00:00:00.000000000",
	deleted_by INTEGER DEFAULT NULL,
	
	FOREIGN KEY(created_by) REFERENCES User(id),
	FOREIGN KEY(deleted_by) REFERENCES User(id)
);