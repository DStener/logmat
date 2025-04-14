INSERT OR IGNORE INTO RoleAndPermission (role, permission) 
SELECT (SELECT id FROM Role WHERE name == "User"), perm.id
FROM Permission as perm
WHERE 
(
	perm.name LIKE '%-file' OR
	perm.name == "get-list-user" OR
	perm.name == "read-user" OR
	perm.name == "update-user"
);