CREATE TABLE IF NOT EXISTS User
(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR UNIQUE NOT NULL,
    avatar INTEGER NULL,
    email VARCHAR UNIQUE NOT NULL,
    time2FA TIMESTAMP DEFAULT "1970-01-01 00:00:00.000000000",
    birthday TIMESTAMP NOT NULL,
    password VARCHAR NOT NULL,

    FOREIGN KEY(avatar) REFERENCES File(id)
);
