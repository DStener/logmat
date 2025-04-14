INSERT OR IGNORE INTO RoleAndPermission (role, permission) 
SELECT (SELECT id FROM Role WHERE name == "Guest"), perm.id
FROM Permission as perm
WHERE 
(
	perm.name == "get-list-user" OR
	perm.name == "get-list-file"
);