import React,{useState,useEffect} from "react";
import {GeneralErrorPage} from "./home";
import {getToken, checkForUsers} from "../services/userControl"

import "../People.css"
import "../App.css"



async function GetStops(setStops){
  await fetch('/api/stops_all_confirmed',{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(r => r.json())
  .then(res => {
    setStops(res)
  })
}

// new 'stop proposal' popup window
const PopStopsWindow = (props) => {
  console.log("novy zastavis")
  return (null)

}

// 'proposal for existing stops' popup window
const EditStop = (props) => {
  console.log("edituj")
  return (null)
}

const EditConvStops = () => {
  const [popProposal,setPopProposal] = useState(false)//for new proposal
  const [popEditStop,setPopEditStop] = useState(false)//for editing existing stop
  const [stops,setStops] = useState([]);
  const [stopData,setStopData] = useState({ID:"",name:"",conveyorID:"",confirmed:""});

  useEffect(()=>{
    GetStops(setStops);
  },[])

  const handleData = (id,n,cid) => {
    console.log(id,n,cid);
    setStopData(()=>({
      ID:id,
      name:n,
      conveyorID:cid,
    }));
    setPopProposal(!popProposal);
  }

  return(
  <div className="main-page-people">
    <button class="add-button" onClick={() => setPopProposal(!popProposal)}>Nový návrh</button>
    <PopStopsWindow trigger={popProposal} handleTrigger={setPopProposal} setStops={setStops} stops={stops}/>
    <EditStop trigger={popEditStop} handleTrigger={setPopEditStop} data={stopData} setData={handleData} setStops={setStops} stops={stops}/>
    <table className="people-table">
      <thead>
        <tr>
          <th>ID</th>
          <th>Jméno</th>
          <th>Návrh vytvořil(DopravceID)</th>
          <th className="float-right">*</th>
        </tr>
      </thead>
      <tbody>
        {stops.map(val => {
          return (
            <tr>
              <td>{val.ID}</td>
              <td>{val.name}</td>
              <td>{val.conveyorID}</td>
              <td><button class="editbtn float-right" onClick={()=> handleData(val.ID,val.name,val.conveyorID)}>úprava</button></td>
            </tr>
          )
        })}
      </tbody>
    </table>
  </div>
  )
}


async function registerConn(richseats,poorseats,handleTrigger,trigger,setviewID){

}

// get all stops from db
async function getAllStops(setAllStops,selected){
  await fetch('/api/stops_all_confirmed',{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(r => r.json())
  .then(res => {
    console.log("yastavky: ",res);
    // set all stops
    let tmp = res.map((item,i)=> {
      let checker = 0;
      for(let i = 0; i < selected.length;++i){
        if(item.name === selected[i].name){
          checker = 1;
          break;
        }
      }
      if(checker){
        return ""
      } else {
        return (<option key={item.ID}>{item.ID}: {item.name}</option>)
      }
    })
    // or exclude ones already selected?
    console.log(tmp)
    setAllStops(tmp)
  })
}

// get all available vehicles from db
async function getVehicleOptions(setVehicleOptions){
  await fetch('/api/available_vehicles',{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(r => r.json())
  .then(res=>{
    console.log(res);

    let tmp = res.map((item,i)=>{
      return (
        <option key={item.ID}>Vozidlo ID: {item.ID}</option>
      )
    })
    setVehicleOptions(tmp)
  })
};



// main window for new connection on the right side
const NewConnWindow = (props) => {
  const setviewID = props.setviewID;
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  // all available vehicles
  const [vehicleOptions,setVehicleOptions] = useState()

  // stops already selected, under 
  const [selectedStops,setSelectedStops] = useState([])
  const [allStops,setAllStops] = useState([])


  const addZastavka = (e) => {
    console.log("lego noew zastavka: ",e.target.value,e.target.name);
    setSelectedStops((prev)=>[
      ...prev,e.target.name
    ])
  }
  

  const  handleNewConn = (event) => {
    event.preventDefault();
    const data = event.target;
    
    // max seats not given
    if( data.richseats.value === ""){
      alert("Zadejte počet sedadel pro 1. třídu prosím!");
      return;
    }
    // if is not a number
    if((isNaN(data.richseats.value) || isNaN(parseFloat(data.richseats.value)))){
      alert("Chyba! počet sedadel může být pouze číslo(1.třída)")
      return;
    }

    // check if passwords match
    if(data.poorseats.value === ""){
      alert("Zadejte počet sedadel pro 2. třídu prosím!");
      return;
    }
    // check if not a number
    if((isNaN(data.poorseats.value) || isNaN(parseFloat(data.poorseats.value)))){
      alert("Chyba! počet sedadel může být pouze číslo(2.třída)")
      return;
    }

    registerConn(data.richseats.value,data.poorseats.value,handleTrigger,trigger,setviewID);
  }


  useEffect(()=> {
    getVehicleOptions(setVehicleOptions);
    getAllStops(setAllStops,selectedStops);
  },[]);

  var count = 1;
  return (
    <div className="conveyor-rightside">
      <div className="main-page-vehicle">
        <h3 className="h3reg">Vytvoř nový spoj</h3>
        <form onSubmit={handleNewConn}>
          <label htmlFor="vehID">Vyber dostupné vozidlo</label>
          <div className="conveyor-conn-select">
            <select id="vehID">
                {vehicleOptions}
            </select>
          </div>
          <label htmlFor="richseats">Cena za 1. třídu</label>
            <input className="register-item" id="richseats" type="text" placeholder="cena pro 1.třídu"></input>
                  
          <label htmlFor="poorseats">Cena za 2. třídu</label>
            <input className="register-item" id="poorseats" type="text" placeholder="cena pro 2.třídu"></input>
            <hr className="solid" />

          <label htmlFor="stops">Zastávky</label>
            <select onChange={e => addZastavka(e)}>
              {allStops}
            </select>
          <table className="people-table">
            <thead>
              <tr>
                <th>#</th>
                <th>Zastávka</th>
                <th>Příjezd</th>
                <th>*</th>
              </tr>
            </thead>
            <tbody>
              {/* {stops.map(val => {
                
                return (
                  <tr>
                  <td>{count++}</td>
                  <td>{val.name}</td>
                  <td>{val.arrival}</td>
                  <td><button class="editbtn" onClick={(e)=>handleData(e,val.count,val.name,val.arrival)}>upravit</button></td>
                  </tr>
                  )
                })} */}
            </tbody>
          </table>
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  );
}

async function getAllStopsByConn(connID,setStops){
  console.log('/api/stops_by_conn?connID='+connID);

  await fetch('/api/stops_by_conn?connID='+connID,{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then((result) => result.json())
  .then((res) => {
    console.log("zastavky got: ",res);
    setStops(res);
  })
}

const EditConn = (props) => {
  const connViewID = props.connViewID;
  const data = props.data;
  const setData = props.setData;
  const [stops,setStops] = useState([])

  useEffect(()=> {
    // laod all stops in connection by connViewID
      getAllStopsByConn(connViewID,setStops);
    // }
  },[connViewID]);

  const handleData = (e,num,name,arrival) => {
    console.log("yello printing editing and shuch");
  }
  var count = 1;
  return connViewID !== undefined ? (
    <div className="conveyor-rightside">
       <div className="main-page-vehicle">

      <h3 className="h3reg">Spoj {connViewID}</h3>
      <table className="people-table">
          <thead>
            <tr>
              <th>#</th>
              <th>Zastávka</th>
              <th>Příjezd</th>
              <th>*</th>
            </tr>
          </thead>
          <tbody>
            {stops.map(val => {
            
              return (
                <tr>
                  <td>{count++}</td>
                  <td>{val.name}</td>
                  <td>{val.arrival}</td>
                  <td><button class="editbtn" onClick={(e)=>handleData(e,val.count,val.name,val.arrival)}>upravit</button></td>
                </tr>
              )
            })}
          </tbody>
        </table>
        <button>Přidat zastávku</button>
       </div>
    </div>
  ): "";
}

async function deleteConnection(id,conns,setConns){
  if(window.confirm("Chcete smazat spoj s ID "+id+" ?")){
    await fetch('api/',{
      method: "DELETE",
      headers: {
        'Content-Type': 'application/json',
        },
    })
    .then(r => r.json())
    .then(res => {
      // update connections
      GetConnections(conns,setConns)
    })
  }else{
    // do nothing
  }
}

async function GetConnections(conns,setConns){
  const token = getToken();
  await fetch("/api/spoj_conveyor?conveyorID="+token.id,{
    method: "GET",
    headers: {
    'Content-Type': 'application/json',
    'Accept': 'application/json'
    },
   })
  .then((res) => res.json())
  .then((result) => {
    setConns(result);
  })
}



const ConnChooseRightView = (props) => {
  if(props.connViewID === undefined){
    return "";
  }
  else if(props.connViewID <= 0){
    return <NewConnWindow trigger={props.popConn} handleTrigger={props.setPopConn} setConns={props.setConns} conns={props.conns} setviewID={props.setShowStopsByID}/>
  }
  else {
    return <EditConn connViewID={props.connViewID} setconnViewID={props.setConnViewID} data={props.data} handleData={props.handleData} conns={props.conns} setConns={props.setConns} />
  }
}

const EditConvConn = () => {
  const [popConn,setPopConn] = useState(false)
  const [connViewID,setConnViewID] = useState()
  const [conns,setConns] = useState([]);
  const [connData,setConnData] = useState({id:"",vehID:"",price_poor:"",price_rich:""});

  useEffect(()=>{
    GetConnections(conns,setConns);
  },[])

  const handleData = (e,id,vehicleID,pp,pr) => {
    console.log(id,vehicleID,pp,pr);
    setConnData(()=>({
      id:id,
      vehID:vehicleID,
      price_poor:pp,
      price_rich:pr
    }));
    setConnViewID(id);
  }

  return(
  <div className="connection-main-frame">
    <div className="conveyor-leftside">
      <div className="main-page-vehicle">
        <h3 className="h3reg">Spoje</h3>
        <button class="add-button" onClick={() => setConnViewID(-1)}>+</button>
        <table className="people-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>VůzID</th>
              <th>1. tř.(Kč)</th>
              <th>2. tř.(Kč)</th>
              <th></th>
            </tr>
          </thead>
          <tbody className="people-table-items">
            {conns.map(val => {
              return (
                <tr onClick={(e)=>handleData(e,val.ID,val.vehicleID,val.price_rich,val.price_poor)}>
                  <td>{val.ID}</td>
                  <td>{val.vehicleID}</td>
                  <td>{val.price_rich}</td>
                  <td>{val.price_poor}</td>
                  <td><button class="editbtn" onClick={()=> deleteConnection(val.ID,conns,setConns)}>-</button></td>
                </tr>
              )
            })}
          </tbody>
        </table>
      </div>
    </div>
    <ConnChooseRightView connViewID={connViewID} setconnViewID={setConnViewID} data={connData} handleData={handleData} conns={conns} setConns={setConns} />
  </div>
  )
}


async function deleteVehicle(id,vehicles,setVehicles,tr,setTr){
  console.log("id,vehicles:: ",id,vehicles);
  
  // check if user confirms deletion
  if(window.confirm("Opravdu chcete smazat vozidlo s ID "+id+" ?")){
    await fetch("/api/vehicle?ID="+id,{
      method: "DELETE",
      headers: {
        'Content-Type': 'application/json',
        },
    })
    .then(result => result.json())
    .then(res => {
      GetVehicles(vehicles,setVehicles);
      setTr(false);
    })
  } else {
  // do nothing when not confirmed
  }
}

async function updateVehicle(id,rs,ps,desc,trigger,handleTrigger){  
  const token = getToken()
  await fetch('/api/vehicle?max_seats_poor='+ps+'&max_seats_rich='+rs+'&description='+desc+'&conveyorID='+token.id+'&ID='+id,{
    method: "PUT",
    headers: {
      'Content-Type': 'application/json',
      },
  })
  .then(response => response.json())
  .then((res)=>{
    alert("Úspěšně změněno!");
  })
}

const editOneVehicle = (e,id,rs,ps,desc,trigger,handleTrigger) => {
  const data = e.target;

  // check if anything has changed at all
  if(data.richseats.value === rs && data.poorseats.value === ps && data.desc.value === desc){
    handleTrigger(!trigger);
    return;
  }
  // there is something to change
  const rs_upd    = data.richseats.value !== "" ? data.richseats.value : rs;
  const ps_upd    = data.poorseats.value !== "" ? data.poorseats.value : ps;
  const desc_upd  = data.desc.value      !== "" ? data.desc.value      : desc;

  updateVehicle(id,rs_upd,ps_upd,desc_upd,trigger,handleTrigger);
}

// popup edit window for machines
const EditMachine = (props) => {
  const data = props.data;
  const setData = props.setData;
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;
  //  id:"",max_rich:"",max_poor:"",desc:""
  console.log(data.id,data.max_rich,data.max_poor,data.desc);

  return trigger ? (
    <div className="popup-window-down">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Uprav Vozidlo</h3>
        <form onSubmit={(e)=>editOneVehicle(e,data.id,data.max_rich,data.max_poor,data.desc,trigger,handleTrigger)}>
          <label htmlFor="desc">Popis</label>
            <input className="register-item" id="desc" type="text"defaultValue={data.desc} ></input>
          
          <label htmlFor="richseats">Celkový počet sedadel(1.třída)</label>
            <input className="register-item" id="richseats" type="text" defaultValue={data.max_rich}></input>
                  
          <label htmlFor="poorseats">Celkový počet sedadel(2.třída)</label>
            <input className="register-item" id="poorseats" type="text" defaultValue={data.max_poor}></input>
            <hr className="solid" />
          <button type="submit" className="register-item button-login">Upravit</button>
        </form>
        <button type="button" className="register-item button-login" onClick={() => deleteVehicle(data.id,props.vehicles,props.setVehicles,trigger,handleTrigger)}>Smazat</button>

      </div>
    </div>
  ) 
  : "";
}

async function registerVehicle(desc,richseats,poorseats,handleTrigger,trigger,vehicles,setVehicles){
  const token = getToken();
  
  console.log("/api/vehicle?conveyorID="+token.id+"&max_seats_rich="+richseats+"&max_seats_poor="+poorseats+"&description="+desc+""+"&last_visited=NULL")
  await fetch("/api/vehicle?conveyorID="+token.id+"&max_seats_rich="+richseats+"&max_seats_poor="+poorseats+"&description="+desc+""+"&last_visited=NULL",{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
    }
  })
  .then(response => response.json())
  .then((res)=>{
    handleTrigger(!trigger);
    GetVehicles(vehicles,setVehicles)
  })
}


const PopVehicleWindow = (props) => {
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  const  handleNewVehicle = (event) => {
    event.preventDefault();
    const data = event.target;
    // desc not given
    if( data.desc.value === ""){
      alert("Zadejte popis prosim!");
      return;
    }
    // max seats not given
    if( data.richseats.value === ""){
      alert("Zadejte pocet sedadel pro 1. třídu prosím!");
      return;
    }
    // check if passwords match
    if(data.poorseats.value === ""){
      alert("Zadejte pocet sedadel pro 2. třídu prosím!");
      return;
    }
    // check if crew exists inside registerCrew
    registerVehicle(data.desc.value,data.richseats.value,data.poorseats.value,handleTrigger,trigger,props.vehicles,props.setVehicles);
  }

  return trigger ? (
    <div className="popup-window">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Vytvořit nové vozidlo</h3>
        <form onSubmit={handleNewVehicle}>
          <label htmlFor="desc">Popis</label>
            <input className="register-item" id="desc" type="text" placeholder="popis pro vozidlo"></input>
          
          <label htmlFor="richseats">Celkový počet sedadel(1.třída)</label>
            <input className="register-item" id="richseats" type="text" placeholder="pocet sedadel pro 1.třídu"></input>
                  
          <label htmlFor="poorseats">Celkový počet sedadel(2.třída)</label>
            <input className="register-item" id="poorseats" type="text" placeholder="pocet sedadel pro 2.třídu"></input>
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
}

// fetch all mechines by conveyorID
async function GetVehicles(vehicles,setVehicles) {
  const token = getToken();
  await fetch("/api/conveyor_vehicle?conveyorID="+token.id,{
    method: "GET",
    headers: {
    'Content-Type': 'application/json',
    'Accept': 'application/json'
    },
   })
  .then((res) => res.json())
  .then((result) => {
    setVehicles(result);
  })
}

const EditConvMachine = (props) => {
  const [popVehicle,setPopVehicle] = useState(false)
  const [popEditVehicle,setPopEditVehicle] = useState(false)
  const [vehicles,setVehicles] = useState([]);
  const [vehicleData,setVehicleData] = useState({id:"",max_rich:"",max_poor:"",desc:""});


  useEffect(()=>{
    GetVehicles(vehicles,setVehicles);
  },[])

  const handleData = (id,desc,mr,mp) => {
    console.log(id,desc,mr,mp);
    setVehicleData(()=>({
      id:id,
      max_rich:mr,
      max_poor:mp,
      desc:desc
    }));
    setPopEditVehicle(!popEditVehicle);
  }



  return(
  <div className="main-page-vehicle">
    <button class="add-button" onClick={() => setPopVehicle(!popVehicle)}>+</button>
    <PopVehicleWindow trigger={popVehicle} handleTrigger={setPopVehicle} setVehicles={setVehicles} vehicles={vehicles}/>
    <EditMachine trigger={popEditVehicle} handleTrigger={setPopEditVehicle} data={vehicleData} setData={handleData} vehicles={vehicles} setVehicles={setVehicles}/>
    <table className="people-table">
      <thead>
        <tr>
          <th>ID</th>
          <th>Popis</th>
          <th>sedadla 1. tř.</th>
          <th>sedadla 2. tř.</th>
          {/* <th>Naposledy v zastávce</th> */}
          <th>*</th>
        </tr>
      </thead>
      <tbody>
        {vehicles.map(val => {
          return (
            <tr>
              <td>{val.ID}</td>
              <td>{val.description}</td>
              <td>{val.max_seats_rich}</td>
              <td>{val.max_seats_poor}</td>
              {/* <td>{val.last_visited}</td> */}
              <td><button class="editbtn" onClick={()=> handleData(val.ID,val.description,val.max_seats_rich,val.max_seats_poor)}>upravit</button></td>
            </tr>
          )
        })}
      </tbody>
    </table>
  </div>
  )
}

async function updatePersonel(n,p,id){
  var boolec = checkForUsers(n);
  if(boolec){
await fetch('/api/crew_manage?name='+n+'&passwd='+p+'&ID='+id,{
  method: "PUT",
  headers: {
    'Content-Type': 'application/json',
    },
})
.then(response => response.json())
.then((res)=>{
  alert("Úspěšně změněno!");
})
  }
  else {alert("Nelze přidat, jméno již existuje")}
}


const editPersonel = (e,prevname,prevpasswd,id) => {
  const data = e.target;
  console.log(data.regn.value,data.regp.value);
  console.log(prevname,prevpasswd);

  // nothing was changed, do nothing
  if(prevname === data.regn.value && prevpasswd === data.regp.value){
  } else {
    updatePersonel(data.regn.value,data.regp.value,id);
  }
}

async function GetPeople(personel,setPersonel) {
  const token = getToken();
  await fetch("/api/crew_manage?firm="+token.login,{
    method: "GET",
    headers: {
    'Content-Type': 'application/json',
    'Accept': 'application/json'
    },
   })
  .then((res) => res.json())
  .then((result) => {
    setPersonel(result)
  })
}

async function addNewCrew(n,p,handleTrigger,trigger,token,pers,sPers){
  await fetch("/api/crew_manage?name="+n+"&passwd="+p+"&conveyorID="+token.id,{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
    }
  })
  .then(response => response.json())
  .then((res)=>{
    handleTrigger(!trigger);
    GetPeople(pers,sPers)
  })
}

async function registerCrew(n,p,handleTrigger,trigger,pers,sPers) {
  const token = getToken();
  var boolec = await checkForUsers(n);

  // check if name already exists
    if (boolec){
  await fetch("/api/crew_manage?firm="+token.login,{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(result => result.json())
  .then(res => {
    for(let x of res){
      if(x.name === n){
        alert("Jmeno jiz existuje!");
        return;
      }
    }
    // its new crew! create him in database
    addNewCrew(n,p,handleTrigger,trigger,token,pers,sPers);
  })
    }
    else {alert("tohoto člověka bohužel zaměstnat nemůžete, uživatele s takovým jménem již v databázi máme");}
}

const PopPeopleWindow = (props) => {
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  const  handleNewCrew = (event) => {
    event.preventDefault();
    const data = event.target;
    // name not given
    if( data.regn.value === ""){
      alert("Zadejte jmeno prosim!");
      return;
    }
    // passwd not given
    if( data.regp.value === ""){
      alert("Zadejte heslo prosím!");
      return;
    }
    // check if passwords match
    if(data.regp.value !== data.regpp.value){
      alert("Hesla nejsou stejna!");
      return;
    }
    // check if crew exists inside registerCrew
    registerCrew(data.regn.value,data.regp.value,handleTrigger,trigger,props.personel,props.setPersonel);
  }

  return trigger ? (
    <div className="popup-window">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Vytvoř účet pro personál</h3>
        <form onSubmit={handleNewCrew}>
          <label htmlFor="regn">Jméno</label>
            <input className="register-item" id="regn" type="text" placeholder="jmeno"></input>
          
          <label htmlFor="regp">Heslo</label>
            <input className="register-item" id="regp" type="password" placeholder="heslo"></input>
                  
          <label htmlFor="regpp">Heslo zopakovat</label>
            <input className="register-item" id="regpp" type="password" placeholder="heslo"></input>
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
}


async function deletePersonel(id,name,personel,setPersonel,trigger,handleTrigger){
  if(window.confirm("Opravdu chcete smazat personál '"+name+"' (ID:"+id+") ?")){
    await fetch("/api/crew_manage?ID="+id,{
      method: "DELETE",
      headers: {
        'Content-Type': 'application/json',
        },
    })
    .then(result => result.json())
    .then(res => {
      GetPeople(personel,setPersonel);
      handleTrigger(false);
    })
  } else {
  // do nothing when not confirmed
  }
}


const EditPerson = (props) => {
  const data = props.data;
  const setData = props.setData;
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;
  console.log(data.name,data.passwd,data.id)

  return trigger ? (
    <div className="popup-window-down">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Uprav personál</h3>
        <form onSubmit={(e) => editPersonel(e,data.name,data.passwd,data.id)}>
          <label htmlFor="regn">Jméno</label>
            <input className="register-item" id="regn" type="text" defaultValue={data.name}></input>
          
          <label htmlFor="regp">Heslo</label>
            <input className="register-item" id="regp" defaultValue={data.passwd}></input>
                
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Upravit</button>
        </form>
        <button type="button" className="register-item button-submit button-login" onClick={() => deletePersonel(data.id,data.name,props.personel,props.setPersonel,trigger,handleTrigger)}>Smazat</button>
      </div>
    </div>
  ) 
  : "";
}

const EditConvPerson = (props) => {
  const [popPeople,setPopPeople] = useState(false)
  const [popEditPerson,setPopEditPerson] = useState(false)
  const [personel,setPersonel] = useState([]);
  const [personData,setPersonData] = useState({name:"",passwd:"",id:""});


  useEffect(()=>{
    GetPeople(personel,setPersonel);
  },[])

  // const handlePop = () => {
    // console.log("handluju popis",popPeople)
    // setPopPeople(prev => ({
    //   popPeople: !prev.popPeople
    // }))
  // }

  const handleData = (id,n,p,e) => {
    console.log(id,n,p,e);
    setPersonData(()=>({
      name:n,
      passwd:p,
      id:id
    }));
    setPopEditPerson(!popEditPerson);
  }



  return(
  <div className="main-page-people">
    <button class="add-button" onClick={() => setPopPeople(!popPeople)}>+</button>
    <PopPeopleWindow trigger={popPeople} handleTrigger={setPopPeople} setPersonel={setPersonel} personel={personel}/>
    <EditPerson trigger={popEditPerson} handleTrigger={setPopEditPerson} data={personData} setData={handleData} setPersonel={setPersonel} personel={personel}/>
    <table className="people-table">
      <thead>
        <tr>
          <th>ID</th>
          <th>Jméno</th>
          <th className="float-right">*</th>
        </tr>
      </thead>
      <tbody>
        {personel.map(val => {
          // console.log("VAL: ",val,val.ID);
          return (
            <tr>
              <td>{val.ID}</td>
              <td>{val.name}</td>
              {/* <td>{val.passwd}</td> */}
              <td><button class="editbtn float-right" onClick={()=> handleData(val.ID,val.name,val.passwd)}>upravit</button></td>
            </tr>
          )
        })}
      </tbody>
    </table>
  </div>
  )
}

const EditConveyorChangeView = (props) => {
  if(props.convPage === 0){
    return <EditConvPerson convPage={props.convPage}/>
  } else if(props.convPage===1) {
    return <EditConvMachine/>
  } else if(props.convPage===2) {
    return <EditConvConn/>
  } else if(props.convPage===3) {
    return <EditConvStops/>
  } else {
    return <GeneralErrorPage />
  }
}

const EditHeaderPeople = (props) => {
  
  return (
    <div>
      <div className="header-people">
        <button onClick={() => props.setConvPage(0)} className="btn-ppl">Personál</button>
        <button onClick={() => props.setConvPage(1)} className="btn-ppl">Vozidla</button>
        <button onClick={() => props.setConvPage(2)} className="btn-ppl">Spoje</button>
        <button onClick={() => props.setConvPage(3)} className="btn-ppl">Zastávky</button>
      </div>
      <div>
        
      </div>
    </div>
  )
}

export const EditConveyorPage = () => {
    
  const [convPage,setConvPage] = useState(0);

  return (
    <div>
      <EditHeaderPeople convPage={convPage} setConvPage={setConvPage}/>
      <EditConveyorChangeView convPage={convPage} setConvPage={setConvPage}/>
    </div>
  )
}