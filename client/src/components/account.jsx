import React from 'react';
import { isLoggedIn, getToken, handleLogin,handleRegister} from '../services/userControl';

const EditLogin = (props) => {
  return (
    <div className="login-main">
      Přihlášení (zadejte své údaje)
      <div className="float-right-hint">
        hint
      </div>
      <hr className="solid" />

      <div className="login-input" >
      <form>
        <label>
          <input className="login-item" id="login-n" type="text" placeholder="jmeno"></input>
        </label>

        <label>
          <input className="login-item" id="login-p" type="password" placeholder="heslo"></input>
        </label>

        <button type="button" className="button-submit button-login float-left" onClick={handleLogin()}>Přihlásit </button>
      </form>
        <button type="button" className="button-submit button-login" onClick={handleRegister()}>Registrovat </button>
      </div>
    </div>

  )
}

const EditLogout = (props) => {
  const [n,p] = getToken();

  return (
    <div>
      <p>Jste přihlášen jako {n} (úroveň: big sause boss)</p>
      <button type="button" className="button-submit" formAction="uc.Logout()">Odhlásit</button>
    </div>
  )
}

const Account = (props) => {
  // if someone is logged in, show logout button
  if (isLoggedIn()){
    return <EditLogout view={props.view} setView={props.setView}/>;
  } else {
    return <EditLogin setView={props.setView}/>
  }
};

export default Account;