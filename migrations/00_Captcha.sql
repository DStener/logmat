CREATE TABLE IF NOT EXISTS Captcha
(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    token VARCHAR UNIQUE NOT NULL,
    answer REAL,
    time TIMESTAMP
);