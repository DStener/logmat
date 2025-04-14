--- "get-list" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-list-file", "Permission to SELECT all data", "get-list-file");
--- "get-story" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-story-file", "Permission to SELECT all story data", "get-story-file");
--- "read" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("read-file", "Permission to SELECT current data", "read-file");
--- "create" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("create-file", "Permission to CREATE data", "create-file");
--- "update" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("update-file", "Permission to UPDATE data", "update-file");
--- "delete" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("delete-file", "Permission to DELETE current data", "delete-file");
--- "restore" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("restore-file", "Permission to RESTORE current data", "restore-file");