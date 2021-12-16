import react from "react";
import {getToken, isLoggedIn} from "../services/userControl";
import { EditErrorPage } from "./edit";
import React,{useState, useEffect , Fragment} from 'react';



async function confirmReservation(resID, setJizdenkos){
    await fetch('/api/reservation_confirm?ID='+resID,{
        method: "POST",
    })
    .then((res) => res.json())
    getRezs(setJizdenkos)
}

async function deconfirmReservation(resID, setJizdenkos){
    await fetch('/api/reservation_deconfirm?ID='+resID,{
        method: "POST",
    })
    .then((res) => res.json())
    getRezs(setJizdenkos)
}

async function destroyReservation(resID, setJizdenkos){
    await fetch('/api/reservation_confirm?ID='+resID,{
        method: "DELETE",
    })
    .then((res) => res.json())
    getRezs(setJizdenkos)
}

async function getRezs(setJizdenkos, name, role){
    console.log(name,role)
    fetch('/api/reservation_confirm',{
        method:"GET",
        headers: {
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        },
      })
      .then((res) => res.json())
      .then((result) => {
        var jizdenkz = [];
        for (let jizdenk of result){
            jizdenkz.push(Object.values(jizdenk));
        }
        setJizdenkos(jizdenkz);
    })
}
async function getVoZas(setJizdenkos){

}

export const EditCrewPage = () => {
  // check if crew is connected
  const  [jizdenkos,setJizdenkos] = useState([]);
  const  [vozidlosZastavkos, setVozidlosZastavkos] = useState([]);
  var tokenis = getToken();

  useEffect(() =>{
    getRezs(setJizdenkos);
    getVoZas(setVozidlosZastavkos, tokenis.login, tokenis.type);
  }, []);

  if(isLoggedIn("CREW")){
    return (
      <div className="admin_frame">
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
              {jizdenkos.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}
                <td><button type='submit' onClick={()=>confirmReservation(comp[0], setJizdenkos)} >vyliž</button></td>
                <td><button type='submit' onClick={()=>deconfirmReservation(comp[0], setJizdenkos)} >si</button></td>
                <td><button type='submit' onClick={()=>destroyReservation(comp[0], setJizdenkos)} className="button-show">prdel</button></td>
                </tr>)}
            </tbody>
          </table>
        </div>
  
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
              {jizdenkos.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}</tr>)}
            </tbody>
          </table>
        </div>

        <div className="admin_others_config">
  
          <div className=""></div>
          <div></div>
          <div></div>
  
        </div>
      </div>
    )
  }else {
    return <EditErrorPage />
  }
}