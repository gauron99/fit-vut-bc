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

//  var pool  = mysql.createPool({
//      connectionLimit : 10,
//      host            : 'localhost',
//      user            : 'root',
//      password        : '',
//      // database        : 'iis_db',
//      database        : 'backup',
//      port            : 3306
//    });

// var pool  = mysql.createPool({
//    connectionLimit : 10,
//    host            : '85.208.51.209',
//    user            : 'kuli_home',
//     password        : 'secret',
//     database        : 'iis_db',
// //    database        : 'backup',
//  });

// var pool  = mysql.createPool({
//     connectionLimit : 10,
//     host            : '85.208.51.209',
//     user            : 'kuli4',
//     password        : 'secret',
//     database        : 'iis_db',
// //  //    database        : 'backup',
//   });

var pool  = mysql.createPool({
    connectionLimit : 10,
    host            : '85.208.51.209',
    user            : 'loudik',
    password        : 'popici',
    database        : 'iis_db',
//  //    database        : 'backup',
  });
  


insertKvery = (query) => {
    return new Promise((resolve,reject) =>{
        pool.query(query, (error, results) => {
            if (error) return reject(error);
            return resolve(results.insertId);
        });
    });
}

kvery = (query) => {
    return new Promise((resolve,reject) =>{
        pool.query(query, (error, results) => {
            if (error) return reject(error);
            console.log(results.insertId)
            return resolve(results);
        });
    });
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

            results = await kvery('SELECT ID, name FROM Passenger WHERE registered=TRUE');
            for (let person of results){
                persons.push({ID: person.ID, name: person.name, type: "passenger"});
            }

            console.log("ouu",persons);
            res.json(persons);
        } catch(e) {
            console.log(e);
        }
    })

router.route('/getpwd')
    .get(async function(req, res){
        var tabka =  req.query.role[0].toUpperCase() + req.query.role.substring(1);
        var result = await kvery('SELECT passwd FROM '+tabka+' WHERE ID='+req.query.ID);
        res.json(result[0])
    })

router.route('/isuserok')
    .get(async function(req, res){
        var result1 = await kvery('SELECT * FROM Passenger WHERE name=\"'+req.query.name+'\";');
        var result2 = await kvery('SELECT * FROM Crew WHERE name=\"'+req.query.name+'\";');
        var result3 = await kvery('SELECT * FROM Admin WHERE name=\"'+req.query.name+'\";');
        var result4 = await kvery('SELECT * FROM Conveyor WHERE firm=\"'+req.query.name+'\";');
        var bruh = 0;
        result1.length > 0 ? bruh++ : bruh;
        result2.length > 0 ? bruh++ : bruh;
        result3.length > 0 ? bruh++ : bruh;
        result4.length > 0 ? bruh++ : bruh;
        res.json({msgis: !bruh})
    })

router.route('/passenger_manage')
    .get(async function(req, res){
        var result = await kvery('SELECT * FROM Passenger WHERE registered=TRUE');
        res.json(result)
    })
    .post(async function(req, res) {
        await kvery('SELECT * FROM Passenger')
        await kvery('INSERT INTO Passenger(name,passwd,registered) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\",'+req.query.registered+');');
        res.json({msg: "sup"})
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Passenger SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Passenger WHERE name=\"'+req.query.name+'\";');
        res.json({msg: "sup"})
    })

router.route('/getdopravceID')
    .get(async function(req, res) {
        var result = await kvery('SELECT ID FROM Conveyor WHERE firm="'+req.query.name+'";');
        if (result.length>0)
            res.json(result[0])
        else
            res.json({ID: 'none'})
    })

router.route('/crew_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT Crew.ID, Crew.name, Crew.passwd FROM Crew, Conveyor WHERE Conveyor.firm = "'+req.query.firm+'" AND Crew.conveyorID = Conveyor.ID;');
        res.json(result)
    })

    .post(async function(req, res) {
        await kvery('INSERT INTO Crew(name,passwd,conveyorID) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\",\"'+req.query.conveyorID+'\");');
        res.json({msg: "sup"})
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Crew SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Crew WHERE name=\"'+req.query.name+'\";');
        res.json({msg: "sup"})
    })  

router.route('/conveyor_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Conveyor;');
        res.json(result)
    })

    .post(async function(req, res) {
        await kvery('INSERT INTO Conveyor(firm,passwd) VALUES (\"'+req.query.firm+'\",\"'+req.query.passwd+'\");');
        res.json({msg: "sup"})
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Conveyor SET firm = \"'+req.query.firm+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Conveyor WHERE firm=\"'+req.query.firm+'\";');
        res.json({msg: "sup"})
    })

router.route('/admin_manage')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Admin;');
        res.json(result)
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Admin(name,passwd) VALUES (\"'+req.query.name+'\",\"'+req.query.passwd+'\");');
        res.json({msg: "sup"})
    })
    
    .put(async function(req, res) {
        await kvery('UPDATE Admin SET name = \"'+req.query.name+'\", passwd = \"'+req.query.passwd+'\" WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Admin WHERE name=\"'+req.query.name+'\";');
        res.json({msg: "sup"})
    })

router.route('/login')

    .post(async function(req, res) {
        var admins = await kvery('SELECT passwd,ID FROM Admin WHERE name=\"'+req.query.name+'\";');
        var conveyors = await kvery('SELECT passwd,ID FROM Conveyor WHERE firm=\"'+req.query.name+'\";');
        var crew = await kvery('SELECT passwd,ID FROM Crew WHERE name=\"'+req.query.name+'\";');
        var passengers = await kvery('SELECT passwd,ID FROM Passenger WHERE name=\"'+req.query.name+'\" AND registered = TRUE;');
        var answer = {};
        if (admins[0]){
            answer['ROLE']   = "admin";
            answer['ACCESS'] = admins[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
            answer['ID']     = admins[0].ID;
        }
        else if (conveyors[0]){
            answer['ROLE']   = "conveyor";
            answer['ACCESS'] = conveyors[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
            answer['ID']     = conveyors[0].ID;
        }
        else if (crew[0]){
            answer['ROLE']   = "crew";
            answer['ACCESS'] = crew[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
            answer['ID']     = crew[0].ID;
        }
        else if (passengers[0]){
            answer['ROLE']   = "passenger";
            answer['ACCESS'] = passengers[0].passwd == req.query.passwd ? "GRANTED" : "DENIED";
            answer['ID']     = passengers[0].ID;
        }
        else {
            answer['ROLE']   = "nenalezeno";
            answer['ACCESS'] = "DENIED"
            answer['ID']     = -1;
        }
        res.json(answer);
    })

// SPOJE MANAGEMENT API

router.route('/isstopok')
    .get(async function(req, res){
        var result = await kvery('SELECT * FROM Stop WHERE name=\"'+req.query.name+'\";');
        var bruh = 0;
        result.length > 0 ? bruh++ : bruh;
        res.json({msgis: !bruh})
    })


router.route('/new_stop')
    .post(async function(req, res) {
        await kvery('INSERT INTO Stop(name,conveyorID,confirmed) VALUES (\"'+req.query.name+'\",'+req.query.conveyorID+',FALSE);');
        res.json({msg: "sup"})
    })

router.route('/stops')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Stop WHERE ID='+req.query.ID+';');
        res.json(result);
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Stop(name,conveyorID,confirmed) VALUES (\"'+req.query.name+'\",'+req.query.conveyorID+',FALSE);');
        res.json({msg: "sup"})
    })

    .put(async function(req, res) {
        await kvery('UPDATE Stop SET name = \"'+req.query.name+'\", conveyorID = '+req.query.conveyorID+', confirmed = '+req.query.confirmed+' WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Stop WHERE name='+req.query.name+';');
        res.json({msg: "sup"})
    })

// get all stops that have been confirmed (for conveyors to propose an update)
router.route('/stops_all_confirmed')
    .get(async function(req,res) {
        var results = await kvery('SELECT * FROM Stop WHERE confirmed=1;');
        res.json(results);

    })
// get all stops for admin
router.route('/stops_all')
    .get(async function(req,res) {
        var results = await kvery('SELECT * FROM Stop');
        res.json(results);
    })


    // get all stops for one connection ID
router.route('/stops_by_conn')
    .get(async function(req,res) {
        var results = await kvery('SELECT name,arrival,stopID FROM Connection_stop,Stop WHERE connID=\"'+req.query.connID+'\" AND confirmed=1 AND Connection_stop.stopID=Stop.ID ORDER BY arrival;');
        res.json(results);
    })
    .post(async function(req,res) {
        var results = await kvery('INSERT INTO Connection_stop(connID,stopID,arrival) VALUES (\"'+req.query.connID+'\",'+req.query.stopID+',\"'+req.query.arrival+'\");');
        res.json(results);
    })

    .delete(async function(req,res) {
        var results = await kvery('DELETE FROM Connection_stop WHERE connID='+req.query.connID+' AND stopID='+req.query.stopID+';');
        res.json({msg: "sup"});
    })
    
    
router.route('/stops_unconfirmed')
    .get(async function(req, res) {
        var results = await kvery('SELECT * FROM Stop WHERE confirmed is FALSE;');
        res.json(results);
    })

router.route('/confirm_stop')
    .post(async function(req, res) {
        await kvery('UPDATE Stop SET confirmed = TRUE WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

// get all vehicles from one conveyor by conveyorID
router.route('/conveyor_vehicle')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Vehicle WHERE conveyorID='+req.query.conveyorID+';');
        res.json(result);
})

router.route('/getvehicles')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Vehicle;');
        res.json(result);
    })


router.route('/available_vehicles')
    .get(async function(req, res) {
        var result = await kvery('SELECT DISTINCT Vehicle.ID, Vehicle.max_seats_poor, Vehicle.max_seats_rich FROM Vehicle, Connection WHERE Vehicle.ID NOT IN (SELECT Vehicle.ID FROM Connection, Vehicle WHERE Connection.vehicleID=Vehicle.ID);');
        res.json(result);
    })

router.route('/vehicle')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Vehicle WHERE ID='+req.query.ID+';');
        res.json(result);
    })
    .post(async function(req, res) {
        await kvery('INSERT INTO Vehicle(max_seats_poor,max_seats_rich,description,last_visited,conveyorID) VALUES ('+req.query.max_seats_poor+', '+req.query.max_seats_rich+',\"'+req.query.description+'\",'+req.query.last_visited+','+req.query.conveyorID+');');
        res.json({msg: "sup"})
    })

    .put(async function(req, res) {
        await kvery('UPDATE Vehicle SET max_seats_poor = '+req.query.max_seats_poor+', max_seats_rich = '+req.query.max_seats_rich+', description = \"'+req.query.description+'\", conveyorID = '+req.query.conveyorID+' WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Vehicle WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })


router.route('/spoj')
    .get(async function(req, res) {
      var result = await kvery('SELECT * FROM Connection WHERE ID='+req.query.ID);
      res.json(result[0]);  
    })
    // delete connection
    .delete(async function(req, res) {
        var result = await kvery('DELETE FROM Connection WHERE ID='+req.query.ID);
        res.json({msg: "sup"});  
      })
  
// get all connections by conveyor ID
router.route('/spoj_conveyor')
.get(async function(req, res) {
    var result = await kvery('SELECT * FROM Connection WHERE conveyorID='+req.query.conveyorID);
    // console.log(result);
    res.json(result);  
})


router.route('/spoje')
    .get(async function(req, res) {
        var spoje = [];
        var result = await kvery('SELECT connID, arrival FROM Connection_stop AS CSS WHERE CSS.stopID IN (SELECT ID FROM Stop WHERE Stop.ID = stopID AND Stop.confirmed=true AND Stop.name = "'+req.query.odkud+'") AND CSS.connID IN (SELECT connID FROM Connection_stop CS WHERE CS.stopID IN (SELECT ID FROM Stop AS S WHERE S.confirmed=true AND S.name = "'+req.query.kam+'") AND TIMEDIFF(CSS.arrival, CS.arrival) < 0 AND TIMEDIFF(TIME("'+req.query.kdy+'"),CSS.arrival) < 0);');
        for (let spoj of result){
            var spojReturn = {}
            var result2 = await kvery('SELECT arrival FROM Connection_stop WHERE stopID IN (SELECT ID FROM Stop AS S WHERE S.name = "'+req.query.kam+'") AND connID = '+spoj.connID+';');
            var price = await kvery('SELECT sum(price_rich) AS cenaRich, sum(price_poor) AS cenaPoor FROM Connection, Connection_stop WHERE '+spoj.connID+' = Connection.ID AND Connection.ID = Connection_stop.connID AND TIMEDIFF(TIME("'+spoj.arrival+'"),arrival) <= 0 AND TIMEDIFF(TIME("'+result2[0].arrival+'"),arrival) > 0;');
            spojReturn['linka'] = spoj.connID;
            spojReturn['odjezd'] = spoj.arrival;
            spojReturn['odkud'] = req.query.odkud;
            spojReturn['prijezd'] = result2[0].arrival;
            spojReturn['kam'] = req.query.kam;
            spojReturn['cenaPoor'] = price[0].cenaPoor;
            spojReturn['cenaRich'] = price[0].cenaRich;
            spoje.push(spojReturn);
        }
        res.json(spoje);
    })
    .post(async function(req, res) {
        var connID = await insertKvery('INSERT INTO Connection(conveyorID,vehicleID,price_poor,price_rich) VALUES ('+req.query.conveyorID+', '+req.query.vehicleID+','+req.query.price_poor+','+req.query.price_rich+');');
        var zastavky = req.body.zastavky;
        for (let stop of zastavky){
            var ID = await kvery('SELECT ID FROM Stop WHERE name=\"'+stop.name+'\";')
            await kvery('INSERT INTO Connection_stop(connID,stopID,arrival) VALUES ('+connID+', '+ID[0].ID+',\"'+stop.cas+'\");');
        }
        res.json({msg: "sup"})
    })

    .put(async function(req, res) {
        await kvery('UPDATE Vehicle SET max_seats_poor = '+req.query.max_seats_poor+', max_seats_rich = '+req.query.max_seats_rich+', description = \"'+req.query.description+'\", last_visited = '+req.query.last_visited+', conveyorID = '+req.query.conveyorID+' WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

    .delete(async function(req, res) {
        await kvery('DELETE FROM Vehicle WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

router.route('/ultimate_getfucked')
    .get(async function(req, res) {
        var resultIDs = await kvery('SELECT Connection.ID as CID FROM Connection, Crew WHERE Crew.name=\"'+req.query.name+'\" AND Crew.conveyorID=Connection.conveyorID;');
        var final = {}
        for(let resID of resultIDs){
            var resultStops = await kvery('SELECT Stop.name as stopName FROM Stop, Connection_stop WHERE Stop.ID=Connection_stop.stopID AND Connection_stop.connID='+resID.CID+';');
            final[resID.CID] = resultStops;
        }
        res.json(final)
    })

router.route('/getvehicle')
    .get(async function(req, res) {        
        var result = await kvery('SELECT vehicleID FROM Connection WHERE ID = '+req.query.ID+';');
        res.json(result[0])
    })

router.route('/gde_spoj')
    .get(async function(req, res) {        
        var result = await kvery('SELECT Stop.name as stopName FROM Vehicle, Stop WHERE Vehicle.last_visited = Stop.ID AND Vehicle.ID = '+req.query.ID+';');
        res.json(result[0])
    })
    .post(async function(req, res) {
        var vehicleID = await kvery('SELECT vehicleID FROM Connection WHERE ID='+req.query.connID+';');
        var stopID = await kvery('SELECT ID FROM Stop WHERE name=\"'+req.query.stopName+'\";');
        await kvery('UPDATE Vehicle SET last_visited='+stopID[0].ID+' WHERE ID ='+vehicleID[0].vehicleID+';');
        res.json({msg: "sup"})
    })    

router.route('/reservation')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Reservation WHERE ID = '+req.query.ID+';');
        res.json(result)
    })

    .post(async function(req, res){
        var fail = 0;
        var vehID = await kvery('SELECT vehicleID FROM Connection WHERE ID ='+req.query.connectionID+';')
        for (let seat of req.query.seats.split(',')){
            var result = await kvery('SELECT free FROM Vehicle_seat WHERE vehicleID='+vehID[0].vehicleID+' AND seat='+seat+';');
            if (result[0].free == 0) fail = 1;
        }
        if (fail == 1){
            res.json({msg: 'ERR'});
        }
        else {
            var resID = await insertKvery('INSERT INTO Reservation(connectionID, passengerID, paid, cost) VALUES ('+req.query.connectionID+', '+req.query.passengerID+', FALSE,'+req.query.cost+')');
            // console.log('MINE I INTO Reservation(connectionID, passengerID, paid, cost) VALUES ('+req.query.connectionID+', '+req.query.passengerID+', FALSE,'+req.query.cost+')')
            // console.log("RESID: ",resID);
            for (let seat of req.query.seats.split(',')){
                console.log('INFOR: INTO Reservation_seat(reservationID, seat) VALUES ('+resID+','+seat+')')
                await kvery('INSERT INTO Reservation_seat(reservationID, seat) VALUES ('+resID+','+seat+')');
                await kvery('UPDATE Vehicle_seat SET free=FALSE WHERE vehicleID='+vehID[0].vehicleID+' AND seat='+seat+';');
            }
            res.json({msg: 'OK'});    
        }
    })

router.route('/free_vehicle_seats_poor')
    .get(async function(req, res) {
        var result = await kvery('SELECT seat FROM Vehicle_seat WHERE free=TRUE AND class=2 AND vehicleID ='+req.query.ID+';');
        res.json(result)
    })

router.route('/free_vehicle_seats_rich')
    .get(async function(req, res) {
        var result = await kvery('SELECT seat FROM Vehicle_seat WHERE free=TRUE AND class=1 AND vehicleID ='+req.query.ID+';');
        res.json(result)
    })

router.route('/reservation_deconfirm')
    .post(async function(req, res) {
        await kvery('UPDATE Reservation SET paid = FALSE WHERE ID ='+req.query.ID+';');
        res.json({msg: "sup"})
    })

router.route('/reservation_confirm')

    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Reservation;');
        res.json(result)
    })

    .post(async function(req, res) {
        await kvery('UPDATE Reservation SET paid = TRUE WHERE ID ='+req.query.ID+';');
        var result = await kvery('SELECT Reservation.passengerID AS RPID, Passenger.registered AS PBOOL FROM Reservation, Passenger WHERE Reservation.passengerID = Passenger.ID AND Reservation.ID ='+req.query.ID+';');
        if (result[0].PBOOL == "FALSE"){
            await kvery('DELETE FROM Passenger WHERE ID ='+result[0].RPID+';');
        }
        res.json({msg: "sup"})
    })
    .delete(async function(req, res) {
        await kvery('DELETE FROM Reservation WHERE ID='+req.query.ID+';');
        res.json({msg: "sup"})
    })

router.route('/test')
    .get(async function(req, res) {
        var result = await kvery('SELECT * FROM Passenger;');
        res.json(result)
    })

    .post(async function(req, res) {
        await kvery('UPDATE Reservation SET paid = TRUE WHERE ID ='+req.query.ID+';');
        var result = await kvery('SELECT Reservation.passengerID AS RPID, Passenger.registered AS PBOOL FROM Reservation, Passenger WHERE Reservation.passengerID = Passenger.ID AND Reservation.ID ='+req.query.ID+';');
        if (result[0].PBOOL == "FALSE"){
            await kvery('DELETE FROM Passenger WHERE ID ='+result[0].RPID+';');
        }
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
