INSERT OR IGNORE INTO UserAndRole
(user, role) VALUES
(
	(SELECT id FROM User WHERE username == "admin"),
	(SELECT id FROM Role WHERE name == "Admin")
);