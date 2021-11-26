const path = require("path");
const express = require("express");
const app = express();
const PORT = 8000;

// setup using body-parser (is integrated in express itself?) for POST requests
app.use(express.json());
app.use(express.urlencoded({ extended: true}));

// Database setup
// const mysql = require('mysql');
// const db = mysql.createPool({
//   host:     '34.116.167.145',
//   user:     'root',
//   password: 'dbmasterkey',
//   // database: 'the-db',
// });

// add middleware -- static files can now be used from frontend
app.use(express.static("client/build"));


app.get("/api*",(req,res) => {
  let O = JSON.stringify(req.query); //this works
  let m = req.query.from;
  let n = req.query.to;
  let o = req.query.date;
  let p = req.query.time;
  console.log(m)

  console.log(req.params)
  console.log(req.params[0])
  res.json({m: "api call bakc boi|" + O + "|"+ m + n + o +p})
});

// https://stackoverflow.com/questions/54282344/how-does-express-and-react-routes-work-on-initial-get-request-from-browser
// /api is the connector to the backend, exclude anything that matches "/api"

//TODO might move this under /api handler because priority exists and the handler
//above WILL handle FIRST. No Next() will be called therefore the second handler
//wont do anything... test if works

// app.get(/^\/(?!api).*/mg, (req, res, next) => {
//   res.send("index.html")
//   next()
// });
app.get("*", (req, res, next) => {
  res.send("index.html")
  next()
});

app.listen(PORT, () => {
  console.log("listening on port %s",PORT);
})
