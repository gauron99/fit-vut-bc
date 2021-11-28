const path = require("path");
const express = require("express");
const app = express();
const PORT = 8000;
var router = express.Router();
var mysql = require('mysql');

var spoje = {};
var id = 0;




// setup using body-parser (is integrated in express itself?) for POST requests
app.use(express.json());
app.use(express.urlencoded({ extended: true}));

var pool  = mysql.createPool({
    connectionLimit : 10,
    host            : '85.208.51.209',
    user            : 'loudik',
    password        : 'popici',
    database        : 'iis_db'
  });

kvery = (query) => {
    return new Promise((resolve,reject) =>{
        pool.query(query, (error, results) => {
            if (error) return reject(error);
            return resolve(results);
        });
    });
}

function fillObj(results) {
    var pieces = {}
    for (const zaznam of results){
        pieces[zaznam.ID] = zaznam.name;
    }
    return pieces;
}

// add middleware -- static files can now be used from frontend
app.use(express.static("client/build"));


// ROUTES FOR OUR API
// =============================================================================

// middleware to use for all requests
router.use(function(req, res, next) {
    // do logging
    console.log('brooo vrtame do apiny');
    next(); // make sure we go to the next routes and don't stop here
});

// test route to make sure everything is working (accessed at GET http://localhost:8000/api)
router.get('/', function(req, res){
    res.json({m: "eyo"});
});

// on routes that end in /spoje
// ----------------------------------------------------
router.route('/spoje')

    // create a spoj (accessed at POST http://localhost:8000/api/spoje)
    .post(function(req, res) {
        var spoj = {odc: req.query.odchod, odk: req.query.odkud, pri: req.query.prichod, kam: req.query.kam, cena: req.query.cena};
        spoje[id++] = spoj;
        // save the spoj and check for errors
        res.json({ message: 'spoj spojed' });
        console.log(spoje);
    })

    // get all the spoje (accessed at GET http://localhost:8000/api/spoje)
    .get(function(req, res) {
        res.json(spoje);
    });

router.route('/read_and_subsequently_possibly_config_if_desired_or_not_if_not_necessary_or_not_desired')

    .get(async function(req, res) {
        var persons = {};
        try{
            var results = await kvery('SELECT ID, name FROM Admin');
            persons['Admins'] = fillObj(results);
        
            results = await kvery('SELECT ID, name FROM Conveyor');
            persons['Conveyor'] = fillObj(results);

            results = await kvery('SELECT ID, name FROM Crew');
            persons['Crew'] = fillObj(results);

            results = await kvery('SELECT ID, name FROM Passenger');
            persons['Passenger'] = fillObj(results);

            console.log("ouu",persons);
            res.json(persons);
        } catch(e) {
            console.log(e);
        }
    })

router.route('/passenger_manage')

    .post(async function(req, res) {
        pool.query('INSERT INTO Passenger(name,passwd) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\");', (err, result) => {
            if (err) console.log('problemis');
        })
    })
    
    .put(async function(req, res) {
        pool.query('UPDATE Passenger SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID), (err, result) => {
            if (err) console.log('problemis');
        }
    })

    .delete(async function(req, res) {
        const id = parseInt(req.query.ID);
        pool.query('DELETE FROM Passenger WHERE ID='+id+';'), (err, result) => {
            if (err) console.log('problemis');
        }
    })

router.route('/crew_manage')

    .post(async function(req, res) {
        pool.query('INSERT INTO Crew(name,passwd,conveyorID) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\",\"'+req.query.conveyorID+'\");', (err, result) => {
            if (err) console.log('problemis');
        })
    })
    
    .put(async function(req, res) {
        pool.query('UPDATE Crew SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\", conveyorID = \"'+req.query.conveyorID+'\" WHERE ID='+req.query.ID), (err, result) => {
            if (err) console.log('problemis');
        }
    })

    .delete(async function(req, res) {
        const id = parseInt(req.query.ID);
        pool.query('DELETE FROM Crew WHERE ID='+id+';'), (err, result) => {
            if (err) console.log('problemis');
        }    
    })

router.route('/conveyor_manage')

    .post(async function(req, res) {
        pool.query('INSERT INTO Conveyor(firm,passwd) VALUES (\"'+req.query.firm+'\",\"'+req.query.passwd+'\");', (err, result) => {
            if (err) console.log('problemis');
        })
    })
    
    .put(async function(req, res) {
        pool.query('UPDATE Conveyor SET firm = \"'+req.query.firm+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID), (err, result) => {
            if (err) console.log('problemis');
        }
    })

    .delete(async function(req, res) {
        const id = parseInt(req.query.ID);
        pool.query('DELETE FROM Conveyor WHERE ID='+id+';'), (err, result) => {
            if (err) console.log('problemis');
        }
    })

router.route('/admin_manage')

    .post(async function(req, res) {
        pool.query('INSERT INTO Admin(name,passwd) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\");', (err, result) => {
            if (err) console.log('problemis');
        })
    })
    
    .put(async function(req, res) {
        pool.query('UPDATE Admin SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID), (err, result) => {
            if (err) console.log('problemis');
        }
    })

    .delete(async function(req, res) {
        const id = parseInt(req.query.ID);
        pool.query('DELETE FROM Admin WHERE ID='+id+';'), (err, result) => {
            if (err) console.log('problemis');
        }
    })

// all of our routes will be prefixed with /api
app.use('/api', router);


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
