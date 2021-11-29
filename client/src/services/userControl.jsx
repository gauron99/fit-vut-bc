// import React from "react";

export const handleRegister = (props) => {

}

export const handleLogin = (props) => {

  const n = props.name;
  const p = props.password

}

export const Logout = (props) => {
  if (isLoggedIn()){

  }
}
// returns true if speficied user is logged in
// special keyword "anyone" checks if anyone is logged in
export const isLoggedIn = (props) => {
  return true;
}

// returns null if token doesnt exist / returns token if exists (array of [name,password])
export const getToken = () => {
  let token = sessionStorage.getItem("token");
}

export const setToken = (props) => {
  sessionStorage.setItem("token",JSON.stringify(props.userToken))
}
