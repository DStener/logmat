INSERT OR IGNORE INTO RoleAndPermission (role, permission) 
SELECT (SELECT id FROM Role WHERE name == "Admin"), perm.id
FROM Permission as perm;