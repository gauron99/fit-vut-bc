import react from "react";
import {getToken, isLoggedIn} from "../services/userControl";
import { EditErrorPage } from "./edit";
import React,{useState, useEffect , Fragment} from 'react';
import {checkForStops} from '../services/userControl';



export async function confirmReservation(resID, setJizdenkos){
    await fetch('/api/reservation_confirm?ID='+resID,{
        method: "POST",
    })
    .then((res) => res.json())
    getRezs(setJizdenkos)
}

export async function deconfirmReservation(resID, setJizdenkos){
    await fetch('/api/reservation_deconfirm?ID='+resID,{
        method: "POST",
    })
    .then((res) => res.json())
    getRezs(setJizdenkos)
}

export async function destroyReservation(resID, setJizdenkos){
    if(window.confirm("Opravdu chcete uplně vymazat rezervaci s ID "+resID+" z databáze?")){
        await fetch('/api/reservation_confirm?ID='+resID,{
            method: "DELETE",
        })
        .then((res) => res.json())
        getRezs(setJizdenkos)
    }
    else {}
}

export async function getRezs(setJizdenkos){
    await fetch('/api/reservation_confirm',{
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

export async function getVoZas(setVozidlosZastavkos){
    var tokenis = getToken();
    var name = tokenis.login;
    await fetch('/api/ultimate_getfucked?name='+name,{
        method:"GET",
        headers: {
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        },
      })
      .then((res) => res.json())
      .then((result) => {
        var fuha = [];
        var klicky = Object.keys(result); 
        for (let klic of klicky){
            let temp = [klic];
            for(let zast of result[klic]){
                temp.push(zast['stopName']);
            }
            fuha.push(temp);
        }
        console.log(fuha);
        setVozidlosZastavkos(fuha);
    })

}

async function updateStop(id,stop){
    await fetch('/api/gde_spoj?connID='+id+'&stopName='+stop,{
        method: "POST",
    })
    .then((res) => res.json())
}

export const EditCrewPage = () => {
  // check if crew is connected
  const  [jizdenkos,setJizdenkos] = useState([]);
  const  [vozidlosZastavkos, setVozidlosZastavkos] = useState([]);

  useEffect(() =>{
    getRezs(setJizdenkos);
    getVoZas(setVozidlosZastavkos);
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
            {vozidlosZastavkos.map(comp => 
            <table className="admin_users_table">
                <thead className="admin_table_head">
                    <tr><th>Linka - {comp[0]}</th></tr>
                </thead>
                <tbody>
                    <td><select id={"selekty-"+comp[0]}>
                        {comp.slice(1).map(compy => <option value={compy}>{compy}</option>)}    
                    </select></td>
                    <td><button type='button' id={comp[0]} onClick={() => updateStop(comp[0], document.getElementById("selekty-"+comp[0]).value)}>Aktualizovat polohu vozidla</button></td>
                </tbody>
            </table>
            )}
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