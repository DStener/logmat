--- "get-list" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-list-permission", "Permission to SELECT all data", "get-list-permission");
--- "get-story" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("get-story-permission", "Permission to SELECT all story data", "get-story-permission");
--- "read" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("read-permission", "Permission to SELECT current data", "read-permission");
--- "create" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("create-permission", "Permission to CREATE data", "create-permission");
--- "update" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("update-permission", "Permission to UPDATE data", "update-permission");
--- "delete" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("delete-permission", "Permission to DELETE current data", "delete-permission");
--- "restore" permission ---
INSERT OR IGNORE INTO Permission
(name, description, code) VALUES 
("restore-permission", "Permission to RESTORE current data", "restore-permission");