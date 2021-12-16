import React, { useState, useEffect } from 'react';
import { Multiselect } from "multiselect-react-dropdown";
import { getToken } from "../services/userControl"
import { useNavigate } from 'react-router';

import "../App.css"

const transformDataToMultiselect = (data,seatClass) => {
  var tmp = [];
  for(let i = 0; i < data.length; ++i){
    var x = {
      name: "Sedadlo "+data[i].seat,
      class: "Třída "+seatClass
    }
    tmp.push(x);
  }
  return (tmp);
}

async function getSeatsRich(ID,setVeh){
  await fetch("/api/free_vehicle_seats_rich?ID="+ID)
  .then(res => res.json())
  .then(data => {
    var transformed = transformDataToMultiselect(data,1)
    setVeh(prev => [...prev, ...transformed])
  })
}

async function getSeatsPoor(ID,setVeh){
  await fetch("/api/free_vehicle_seats_poor?ID="+ID)
  .then(res => res.json())
  .then(data => {
    var transformed = transformDataToMultiselect(data,2)
    setVeh(transformed)
  })
}

async function getAllInfoReservations(connID,setVeh) {
  await fetch("/api/spoj?ID="+connID,{
    method: "GET",
  })
  .then(response => response.json())
  .then(conn => {
    getSeatsPoor(conn.vehicleID,setVeh)
    getSeatsRich(conn.vehicleID,setVeh)
  })
}

const onSelect = (selectedList,set,poor,rich,setallSelected) =>{
  var currPrice = 0;
  for(let i = 0; i < selectedList.length;++i){
    if(selectedList[i].class.localeCompare("Třída 2")===0){
      currPrice += poor;
    } else {
      currPrice += rich;
    }
  } 
  set(currPrice);
  setallSelected(selectedList);

} 

const onRemove = (selectedList,set,poor,rich,setallSelected) =>{
  var currPrice = 0;
  for(let i = 0; i < selectedList.length;++i){
    if(selectedList[i].class.localeCompare("Třída 2")===0){
      currPrice += poor;
    } else {
      currPrice += rich;
    }
  } 
  set(currPrice);
  setallSelected(selectedList);
}
//   const navv = useNavigate(); REDIRECT TO ANOTHER WINDOW BABY
async function RegisterOnly(list,connID,cost,navv){
  // if no seats were selected
  if(list === undefined){
    alert("Vyberte sedadla k rezervaci")
    return;
  }

  var ID;
  const token = getToken();
  if (!token) {
    alert ("Prosím registruj se předtím než si něco koupíš (bo zatím nelze jinak)");
    // create new user - table-row registered=FALSE (all i need is a name)

  } else {
    ID = token.id;
  }

  var seats = '';
  for (let iter of list){
    var seatID = iter.name.split(' ')[1];
    seats += seatID+','
  }
  const res = seats.slice(0, -1);
  console.log("res seats:",res)

  await fetch("/api/reservation?connectionID="+connID+"&passengerID="+ID+"&seats="+res+"&cost="+cost,{
    method: "POST",
  })
  .then(data => {
    console.log("DATAAFETRPOST: ",data)
    if(data.msg.localeCompare("OK") === 0) {
      alert("Rezervace úspěšná!")
      navv('/');
    } else {
      alert("Rezervace nemohla být provedena(některá ze sedadel již nejsou volná)")
      navv('/');
    }
  })
}

async function RegisterBuy(list){
  await fetch('/api/reservation?ID=',{

  })
  .then()
}

async function updateStop(connID,setLastStop) {
  var vehicleID;
  await fetch('/api/getvehicle?ID='+connID,{
    method: "GET",
  })
  .then(response => response.json())
  .then(result => {
    vehicleID = result.vehicleID;
  })
  console.log("no?"+vehicleID)
  await fetch('/api/gde_spoj?ID='+vehicleID,{
    method: "GET",
  })
  .then(response => response.json())
  .then(result =>{
      console.log(result.stopName);
      setLastStop(result.stopName);
  })
}

export const PopupRegister = (props) => {
  const price = props.price;
  const [allSelected, setallSelected] = useState();
  const [vehSeats,setVehSeats]  = useState([]);
  const [total, setTotal] = useState(0);
  const [lastStop, setLastStop] = useState("");
  const navv = useNavigate();
  useEffect(()=>{
    if(props.trigger){
      getAllInfoReservations(props.connID,setVehSeats)
    }
    updateStop(props.connID, setLastStop);
  },[props.connID])//whenever connID changes (button clicks and sets it in home.jsx - useEffect runs)
  return props.trigger ? (
    <div className="popup-window">
        <div className="popup-in">
          <button className="reserve-close-button" onClick={(e) =>{ props.updTrigger(e);setTotal(0);}}>X</button>
          <h3>Rezervace: Linka: {props.connID}</h3>

          {/* <Multiselect
            placeholder="Vyber jízdenky"
            onSelect={(a)=>onSelect(a,setTotal,price[0],price[1],setallSelected)}
            onRemove={(a)=>onRemove(a,setTotal,price[0],price[1],setallSelected)}
            options={vehSeats}
            displayValue="name"
            groupBy="class"
            /> */}
          <p>Celková cena: {total},-</p>
          <div>Last stop: {lastStop}</div>
          <button className="reserve-confirms" onClick={()=>RegisterOnly(allSelected,props.connID,total,navv)}>Rezervovat</button>
          <button className="reserve-confirms" onClick={()=>RegisterBuy(allSelected,props.connID,total,navv)}>Rezervovat a koupit</button>
          <button className="reserve-confirms" onClick={()=>updateStop(props.connID, setLastStop)}>Aktualizovat polohu vozidla</button>
        </div>
    </div>
  ) : "";
};
