const path = require("path");
const express = require("express");
const app = express();
const PORT = 8000;

// Database setup
// const mysql = require('mysql');
// const db = mysql.createPool({
//   host:     '34.116.167.145',
//   user:     'root',
//   password: 'dbmasterkey',
//   // database: 'the-db',
// });


// add middleware
app.use(express.static("client/build"));

// app.get("/", (req, res) => {
  // res.sendFile("/index.html")
  // res.sendFile(path.join(__dirname,"public","index.html"));
  // console.log("send dis file to cons!");
//  });

// app.get("/add",(req,res) => {
//   const sqlIn = "INSERT INTO iis ()"
//   res.send("added some stuff to DB :cuteface:");
// })

app.get("/login",(req,res) => {
  res.send("login scrn");
})

app.get("/admin",(req,res) => {
  res.send("overlord page");
})

app.listen(PORT, () => {
  console.log("listening on port %s",PORT);
})