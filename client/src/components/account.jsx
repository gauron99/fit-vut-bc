import React,{useEffect,useState,useRef} from 'react';
import { isLoggedIn, getToken, setToken, removeToken} from '../services/userControl';
import { useNavigate } from 'react-router';

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



async function DelAcc(data,setL,setV) {
  console.log("/api/delete?"+data)

  await fetch("/api/delete?"+data,{
    method: "DELETE",
  })
  .then(()=> {
    
    setL({login:'',passwd:'',type:''})
    setV("UNREGISTERED")
    alert("účet úspěšně smazán");
  })
}

const DeleteAcc = (login,setL,setV,navv) => {
  removeToken("token");
  DelAcc(login.login,setL,setV);

  navv('/');
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

    return (
      <div>
        <p>Jste přihlášen jako {token.login} (úroveň: {token.type})</p>
        <button type="button" className="button-submit" onClick={() => Logout(setLogin,setView,navv)}>Odhlásit</button>
        <button type="button" className="button-submit" onClick={() => DeleteAcc(token.login,setLogin,setView,navv)}>Smazat</button>
        
      </div>
      
  )
  } else {
    return (
      <EditLogin handleSubmit={props.handleSubmitLogin} setView={setView}/>
    )
  }
}


const EditRegister = () => {
  const handleRegister = (event) => {
    event.preventDefault();
    const data = event.target;
    if(data.regp != data.regpp){
      alert("Hesla nejsou stejna!");
    }
    console.log(data.regn.value);
    console.log(data.regp.value,"=");
    console.log(data.regpp.value);

  }

  console.log("editRegister in\n");
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
  console.log("/api/login?name="+us+"&passwd="+pas)

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