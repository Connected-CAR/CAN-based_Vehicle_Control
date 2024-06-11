// get the client
const mysql = require('mysql2/promise');

// Create the connection pool. The pool-specific settings are the defaults
const pool = mysql.createPool({
  host: 'xx.xx.xx.xxx', // id
  user: 'wooyoung',
  password: "xxxx", // pwd
  database: 'wooDB',
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

module.exports = pool
