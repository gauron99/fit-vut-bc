
import React,{useEffect} from 'react';
import { NavLink } from "react-router-dom";

function NormalNavigation() {
  return (
    <ul className="navbar-nav ml-auto">
      <li className="nav-item">
        <NavLink className="nav-link" to="/history">
          Historie
        </NavLink>
      </li>
      <li className="nav-item">
        <NavLink className="nav-link" to="/favourites">
          Oblíbené
        </NavLink>
      </li>
      <li className="nav-item">
        <NavLink className="nav-link" to="/account">
          Účet
        </NavLink>
      </li>
    </ul>
  )
}

class PrivilegedNavigation extends React.Component {
  render() {

    return (
      <ul className="navbar-nav ml-auto">
        <li className="nav-item">
          <NavLink className="nav-link" to="/edit">
            Manager
          </NavLink>
        </li>  
        <li className="nav-item">
          <NavLink className="nav-link" to="/history">
            Historie
          </NavLink>
        </li>
        <li className="nav-item">
          <NavLink className="nav-link" to="/favourites">
            Oblíbené
          </NavLink>
        </li>
        <li className="nav-item">
          <NavLink className="nav-link" to="/account">
            Účet
          </NavLink>
        </li>
      </ul>
    )
  }
}

const SelectMode = (props) =>{
    
    useEffect(() => {
      props.setView("CONVEYOR")
    });

    if(props.view === "UNREGISTERED" || props.view === "USER"){
      return <NormalNavigation />
    } else {
      return <PrivilegedNavigation />
  }
}

const Navigation = (props) => {  
  const view = props.view;
  const setView = props.setView;

    return (
      <div className="navigation">
      <nav className="navbar navbar-expand">
        <div className="container">
        <NavLink className="navbar-brand" to="/">
          Domů
        </NavLink>
        <div>debug: {view}</div>
          <div>
            <SelectMode view={view} setView={setView} />
          </div>
        </div>
      </nav>
    </div>
    )
}

export default Navigation;