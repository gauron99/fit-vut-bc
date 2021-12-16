import React,{useEffect,useState,useRef} from 'react';
import { isLoggedIn, getToken, setToken, removeToken} from '../services/userControl';
import { useNavigate } from 'react-router';
import {checkForUsers} from '../services/userControl';

import "../App.css";


  
const EditLogin = (props) => {
  const onSubmit = props.handleSubmit;

  const loginRef = useRef('');
  const passwdRef = useRef('');
  
    const handleSubmit = e => {
      e.preventDefault()
      const data = {
        username:loginRef.current.value,
        passwd:passwdRef.current.value
      };
      onSubmit(data);
    }


  return (
    <div className="login-main">
      Přihlášení (zadejte své údaje)
      <div className="float-right-hint">
        hint
      </div>
      <hr className="solid" />

      <div className="login-input" >
      <form onSubmit={handleSubmit}>
        <label>
          <input ref={loginRef} className="login-item" id="login-n" type="text" placeholder="jmeno"></input>
          {/* <input value={username} onChange={e => setUSERNAME(e.target.value)} className="login-item" id="login-n" type="text" placeholder="jmeno"></input> */}
        </label>

        <label>
          <input ref={passwdRef} className="login-item" id="login-p" type="password" placeholder="heslo"></input>
          {/* <input value={passwd} onChange={e => setPASSWD(e.target.value)} className="login-item" id="login-p" type="password" placeholder="heslo"></input> */}
        </label>

        <button type="submit" className="button-submit button-login float-left">Přihlásit </button>
      </form>
        <button type="button" className="button-submit button-login" onClick={()=>props.setviewACC(2)}>Registrovat </button>
      </div>
    </div>
  )
}

// /////////////////////////////////////////////////////////////////////////////



async function DelAcc(login,setL,setV,role) {
  var nameName = role === "conveyor" ? "firm" : "name";
  console.log("/api/"+role+"_manage?"+nameName+"="+login);

  await fetch("/api/"+role+"_manage?"+nameName+"="+login,{
    method: "DELETE",
  })
  .then(()=> {
    
    setL({login:'',passwd:'',type:''})
    setV("UNREGISTERED")
    alert("účet úspěšně smazán");
  })
}

const DeleteAcc = (login,setL,setV,navv,role) => {
  if(window.confirm("Opravdu chcete smazat svůj účet '"+login+"'?")){
    removeToken("token");
    DelAcc(login,setL,setV,role);

    navv('/');
  } else {}
}

const Logout = (setL,setV,navv) => {
    removeToken("token");
    setL({login:'',passwd:'',type:''});
    setV("UNREGISTERED");

  navv('/')
}

const EditLogout = (props) => {
  const setLogin = props.setLogin;
  const setView = props.setView;

  const navv = useNavigate();
  const token = getToken();

  if(token){
    if(token.type == "admin"){
      // if logged in is admin, cant delete
      return (
        <div>
        <p>Jste přihlášen jako {token.login} (úroveň: {token.type})</p>
        <button type="button" className="button-submit" onClick={() => Logout(setLogin,setView,navv)}>Odhlásit</button>
      </div>
      )
    }
    else {
      // if anyone else, you can delete
      return (
        <div>
        <p>Jste přihlášen jako {token.login} (úroveň: {token.type})</p>
        <button type="button" className="button-submit" onClick={() => Logout(setLogin,setView,navv)}>Odhlásit</button>
        <button type="button" className="button-submit" onClick={() => DeleteAcc(token.login,setLogin,setView,navv,token.type)}>Smazat</button>
      </div>
      )
    }
  } else {
    return (
      <EditLogin handleSubmit={props.handleSubmitLogin} setView={setView}/>
    )
  }
}


async function registerUser(n,p,setView){
    var boolec = await checkForUsers(n);
   if(boolec){
    await fetch('/api/passenger_manage?name='+n+"&passwd="+p+"&registered=1",{
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
    })
    .then(response => response.json())
    .then((res)=>{
        alert("Uspesne registrovan!");
        var data = {name: n, passwd: p, type:"passenger"}
        setToken(data);
        setView("USER");
    })
  }
  else {
      alert("zajděte se na matriku přejmenovat prosím, uživatele s takovým jménem již v databázi máme");
  }
}

async function checkPassengers(name,passwd,setView){
  await fetch('/api/passenger_manage',{
    method: "GET",
  })
  .then(response => response.json())
  .then(users => {
    console.log(users);
    for (const x of users){
      if (x.name == name){
        alert("Chyba! jmeno jiz existuje");
        return;
      }
    }
    // console.log("Jsem za! vseckocajk");
    // pokud neexistuje uzivatel, muzes se uspesne registrovat
      registerUser(name,passwd,setView)
  })

}

const EditRegister = (props) => {
  const handleRegister = (event) => {
    event.preventDefault();
    const data = event.target;

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

    // check if user exists
    checkPassengers(data.regn.value,data.regp.value,props.setView);
  }

  return (
    <div className="registering">
      <h3 className="h3reg">Zaregistruj se</h3>
      <form onSubmit={handleRegister}>
        <label htmlFor="regn">Jméno</label>
          <input className="register-item" id="regn" type="text" placeholder="jmeno"></input>
        
        <label htmlFor="regp">Heslo</label>
          <input className="register-item" id="regp" type="password" placeholder="heslo"></input>
                
        <label htmlFor="regpp">Heslo zopakovat</label>
          <input className="register-item" id="regpp" type="password" placeholder="heslo"></input>
          <hr className="solid" />
        <button type="submit" className=" register-item button-submit button-login">Registrovat</button>
      </form>
    </div>
  )
}

async function loginUser(setLogin,data) {
  const us = data.username;
  const pas = data.passwd;
  // console.log("/api/login?name="+us+"&passwd="+pas)

  await fetch("/api/login?name="+us+"&passwd="+pas,{
    method: "POST",
    headers: { 'Content-Type': 'application/json' },
  })
  .then(response => response.json())
  .then(data => {
    console.log("afterFETCHER: ",data);
    if(data.ACCESS == "GRANTED"){
      alert("login úspěšný");
      const d = {
        id: data.ID,
        login: us,
        passwd: pas,
        type: data.ROLE
      }
      // localStorage.setItem("token",JSON.stringify(d));
      setToken(d);
      // const data = getToken("token")
      setLogin({id:d.id,login:d.login, passwd:d.passwd,type:d.type})
    } else {
      alert("Nepodařilo se přihlásit");
    }
  })
  .then( () => {
    
  })
};
 
const AccountSelector = (props) => {
  const handleSubmitLogin = (data) => {
    loginUser(props.setLogin,data);
    }

  if (props.viewACC === 0){ //basic page, not logged in
    console.log("LOGGING IN")
    return <EditLogin handleSubmit={handleSubmitLogin} setviewACC={props.setviewACC}/>
  } else if (props.viewACC === 1) { //logged in page
    console.log("ALREADY LOGGED IN")
    return <EditLogout view={props.view} setView={props.setView} login={props.login} setLogin={props.setLogin} handleSubmit={handleSubmitLogin}/>;
  } else {// trying to register
    console.log("REGISTERING")
    return <EditRegister setView={props.setView} login={props.login} setLogin={props.setLogin}/>
  }
};

const Account = (props) => {
  // if someone is logged in, show logout button
  const [viewACC,setviewACC] = useState(0);
  const [login,setLogin] = useState({id:-1,login:'',passwd:'',type:''});

  console.log("ACCOUNTview: ",props.view)
  console.log("ACCOUNTlogin: ",login)

  useEffect(()=>{
    if(isLoggedIn()){

      let data = getToken("token");
      console.log(data)
      if(data){
        const loc = data;
        if(login.login !== loc.login){
          setLogin(() => ({
            id: loc.id,
            login: loc.login,
            passwd: loc.passwd,
            type: loc.type
          }))
        }        

        if(viewACC !== 1){
          setviewACC(1);
        }

        if(loc.type === "admin" && props.view !== "ADMIN"){
          props.setView("ADMIN");
        } else if (loc.type === "crew" && props.view !== "CREW"){
          props.setView("CREW");
        } else if (loc.type === "conveyor" && props.view !== "CONVEYOR"){
          props.setView("CONVEYOR");
        } else if (loc.type === "passenger" && props.view !== "USER"){
          props.setView("USER");
        } else if (loc.type==='' && props.view !== "UNREGISTERED"){
            props.setView("UNREGISTERED");
          }
        }
    }
  },[login,props.view,viewACC])

  return <AccountSelector view={props.view} setView={props.setView} viewACC={viewACC} setviewACC={setviewACC} login={login} setLogin={setLogin} />
}

export default Account;