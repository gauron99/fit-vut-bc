const express = require("express");
const app = express();

const mysql = require('mysql');
const db = mysql.createPool({
  host:     '34.116.167.145',
  user:     'sqlserver',
  password: 'dbmasterkey',
  // database: 'the-db',
});

app.get("/",(req,res) => {
  res.send("ahoj");
})

app.get("/add",(req,res) => {
  const sqlIn = "INSERT INTO iis ()"
  res.send("added some stuff to DB :cuteface:");

})

app.get("/login",(req,res) => {
  res.send("login scrn");
})

app.get("/admin",(req,res) => {
  res.send("overlord page");
})

PORT = 8000;
app.listen(PORT, () => {
  console.log("listening on port %s",PORT);
})