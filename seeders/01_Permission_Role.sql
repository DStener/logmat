--- "get-list" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-list-role", "Permission to SELECT all data", "get-list-role");
--- "get-story" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-story-role", "Permission to SELECT all story data", "get-story-role");
--- "read" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("read-role", "Permission to SELECT current data", "read-role");
--- "create" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("create-role", "Permission to CREATE data", "create-role");
--- "update" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("update-role", "Permission to UPDATE data", "update-role");
--- "delete" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("delete-role", "Permission to DELETE current data", "delete-role");
--- "restore" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("restore-role", "Permission to RESTORE current data", "restore-role");