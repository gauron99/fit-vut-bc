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



// https://stackoverflow.com/questions/54282344/how-does-express-and-react-routes-work-on-initial-get-request-from-browser
// /api is the connector to the backend, exclude anything that matches "/api"
app.get(/^\/(?!api).*/mg, (req, res, next) => {
  res.send("index.html")
  next()
});

app.get("/api*",(req,res) => {
  var params = req.params[0];
  console.log(req.params)
  console.log(params)
});


app.get("/login",(req,res) => {
  res.send("login scrn");
})

app.get("/admin",(req,res) => {
  res.send("overlord page");
})

app.listen(PORT, () => {
  console.log("listening on port %s",PORT);
})