import React, { useState, useEffect } from 'react';
import { Multiselect } from "multiselect-react-dropdown";
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

async function getAllInfoReservations(connID,set,setVeh) {
  await fetch("/api/spoj?ID="+connID,{
    method: "GET",
  })
  .then(response => response.json())
  .then(conn => {
    set({poor:conn.price_poor,rich:conn.price_rich});
    getSeatsPoor(conn.vehicleID,setVeh)
    getSeatsRich(conn.vehicleID,setVeh)
  })
}


const onSelect = (selectedList,set,poor,rich) =>{
  var currPrice = 0;
  for(let i = 0; i < selectedList.length;++i){
    if(selectedList[i].class.localeCompare("Třída 2")===0){
      currPrice += poor;
    } else {
      currPrice += rich;
    }
  } 
  set(currPrice);
} 

const onRemove = (selectedList,set,poor,rich) =>{
  var currPrice = 0;
  for(let i = 0; i < selectedList.length;++i){
    if(selectedList[i].class.localeCompare("Třída 2")===0){
      currPrice += poor;
    } else {
      currPrice += rich;
    }
  } 
  set(currPrice);
}





export const PopupRegister = (props) => {
  const [price, setPrice]       = useState({poor:0,rich:0});
  const [vehSeats,setVehSeats]  = useState([]);
  const [total, setTotal] = useState(0);

  useEffect(()=>{
    if(props.trigger){
      getAllInfoReservations(props.connID,setPrice,setVehSeats)
    }
  },[props.connID])//whenever connID changes (button clicks and sets it in home.jsx - useEffect runs)

  useEffect(()=>{

  },[total])
  
  return props.trigger ? (
    <div className="popup-window">
        <div className="popup-in">
          <button className="reserve-close-button" onClick={(e) =>{ props.updTrigger(e);setTotal(0);}}>X</button>
          <h3>Rezervace: Linka: {props.connID}</h3>

          <Multiselect
            placeholder="Vyber jízdenky"
            onSelect={(a)=>onSelect(a,setTotal,price.poor,price.rich)}
            onRemove={(a)=>onRemove(a,setTotal,price.poor,price.rich)}
            options={vehSeats}
            displayValue="name"
            groupBy="class"
            />
          <p>Celková cena: {total},-</p>
          <button className="reserve-confirms" onClick={()=>console.log("REZERVUJES JEN")}>Rezervovat</button>
          <button className="reserve-confirms" onClick={()=>console.log("KUPUJES HNED")}>Rezervovat a koupit</button>
        </div>
    </div>
  ) : "";
};
