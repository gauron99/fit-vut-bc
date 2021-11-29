const path = require("path");
const express = require("express");
const app = express();
const PORT = 8000;
var router = express.Router();
var mysql = require('mysql');
const { json } = require("stream/consumers");

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

function fillObj2(results) {
    var pieces = {}
    for (const zaznam of results){
        pieces[zaznam.ID] = zaznam.firm;
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

// // on routes that end in /spoje
// // ----------------------------------------------------
// router.route('/spoje')

//     // create a spoj (accessed at POST http://localhost:8000/api/spoje)
//     .post(function(req, res) {
//         var spoj = {odc: req.query.odchod, odk: req.query.odkud, pri: req.query.prichod, kam: req.query.kam, cena: req.query.cena};
//         spoje[id++] = spoj;
//         // save the spoj and check for errors
//         res.json({ message: 'spoj spojed' });
//         console.log(spoje);
//     })

//     // get all the spoje (accessed at GET http://localhost:8000/api/spoje)
//     .get(function(req, res) {
//         res.json(spoje);
//     });


// USER MANAGEMENT API
router.route('/read_and_subsequently_possibly_config_if_desired_or_not_if_not_necessary_or_not_desired')

    .get(async function(req, res) {
        var persons = [];
        try{
            var results = await kvery('SELECT ID, name FROM Admin');
            for (let person of results){
                persons.push({ID: person.ID, name: person.name, type: "admin"});
            }        

            results = await kvery('SELECT ID, firm FROM Conveyor');
            for (let person of results){
                persons.push({ID: person.ID, name: person.firm, type: "conveyor"});
            }

            results = await kvery('SELECT ID, name FROM Crew');
            for (let person of results){
                persons.push({ID: person.ID, name: person.name, type: "crew"});
            }

            results = await kvery('SELECT ID, name FROM Passenger');
            for (let person of results){
                persons.push({ID: person.ID, name: person.name, type: "passenger"});
            }

            console.log("ouu",persons);
            res.json(persons);
        } catch(e) {
            console.log(e);
        }
    })

router.route('/passenger_manage')
    .get(async function(req, res){
        var result = await kvery('SELECT * FROM Passenger');
        res.json(result)
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Passenger(name,passwd) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\");');
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Passenger SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Passenger WHERE ID='+req.query.ID+';');
    })

router.route('/crew_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT Crew.ID, Crew.name, Crew.passwd FROM Crew, Conveyor WHERE Conveyor.firm = "'+req.query.firm+'" AND Crew.conveyorID = Conveyor.ID;');
        res.json(result)
})
    .post(async function(req, res) {
        await kvery('INSERT INTO Crew(name,passwd,conveyorID) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\",\"'+req.query.conveyorID+'\");');
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Crew SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\", conveyorID = \"'+req.query.conveyorID+'\" WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Crew WHERE ID='+req.query.ID+';');
    })

router.route('/conveyor_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Conveyor;');
        res.json(result)
    })

    .post(async function(req, res) {
        await kvery('INSERT INTO Conveyor(firm,passwd) VALUES (\"'+req.query.firm+'\",\"'+req.query.passwd+'\");');
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Conveyor SET firm = \"'+req.query.firm+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Conveyor WHERE ID='+req.query.ID+';');
    })

router.route('/admin_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Admin;');
        res.json(result)
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Admin(name,passwd) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\");');
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Admin SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Admin WHERE ID='+req.query.ID+';');
    })

router.route('/login')

    .post(async function(req, res) {
        var admins = await kvery('SELECT passwd FROM Admin WHERE name=\"'+req.query.name+'\";');
        var conveyors = await kvery('SELECT passwd FROM Conveyor WHERE firm=\"'+req.query.name+'\";');
        var crew = await kvery('SELECT passwd FROM Crew WHERE name=\"'+req.query.name+'\";');
        var passengers = await kvery('SELECT passwd FROM Passenger WHERE name=\"'+req.query.name+'\";');
        var answer = {};
        if (admins[0]){
            answer['ROLE'] = "admin";
            answer['ACCESS'] = admins[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
        }
        else if (conveyors[0]){
            answer['ROLE'] = "conveyor";
            answer['ACCESS'] = conveyors[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
        }
        else if (crew[0]){
            answer['ROLE'] = "crew";
            answer['ACCESS'] = crew[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
        }
        else if (passengers[0]){
            answer['ROLE'] = "passenger";
            answer['ACCESS'] = passengers[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
        }
        else {
            answer['ROLE'] = "nenalezeno";
            answer['ACCESS'] = "DENIED"
        }
        res.json(answer);
    })

// SPOJE MANAGEMENT API

router.route('/stops')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Stop WHERE ID='+req.query.ID+';');
        res.json(result);
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Stop(name,conveyorID,confirmed) VALUES (\"'+req.query.name+'\",'+req.query.conveyorID+',FALSE);');
    })

    .put(async function(req, res) {
        await kvery('UPDATE Stop SET name = \"'+req.query.name+'\", conveyorID = '+req.query.conveyorID+', confirmed = '+req.query.confirmed+' WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Stop WHERE ID='+req.query.ID+';');
    })


router.route('/stops_unconfirmed')
    .get(async function(req, res) {
        var results = await kvery('SELECT * FROM Stop WHERE confirmed is FALSE;');
        res.json(results);
    })

router.route('/confirm_stop')
    .post(async function(req, res) {
        await kvery('UPDATE Stop SET confirmed = TRUE WHERE ID='+req.query.ID+';');
    })

router.route('/vehicle')
    .get(async function(req, res) {
        var result = await kvery('SELECT FROM Vehicle WHERE ID='+req.query.ID+';');
        res.json(result);
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Vehicle(max_seats_poor,max_seats_rich,description,last_visited,conveyorID) VALUES ('+req.query.max_seats_poor+', '+req.query.max_seats_rich+',\"'+req.query.description+'\",'+req.query.last_visited+','+req.query.conveyorID+');');
    })

    .put(async function(req, res) {
        await kvery('UPDATE Vehicle SET max_seats_poor = '+req.query.max_seats_poor+', max_seats_rich = '+req.query.max_seats_rich+', description = \"'+req.query.description+'\", last_visited = '+req.query.last_visited+', conveyorID = '+req.query.conveyorID+' WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Vehicle WHERE ID='+req.query.ID+';');
    })


router.route('/spoje')
    .get(async function(req, res) {
        var spoje = [];
        var result = await kvery('SELECT connID, arrival FROM Connection_stop AS CSS WHERE CSS.stopID IN (SELECT ID FROM Stop WHERE Stop.ID = stopID AND Stop.name = "'+req.query.odkud+'") AND CSS.connID IN (SELECT connID FROM Connection_stop CS WHERE CS.stopID IN (SELECT ID FROM Stop AS S WHERE S.name = "'+req.query.kam+'") AND TIMEDIFF(CSS.arrival, CS.arrival) < 0 AND TIMEDIFF(TIME("'+req.query.kdy+'"),CSS.arrival) < 0);');
        for (let spoj of result){
            var spojReturn = {}
            var result2 = await kvery('SELECT arrival FROM Connection_stop WHERE stopID IN (SELECT ID FROM Stop AS S WHERE S.name = "'+req.query.kam+'") AND connID = '+spoj.connID+';');
            var price = await kvery('SELECT sum(price_rich) AS cenaRich, sum(price_poor) AS cenaPoor FROM Connection, Connection_stop WHERE '+spoj.connID+' = Connection.ID AND Connection.ID = Connection_stop.connID AND TIMEDIFF(TIME("'+spoj.arrival+'"),arrival) <= 0 AND TIMEDIFF(TIME("'+result2[0].arrival+'"),arrival) > 0;');
            spojReturn['odjezd'] = spoj.arrival;
            spojReturn['prijezd'] = result2[0].arrival;
            spojReturn['cenaRich'] = price[0].cenaRich;
            spojReturn['cenaPoor'] = price[0].cenaPoor;
            spoje.push(spojReturn);
        }
        res.json(spoje);
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Connection(conveyorID,vehicleID,price_poor,price_rich) VALUES ('+req.query.conveyorID+', '+req.query.vehicleID+','+req.query.price_poor+','+req.query.price_rich+');');
        var connID = await kvery('SELECT LAST_INSERT_ID();');
        var zastavky = req.body.zastavky;
        for (let stop of zastavky){
            var ID = await kvery('SELECT ID FROM Stop WHERE name=\"'+stop.name+'\";')
            await kvery('INSERT INTO Connection_stop(connID,stopID,time) VALUES ('+connID[0]['LAST_INSERT_ID()']+', '+ID[0].ID+','+stop.cas+');');
        }       
    })

    .put(async function(req, res) {
        await kvery('UPDATE Vehicle SET max_seats_poor = '+req.query.max_seats_poor+', max_seats_rich = '+req.query.max_seats_rich+', description = \"'+req.query.description+'\", last_visited = '+req.query.last_visited+', conveyorID = '+req.query.conveyorID+' WHERE ID='+req.query.ID+';');
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Vehicle WHERE ID='+req.query.ID+';');
    })

router.route('/test')

    .get(async function(req, res) {        
        var result = await kvery('SELECT ID, firm FROM Conveyor');
        res.json(result)
    })        
    
// all of our routes will be prefixed with /api
app.use('/api', router);


// https://stackoverflow.com/questions/54282344/how-does-express-and-react-routes-work-on-initial-get-request-from-browser
// /api is the connector to the backend, exclude anything that matches "/api"


app.get("*", (req, res) => {
  res.sendFile(path.join(__dirname,"client","build","index.html"));      
});

app.listen(PORT, () => {
  console.log("listening on port %s",PORT);
})
