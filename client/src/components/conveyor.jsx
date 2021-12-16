import React,{useState,useEffect} from "react";
import {GeneralErrorPage} from "./home";
import {getToken} from "../services/userControl"

import "../People.css"
import "../App.css"

const EditConvStops = () => {
  return (
    null
  )
}



const PopConnWindow = (props) => {
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
        <h3 className="h3reg">Vytvořit nový spoj</h3>
        <form onSubmit={handleNewVehicle}>
          <label htmlFor="desc">Popis</label>
            <input className="register-item" id="desc" type="text" placeholder="popis pro vozidlo"></input>
          
          <label htmlFor="richseats">Cena za 1. třídu</label>
            <input className="register-item" id="richseats" type="text" placeholder="cena pro 1.třídu"></input>
                  
          <label htmlFor="poorseats">Cena za ě. třídu</label>
            <input className="register-item" id="poorseats" type="text" placeholder="cena pro 2.třídu"></input>
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
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
    // nacti vsechny zastavky ve spoji podle spojID
    // if(connViewID !== undefined){
      getAllStopsByConn(connViewID,setStops);
    // }
  },[connViewID]);

  const handleData = (e,num,name,arrival) => {
    console.log("yello printing editing and shuch");
  }
 
  var count = 1;
  return (
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
  );
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
      // update spoje
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

const EditConvConn = () => {
  const [popConn,setPopConn] = useState(false)
  const [showStopsByID,setShowStopsByID] = useState()
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
    setShowStopsByID(id);
  }

  return(
  <div className="connection-main-frame">
    <div className="conveyor-leftside">
      <div className="main-page-vehicle">
        <h3 className="h3reg">Spoje</h3>
        <button class="add-button" onClick={() => setPopConn(!popConn)}>+</button>
        <PopConnWindow trigger={popConn} handleTrigger={setPopConn} setConns={setConns} conns={conns}/>
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
    <EditConn connViewID={showStopsByID} setconnViewID={setShowStopsByID} data={connData} setData={handleData} conns={conns} setConns={setConns} />
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

  console.log("convPAGE:",props.convPage);

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
    // console.log(personel);
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

  // check if name already exists
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

  console.log("convPAGE:",props.convPage);

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
          <th>*</th>
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
              <td><button class="editbtn" onClick={()=> handleData(val.ID,val.name,val.passwd)}>upravit</button></td>
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
        vidim: {props.convPage}
      </div>
      <div>
        
      </div>
    </div>
  )
}

export const EditConveyorPage = () => {
    
  const [convPage,setConvPage] = useState(0);
  console.log("CONVPAGEROOT: ",convPage);

  return (
    <div>
      <EditHeaderPeople convPage={convPage} setConvPage={setConvPage}/>
      <EditConveyorChangeView convPage={convPage} setConvPage={setConvPage}/>
    </div>
  )
}