
import React from 'react';
import { NavLink } from "react-router-dom";


function Navigation() { 
  return (
    <div className="navigation">
      <nav className="navbar navbar-expand">
        <div className="container">
        <NavLink className="navbar-brand" to="/">
          Home
        </NavLink>
          <div>
            <ul className="navbar-nav ml-auto">
              <li className="nav-item">
                <NavLink className="nav-link" to="/history">
                  History of the wolrd
                </NavLink>
              </li>
              <li className="nav-item">
                <NavLink className="nav-link" to="/favourites">
                  yo fav
                </NavLink>
              </li>
              <li className="nav-item">
                <NavLink className="nav-link" to="/account">
                  yo acc
                </NavLink>
              </li>
            </ul>
          </div>
        </div>
      </nav>
    </div>
  )
}

export default Navigation;