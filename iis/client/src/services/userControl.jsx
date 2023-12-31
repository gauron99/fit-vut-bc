import React,{useEffect,useState} from "react";
import Account from "../components/account"


export const handleRegister = (props) => {
  
}

export async function checkForUsers(name){
    var p;
    await fetch('/api/isuserok?name='+name,{
        method:"GET",
        headers: {
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        },
      })
      .then((res) => res.json())
      .then((result) => {
         p = result.msgis;
        })
    return p;
}

export async function checkForStops(name){
    var p;
    await fetch('/api/isstopok?name='+name,{
        method:"GET",
        headers: {
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        },
      })
      .then((res) => res.json())
      .then((result) => {
         p = result.msgis;
        })
    return p;
}

export const Logout = (props) => {
  if (isLoggedIn()){

  }
}
// returns true if speficied user is logged in
// special keyword "anyone" checks if anyone is logged in
export const isLoggedIn = (props) => {
  // return (localStorage.getItem("token") !== null) ? true : false;
  return ((JSON.parse(localStorage.getItem("token")) !== null) ? true : false);
}

export const getToken = () => {
  let token = localStorage.getItem("token")
  console.log("TOKEN in SESS: ",token);
  return JSON.parse(token, null, -1);
}

export const setToken = (data) => {
  console.log("SESS DATA SETTING TO: ",data)
  localStorage.setItem("token",JSON.stringify(data))
}

export const removeToken = () => {
  localStorage.removeItem("token");
  const data = localStorage.getItem("token") === null;
  console.log("removed logged; is null now?: ",data)
}