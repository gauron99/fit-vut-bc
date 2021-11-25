
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
                  History
                </NavLink>
              </li>
              <li className="nav-item">
                <NavLink className="nav-link" to="/favourites">
                  Favourites
                </NavLink>
              </li>
              <li className="nav-item">
                <NavLink className="nav-link" to="/account">
                  Account
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