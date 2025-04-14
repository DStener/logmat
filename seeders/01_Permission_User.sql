--- "get-list" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-list-user", "Permission to SELECT all data", "get-list-user");
--- "get-story" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-story-user", "Permission to SELECT all story data", "get-story-user");
--- "read" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("read-user", "Permission to SELECT current data", "read-user");
--- "create" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("create-user", "Permission to CREATE data", "create-user");
--- "update" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("update-user", "Permission to UPDATE data", "update-user");
--- "delete" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("delete-user", "Permission to DELETE current data", "delete-user");
--- "restore" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("restore-user", "Permission to RESTORE current data", "restore-user");