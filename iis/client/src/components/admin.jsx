import React,{useState, useEffect} from 'react';
import {checkForUsers,checkForStops,getToken} from '../services/userControl';
import {getRezs,getVoZas,confirmReservation,deconfirmReservation,destroyReservation} from './edit_crew';
import {PopVehicleWindow,deleteVehicle,getVehicleOptions,getAllStops,EditConn,deleteConnection} from './conveyor';
import "../Admin.css"
import Select from 'react-select';


async function addUser(n,p){
    var boolec = await checkForUsers(n);
   if(boolec){
    await fetch('/api/passenger_manage?name='+n+"&passwd="+p+"&registered=1",{
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
    })
    .then(response => response.json())
    .then((res)=>{
        alert("Uspesne registrovan!");
    })
  }
  else {
      alert("Vyberte prosím jiné jméno, toto již existuje");
  }
}

async function addCrew(n,p,dn){
    var boolec = await checkForUsers(n);
   if(boolec){
    var conID;
    await fetch('/api/getdopravceID?name='+dn,{
        method: "GET",
        headers: { 'Content-Type': 'application/json' },
    })
    .then(response => response.json())
    .then((res)=>{
        conID = res.ID;
    })
    console.log(conID)
    if (conID == 'none'){
        alert("dopravce s timto jmenem neexistuje!")
    }
    else {
        await fetch('/api/crew_manage?name='+n+"&passwd="+p+"&conveyorID="+conID,{
            method: "POST",
            headers: { 'Content-Type': 'application/json' },
        })
        .then(response => response.json())
        .then((res)=>{
            alert("Uspesne registrovan!");
        })
    }
  }
  else {
      alert("uživatele s takovým jménem již v databázi máme");
  }
}

async function addConveyor(n,p){
    var boolec = await checkForUsers(n);
   if(boolec){
    await fetch('/api/conveyor_manage?firm='+n+"&passwd="+p,{
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
    })
    .then(response => response.json())
    .then((res)=>{
        alert("Uspesne registrovan!");
    })
  }
  else {
      alert("Vyberte prosím jiné jméno, toto již existuje");
  }
}

async function addAdmin(n,p){
    var boolec = await checkForUsers(n);
   if(boolec){
    await fetch('/api/admin_manage?name='+n+"&passwd="+p,{
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
    })
    .then(response => response.json())
    .then((res)=>{
        alert("Uspesne registrovan!");
    })
  }
  else {
      alert("Vyberte prosím jiné jméno, toto již existuje");
  }
}

async function updatePerson(name, pwd, id, role){
    var nameName = role === 'Conveyor' ? 'firm' : 'name';
    var boolec = await checkForUsers(name);
    if(boolec){
        await fetch("/api/"+role+"_manage?"+nameName+"="+name+"&passwd="+pwd+'&ID='+id,{
            method: "PUT",
            headers: { 'Content-Type': 'application/json' },
        })
        .then(response => response.json())
    }
    else {alert("NOPE")}
}

async function deletePerson(name, role){
    if(window.confirm("Opravdu chcete smazat uživatele '"+name+"'?")){
        var nameName = role === 'Conveyor' ? 'firm' : 'name';
        
        await fetch("/api/"+role+"_manage?"+nameName+"="+name,{
            method: "DELETE",
            headers: { 'Content-Type': 'application/json' },
        })
        .then(response => response.json())
    }
    else {}
}

async function getHeslo(setslo,userRole,userID) {
    await fetch('/api/getpwd?role='+userRole+'&ID='+userID,{
        method: "GET",
      })
      .then(response => response.json())
      .then(hesl => {
          console.log(hesl)
         setslo(hesl.passwd);
         return hesl.passwd;
      })
}

async function GetVehicles(vehicles,setVehicles) {
    await fetch("/api/getvehicles",{
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

const editOneVehicle = (e,id,rs,ps,desc,cid,trigger,handleTrigger) => {
    const data = e.target;
    e.preventDefault();
    // check if anything has changed at all
    if(data.richseats.value === rs && data.poorseats.value === ps && data.desc.value === desc && data.cid.value === cid){
      handleTrigger(!trigger);
      return;
    }
    // there is something to change
    const rs_upd    = data.richseats.value !== "" ? data.richseats.value : rs;
    const ps_upd    = data.poorseats.value !== "" ? data.poorseats.value : ps;
    const desc_upd  = data.desc.value      !== "" ? data.desc.value      : desc;
    const cid_upd  = data.cid.value      !== "" ? data.cid.value      : cid;

    console.log('CID: '+cid_upd)
    updateVehicle(id,rs_upd,ps_upd,desc_upd,cid_upd,trigger,handleTrigger);
  }

async function updateVehicle(id,rs,ps,desc,cid,trigger,handleTrigger){  
    console.log('/api/vehicle?max_seats_poor='+ps+'&max_seats_rich='+rs+'&description='+desc+'&conveyorID='+cid+'&ID='+id)
    await fetch('/api/vehicle?max_seats_poor='+ps+'&max_seats_rich='+rs+'&description='+desc+'&conveyorID='+cid+'&ID='+id,{
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

const EditMachine = (props) => {
    const data = props.data;
    const setData = props.setData;
    const trigger = props.trigger;
    const handleTrigger = props.handleTrigger;
    //  id:"",max_rich:"",max_poor:"",desc:""
    console.log(data.id,data.max_rich,data.max_poor,data.desc,data.cid);
  
    return trigger ? (
      <div className="popup-window-down">
        <div className="popup-in">
        <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
          <h3 className="h3reg">Uprav Vozidlo</h3>
          <form onSubmit={(e)=>editOneVehicle(e,data.id,data.max_rich,data.max_poor,data.desc,data.cid,trigger,handleTrigger)}>
            <label htmlFor="desc">Popis</label>
              <input className="register-item" id="desc" type="text"defaultValue={data.desc} ></input>
            
            <label htmlFor="richseats">Celkový počet sedadel(1.třída)</label>
              <input className="register-item" id="richseats" type="text" defaultValue={data.max_rich}></input>
                    
            <label htmlFor="poorseats">Celkový počet sedadel(2.třída)</label>
              <input className="register-item" id="poorseats" type="text" defaultValue={data.max_poor}></input>
              
            <label htmlFor="poorseats">ID dopravce</label>
              <input className="register-item" id="cid" type="text" defaultValue={data.cid}></input>

              <hr className="solid" />
            <button type="submit" className="register-item button-login">Upravit</button>
          </form>
          <button type="button" className="register-item button-login" onClick={() => deleteVehicle(data.id,props.vehicles,props.setVehicles,trigger,handleTrigger)}>Smazat</button>
  
        </div>
      </div>
    ) 
    : "";
  }
const PopPeopleWindow = (props) => {
    if (!props.trigger)
        return "";
    else {
        var name = props.userRole === 'conveyor' ? 'firm' : 'jmeno';
        return (
            <div className="popup-window">
            <div className="popup-in">
            <button className="reserve-close-button" onClick={() =>{props.handleTrigger(!props.trigger)}}>X</button>
                <h3 className="h3reg">Uprav uživatele</h3>
                <form>
                <label htmlFor="regn">{name}</label>
                    <input className="register-item" id="meno" type="text" defaultValue={props.userName}></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" key={props.heslo} id="hesloo" type="text" defaultValue={props.heslo}></input>
                        
                    <hr className="solid" />
                <button onClick={() => updatePerson(document.getElementById('meno').value, document.getElementById('hesloo').value, props.userID, props.userRole)} type="submit" className=" register-item button-submit button-login">Potvrdit</button>
                <button onClick={() => deletePerson(document.getElementById('meno').value, props.userRole)} type="submit" className=" register-item button-submit button-login">Smazat uživatele</button>
                </form>
            </div>
            </div>
        );
    }
}

const AddPassenger = (props) => {
    if(props.trigger === "Passenger"){
        return (
            <div>
                <h3 className="h3reg">Vytvoř cestujícího</h3>
                <form>
                <label htmlFor="regn">Jméno</label>
                    <input className="register-item" id="meno" type="text"></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" id="hesloo" type="text"></input>
                        
                    <hr className="solid" />
                <button type="button" onClick={() => addUser(document.getElementById('meno').value, document.getElementById('hesloo').value)} className=" register-item button-submit button-login">Vytvořit</button>
                </form>
            </div>
        );
    } else {return "";}
}

const AddCrew = (props) => {
    if(props.trigger === "Crew"){
        return (
            <div>
                <h3 className="h3reg">Vytvoř zaměstnance dopravce</h3>
                <form>
                <label htmlFor="regn">Jméno</label>
                    <input className="register-item" id="meno" type="text"></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" id="hesloo" type="text"></input>

                <label htmlFor="regp">Jméno dopravce</label>
                    <input className="register-item" id="dopravceName" type="text"></input>
                        
                    <hr className="solid" />
                <button type="button" onClick={() => addCrew(document.getElementById('meno').value, document.getElementById('hesloo').value,document.getElementById('dopravceName').value)} className=" register-item button-submit button-login">Vytvořit</button>
                </form>
            </div>
        );
    } else {return "";}
}

const AddConveyor = (props) => {
    if(props.trigger === "Conveyor"){
        return (
            <div>
                <h3 className="h3reg">Vytvoř dopravce</h3>
                <form>
                <label htmlFor="regn">Název firmy</label>
                    <input className="register-item" id="meno" type="text"></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" id="hesloo" type="text"></input>
                        
                    <hr className="solid" />
                <button type="button" onClick={() => addConveyor(document.getElementById('meno').value, document.getElementById('hesloo').value)} className=" register-item button-submit button-login">Vytvořit</button>
                </form>
            </div>
        );
    } else {return "";}
}

const AddAdmin = (props) => {
    if(props.trigger === "Admin"){
        return (
            <div>
                <h3 className="h3reg">Vytvoř admina</h3>
                <form>
                <label htmlFor="regn">Jméno</label>
                    <input className="register-item" id="meno" type="text"></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" id="hesloo" type="text"></input>
                        
                    <hr className="solid" />
                <button type="button" onClick={() => addAdmin(document.getElementById('meno').value, document.getElementById('hesloo').value)} className=" register-item button-submit button-login">Vytvořit</button>
                </form>
            </div>
        );
    } else {return "";}
}

const AddUser = (props) => {
    const  [role,setRole] = useState("");

    function handleChange(event) {    setRole(event.target.value);  }

    if (!props.trigger)
        return "";
    else {
        return (
            <div className="popup-window">
            <div className="popup-in">
            <button className="reserve-close-button" onClick={() =>{props.handleTrigger(!props.trigger)}}>X</button>
            <td><select id={"role"} defaultValue="" value={role} onChange={handleChange}>
                <option value="Passenger">Passenger</option>
                <option value="Conveyor">Conveyor</option>
                <option value="Crew">Crew</option>
                <option value="Admin">Admin</option>
            </select></td>
            <AddPassenger trigger={role}/>
            <AddCrew trigger={role}/>
            <AddConveyor trigger={role}/>
            <AddAdmin trigger={role}/>
            </div>
            </div>
        );
    }
}

const ManageUsers = (props) => {
  // convert props
  const users = props.users;
  const setUsers = props.setUsers;
  const trigger = props.trigger;
  const addUserB = props.addUserB;
  const setAddUserB = props.setAddUserB;
  const usePOP = props.usePop;
  const setPop = props.setPop;
  const heslo = props.heslo;
  const setslo = props.setslo;

  if(trigger === "Uživatelé"){
    return (

      <div className="admin-item">
      <button type="button" onClick={() => setAddUserB(!addUserB)} className=" register-item button-submit button-login">Vytvořit uživatele</button>
      <AddUser trigger={addUserB} heslo={typeof(usePOP[4]) === 'object' ? heslo : typeof(usePOP[4])} userName={usePOP[3]} userID={usePOP[1] === undefined ? '1' : usePOP[1]} userRole={usePOP[2] === undefined ? 'admin' : usePOP[2]} handleTrigger={setAddUserB} />
      <table className="admin_users_table">
        <thead className="admin_table_head">
          <tr>
            <th>ID</th>
            <th>Jméno</th>
            <th>Účet</th>
            <th>*</th>
          </tr>
        </thead>
        <tbody>
          {users.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}<td><button type='button' onClick={()=>setPop([!usePOP[0],comp[0],comp[2],comp[1],getHeslo(setslo,comp[2],comp[0])])} className="button-show">upravit</button></td></tr>)}
          <PopPeopleWindow trigger={usePOP[0]} heslo={typeof(usePOP[4]) === 'object' ? heslo : typeof(usePOP[4])} userName={usePOP[3]} userID={usePOP[1] === undefined ? '1' : usePOP[1]} userRole={usePOP[2] === undefined ? 'admin' : usePOP[2]} handleTrigger={setPop} />
        </tbody>
      </table>
    </div>
    )  
  } else {return "";}
}

const ManageReservations = (props) =>{
  if(props.trigger === "Rezervace"){
    return (
            <div className="admin-item">
            <table className="admin_users_table">
              <thead className="admin_table_head">
                <tr>
                  <th>ID</th>
                  <th>connectionID</th>
                  <th>passengerID</th>
                  <th>paid</th>
                  <th>cost</th>
                  <th>potvrdit</th>
                  <th>zneplatnit</th>
                  <th>smazat</th>
                </tr>
              </thead>
              <tbody>
                {props.jizdenkos.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}
                  <td><button type='submit' onClick={()=>confirmReservation(comp[0], props.setJizdenkos)} >+</button></td>
                  <td><button type='submit' onClick={()=>deconfirmReservation(comp[0], props.setJizdenkos)} >-</button></td>
                  <td><button type='submit' onClick={()=>destroyReservation(comp[0], props.setJizdenkos)} className="button-show">smazat</button></td>
                  </tr>)}
              </tbody>
            </table>
          </div>
        );
    } else {return "";}
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


async function registerConn(vehID,richprice,poorprice,stops,setviewID,conns,setConns){
  const token = getToken()

  console.log("stop prev post req:",stops)
  await fetch('/api/spoje?conveyorID='+token.id+"&vehicleID="+vehID+"&price_poor="+poorprice+"&price_rich="+richprice,{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
      body: JSON.stringify({zastavky: stops})
  })
  .then(r=>r.json())
  .then(res=>{
    setviewID(undefined);
    GetConnections(conns,setConns)
    
  })
}

// main window for new connection on the right side
const NewConnWindow = (props) => {
  const setviewID = props.setviewID;
  const conns = props.conns;
  const setConns = props.setConns;

  // all available vehicles
  const [vehicleOptions,setVehicleOptions] = useState()

  // show this in dropdown menu
  const [allStops,setAllStops] = useState([])
  // stops already selected, under dropdown menu
  const [selectedStops,setSelectedStops] = useState([])
  

  const  handleNewConn = (event,conns,setConns) => {
    event.preventDefault();
    const data = event.target.elements;

    let vehID = data[0].value.split(' ')[2]
    // skip 3rd entirely
    
    let saveStops = []
    // cycle through all stops and save arrival times and shit
    for(let i=0;i<selectedStops.length;++i){
      let x = {
        name:data[i+4].id,
        cas:data[i+4].value+":00"
      }
      saveStops.push(x)
    }
    // test if vehicle is set
    if(vehID === undefined || vehID === 0){
      alert("Vyber auto prosím!");
      return;
    }

    // test if price for rich is set
    if(data[1].value === "" || (isNaN(data[1].value) || isNaN(parseFloat(data[1].value)))){
      alert("Zadejte počet sedadel pro 1. třídu prosím!(číslo)");
      return;
    }

    // test price for poor seats
    if(data[2].value === "" || (isNaN(data[2].value) || isNaN(parseFloat(data[2].value)))){
      alert("Zadejte počet sedadel pro 2. třídu prosím!(číslo)");
      return;
    }

    if(saveStops.length < 2){
      alert("Zadejte alespon 2 zastavky pro spoj prosím!");
      return;
    }
  
    registerConn(vehID,data[1].value,data[2].value,saveStops,setviewID,conns,setConns);
  }

  useEffect(()=> {
    getVehicleOptions(setVehicleOptions);
    getAllStops(setAllStops,selectedStops);
  },[]);

  const handleSelected = (e) => {
    console.log("aadad:",e)
    setSelectedStops(e)

  }
  
  const handleData = (e,count,name,label) =>{

  }
  let count = 1;
  return (
    <div className="conveyor-rightside">
      <div className="main-page-vehicle">
        <h3 className="h3reg">Vytvoř nový spoj</h3>
        <form onSubmit={(e)=>handleNewConn(e,conns,setConns)}>
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

          <label htmlFor="stops">Vyber zastávky</label>
            <Select
            
              placeholder="Zastávky"
              closeMenuOnSelect={false}
              // isSearchable
              isMulti
              // name="name"
              options={allStops}
              noOptionsMessage={()=> "Zadne zastavky"}
              onChange={handleSelected}
              
            />
          <hr className="solid" />

          <table className="people-table">
            <thead>
              <tr>
                <th>Zastávka</th>
                <th>Příjezd</th>
              </tr>
            </thead>
            <tbody>
              {selectedStops.map(val => {
                return (
                  <tr>
                  <td id={count++}>{val.label}</td>
                  <td><input id={val.label} type="time"></input></td>
                  </tr>
                  )
                })}
            </tbody>
          </table>
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  );
}
const ConnChooseRightView = (props) => {
  if(props.connViewID === undefined){
    return "";
  }
  else if(props.connViewID <= 0){
    return <NewConnWindow setConns={props.setConns} conns={props.conns} setviewID={props.setconnViewID}/>
  }
  else {
    return <EditConn connViewID={props.connViewID} setconnViewID={props.setConnViewID} data={props.data} handleData={props.handleData} conns={props.conns} setConns={props.setConns} />
  }
}

const ManageConnections = (props) =>{
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

  if(props.trigger === "Spoje"){
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
  } else {return "";}
}

async function handleNewStop(name,setStops){
  // check if exists
  if(!checkForStops(name)){
    alert("Takova zastavka jiz existuje");
    return;
  }
  
  await fetch('/api/new_stop&name='+name+"&conveyorID=1",{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(r=>r.json())
  .then(res=>{
    GetStops(setStops);
  })
}

async function GetStops(setStops){
  await fetch('/api/stops_all',{
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

// confirm one stop if not confirmed
async function ConfirmStop(id,name,status,setStops){
  if(status){
    return;
  }

  await fetch("/api/confirm_stop?ID="+id,{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(r=>r.json())
  .then(res=>{

    // update zastavky
    GetStops(setStops);
  })

}

async function handleStopChange(e,given_name,given_confirmed,id,n,confirmed,conveyorID,trigger,handleTrigger){
  e.preventDefault();
  const conf = confirmed.toLowerCase();
  if(conf !== "ano" || conf !== "ne"){
    alert("Pro konfirmaci slouží pouze slova 'ano' či 'ne'");
    return;
  }

  const conf_num = conf === "ano" ? 1 : 0;
  console.log('/api/new_stop?name='+n+'&conveyorID='+conveyorID+'&confirmed='+conf_num+'&ID='+id);
  // await fetch('/api/new_stop?name='+n+'&conveyorID='+conveyorID+'&confirmed='+conf_num+'&ID='+id,{})
  alert("stop");
}

async function delStop(){
  console.log("smaz to")
}

// edit stop for admin view
const EditStop = (props) =>{
  const data = props.data;
  const setData = props.setData;
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  return trigger ? (
    <div className="popup-window-down">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Spravuj zastávku</h3>
        <form onSubmit={(e)=>handleStopChange(e,e.target.regn.value,e.target.conf.value,data.id,data.name,data.confirmed,data.conveyorID,trigger,handleTrigger)}>
          <label htmlFor="regn">Název</label>
            <input className="register-item" id="regn" type="text"defaultValue={data.name} ></input>
          <label htmlFor="conf">Potvrzená?(Ano/Ne)</label>
            <input className="register-item" id="conf" type="text"defaultValue={data.confirmed? "Ano": "Ne"} ></input>
                  
            <hr className="solid" />
          <button type="submit" className="register-item button-login">Upravit</button>
        </form>
          <button type="submit" onClick={()=> delStop(data.name,data.id,)} className="register-item button-login">Smazat</button>

      </div>
    </div>
  ) 
  : "";
}

const NewStop = (props) => {
  const trigger = props.trigger
  const handleTrigger = props.setTrigger
  return trigger ? (
    <div className="popup-window">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Vytvoř novou zastávku</h3>
        <form onSubmit={handleNewStop}>
          <label htmlFor="regn">Jméno</label>
            <input className="register-item" id="regn" type="text" placeholder="jmeno"></input>
          
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
}

const ManageStops = (props) =>{
  const [popProposal,setPopProposal] = useState(false)//for new proposal
  const [popEditStop,setPopEditStop] = useState(false)//for editing existing stop
  const [stops,setStops] = useState([]);
  const [stopData,setStopData] = useState({ID:"",name:"",conveyorID:"",confirmed:""});

  useEffect(()=>{
    GetStops(setStops);
  },[])

  const handleData = (id,n,cid,conf) => {
    console.log(id,n,cid,conf);
    setStopData(()=>({
      ID:id,
      name:n,
      conveyorID:cid,
      confirmed:conf
    }));
    setPopEditStop(!popEditStop);}


    if(props.trigger === "Zastávky"){
      return(
        <div className="main-page-people">
          <button class="add-button" onClick={() => setPopProposal(!popProposal)}>Nová zastávka</button>
          <NewStop trigger={popProposal} handleTrigger={setPopProposal} setStops={setStops} stops={stops}/>
          <EditStop trigger={popEditStop} handleTrigger={setPopEditStop} data={stopData} setData={handleData} setStops={setStops} stops={stops}/>
      
          <table className="people-table">
            <thead>
              <tr>
                <th>ID</th>
                <th>Jméno</th>
                <th>DopravceID</th>
                <th>Potvrzené</th>
                <th className="">Potvrzení</th>
                <th className="">Upravit</th>
              </tr>
            </thead>
            <tbody>
              {stops.map(val => {
                return (
                  <tr>
                    <td>{val.ID}</td>
                    <td>{val.name}</td>
                    <td>{val.conveyorID}</td>
                    <td>{val.confirmed? "Ano" : "Ne"}</td>
                    <td><button class="editbtn" onClick={()=> ConfirmStop(val.ID,val.name,val.confirmed,setStops)}>potvrdit</button></td>
                    <td><button class="editbtn" onClick={()=> handleData(val.ID,val.name,val.conveyorID,val.confirmed)}>úprava</button></td>
                  </tr>
                )
              })}
            </tbody>
          </table>
        </div>
        )
    } else {return "";}
}

const ManageVehicles = (props) =>{
    const [popVehicle,setPopVehicle] = useState(false)
    const [popEditVehicle,setPopEditVehicle] = useState(false)
    const [vehicles,setVehicles] = useState([]);
    const [vehicleData,setVehicleData] = useState({id:"",max_rich:"",max_poor:"",desc:"",conveyorID: ""});

    useEffect(()=>{
        GetVehicles(vehicles,setVehicles);
      },[])
    const handleData = (id,desc,mr,mp,cid) => {
    console.log(id,desc,mr,mp,cid);
    setVehicleData(()=>({
        id:id,
        max_rich:mr,
        max_poor:mp,
        desc:desc,
        cid:cid
    }));
    setPopEditVehicle(!popEditVehicle);
    }

    if(props.trigger === "Vozidla"){
        return ( 
            <div className="main-page-vehicle-admin">
            <button class="add-button" onClick={() => setPopVehicle(!popVehicle)}>+</button>
            <PopVehicleWindow trigger={popVehicle} handleTrigger={setPopVehicle} setVehicles={setVehicles} vehicles={vehicles}/>
            <EditMachine trigger={popEditVehicle} handleTrigger={setPopEditVehicle} data={vehicleData} setData={handleData} vehicles={vehicles} setVehicles={setVehicles}/>
            <table className="people-table">
              <thead>
                <tr>
                  <th>ID</th>
                  <th>Popis</th>
                  <th>ID vlastníka</th>
                  <th>sedadla 1. tř.</th>
                  <th>sedadla 2. tř.</th>
                  <th>*</th>
                </tr>
              </thead>
              <tbody>
                {vehicles.map(val => {
                  return (
                    <tr>
                      <td>{val.ID}</td>
                      <td>{val.description}</td>
                      <td>{val.conveyorID}</td>
                      <td>{val.max_seats_rich}</td>
                      <td>{val.max_seats_poor}</td>
                      {/* <td>{val.last_visited}</td> */}
                      <td><button class="editbtn" onClick={()=> handleData(val.ID,val.description,val.max_seats_rich,val.max_seats_poor,val.conveyorID)}>upravit</button></td>
                    </tr>
                  )
                })}
              </tbody>
            </table>
          </div>
        );
    } else {return "";}
}


export const EditAdminPage = () => {
  const  [users,setUsers] = useState([]);
  const  [usePOP,setPop] = useState([false]);
  const  [heslo,setslo] = useState("");
  const  [addUserB,setAddUserB] = useState(false);
  const  [view,setView] = useState("Uživatelé"); //default view of people

  const  [jizdenkos,setJizdenkos] = useState([]);
  const  [vozidlosZastavkos, setVozidlosZastavkos] = useState([]);

  function handleChange(event) {    setView(event.target.value);  }

  useEffect(() =>{
    getRezs(setJizdenkos);
    getVoZas(setVozidlosZastavkos);
    fetch('/api/read_and_subsequently_possibly_config_if_desired_or_not_if_not_necessary_or_not_desired',{
      method:"GET",
      headers: {
        'Content-Type': 'application/json',
        'Accept': 'application/json'
      },
    })
    .then((res) => res.json())
    .then((result) => {
        var lidicky = [];
        for (let lidicka of result){
            lidicky.push(Object.values(lidicka));
        }
      setUsers(lidicky);

    });
  }, []);

  console.log(users); //works

  return (
    <div className="admin-page">
      <div className="admin-header">
        <td>
          <select id={"role"} defaultValue="Uživatelé" value={view} onChange={handleChange}>
            <option value="Uživatelé">Uživatelé</option>
            <option value="Rezervace">Rezervace</option>
            <option value="Spoje">Spoje</option>
            <option value="Zastávky">Zastávky</option>
            <option value="Vozidla">Vozidla</option>
          </select>
        </td>
      </div>
      <hr className="solid" />

        <ManageUsers trigger={view} users={users} setUsers={setUsers} usePop={usePOP} setPop={setPop} heslo={heslo} setslo={setslo} addUserB={addUserB} setAddUserB={setAddUserB} />
        <ManageReservations trigger={view} jizdenkos={jizdenkos} vozidlosZastavkos={vozidlosZastavkos} setJizdenkos={setJizdenkos} setVozidlosZastavkos={setVozidlosZastavkos}/>
        <ManageConnections trigger={view}/>
        <ManageStops trigger={view}/>
        <ManageVehicles trigger={view}/>
    </div>
  )   
}