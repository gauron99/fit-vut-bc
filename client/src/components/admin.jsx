import React,{useState, useEffect} from 'react';
import {checkForUsers} from '../services/userControl';
import {getRezs,getVoZas,confirmReservation,deconfirmReservation,destroyReservation} from './edit_crew';

import "../Admin.css"


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
      alert("zajděte se na matriku přejmenovat prosím, uživatele s takovým jménem již v databázi máme");
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
      alert("zajděte se na matriku přejmenovat prosím, uživatele s takovým jménem již v databázi máme");
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
      alert("zajděte se na matriku přejmenovat prosím, uživatele s takovým jménem již v databázi máme");
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

const ManageReservations = (props) =>{
    if(props.trigger === "Rezervace"){
        return (
            <div className="admin_users_config">
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
                  <td><button type='submit' onClick={()=>confirmReservation(comp[0], props.setJizdenkos)} >vyliž</button></td>
                  <td><button type='submit' onClick={()=>deconfirmReservation(comp[0], props.setJizdenkos)} >si</button></td>
                  <td><button type='submit' onClick={()=>destroyReservation(comp[0], props.setJizdenkos)} className="button-show">prdel</button></td>
                  </tr>)}
              </tbody>
            </table>
          </div>
        );
    } else {return "";}
}

const ManageConnections = (props) =>{
    if(props.trigger === "Spoje"){
        return (
            <div>
                henlo spoje
            </div>
        );
    } else {return "";}
}

const ManageStops = (props) =>{
    if(props.trigger === "Zastávky"){
        return (
            <div>
                henlo zastavky
            </div>
        );
    } else {return "";}
}

const ManageVehicles = (props) =>{
    if(props.trigger === "Vozidla"){
        return (
            <div>
                henlo auticka
            </div>
        );
    } else {return "";}
}


export const EditAdminPage = () => {
  const  [users,setUsers] = useState([]);
  const  [usePOP,setPop] = useState([false]);
  const  [heslo,setslo] = useState("");
  const  [addUserB,setAddUserB] = useState(false);
  const  [view,setView] = useState("");

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
    <div className="admin_frame">
      <div className="admin_users_config">
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

      <div className="admin_others_config">

        <div className="">
        <td><select id={"role"} defaultValue="" value={view} onChange={handleChange}>
                <option value="Rezervace">Rezervace</option>
                <option value="Spoje">Spoje</option>
                <option value="Zastávky">Zastávky</option>
                <option value="Vozidla">Vozidla</option>
            </select></td>
        </div>
        <div>            
            <ManageReservations trigger={view} jizdenkos={jizdenkos} vozidlosZastavkos={vozidlosZastavkos} setJizdenkos={setJizdenkos} setVozidlosZastavkos={setVozidlosZastavkos}/>
            <ManageConnections trigger={view}/>
            <ManageStops trigger={view}/>
            <ManageVehicles trigger={view}/></div>
        <div></div>

      </div>
    </div>
  )   
}