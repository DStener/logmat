--- "admin" role ---
INSERT OR IGNORE INTO Role
(name, description, code) VALUES 
("Admin", "Administrator", "ADMIN");
--- "user" role ---
INSERT OR IGNORE INTO Role
(name, description, code) VALUES 
("User", "Standert user", "USER");
--- "guest" role ---
INSERT OR IGNORE INTO Role
(name, description, code) VALUES 
("Guest", "Guest", "GUEST");