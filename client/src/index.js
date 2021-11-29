import React,{ useState } from 'react';
import ReactDOM from 'react-dom';
import "./App.css"
import {getToken} from "./services/userControl";

import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import Home from "./components/home";
import Navigation from "./components/navigation";
import History from "./components/history";
import Favourites from "./components/favourites";
import Account from "./components/account";
import Edit from "./components/edit";


// let modes = ["UNREGISTERED","USER","CREW","CONVEYOR","ADMIN"]
const ViewSetup = () => {

  const [view, setView] = useState("UNREGISTERED");
  
  return (
    <Router>
    <Navigation view={view} setView={setView}/>
    <Routes>
      <Route path="/" element={<Home />} />
      <Route path="/edit" element={<Edit view={view}/>} />
      <Route path="/history" element={<History />} />
      <Route path="/favourites" element={<Favourites />} />
      <Route path="/account" element={<Account view={view} setView={setView}/>} />
    </Routes>
  </Router>
  )
}

ReactDOM.render(
  <ViewSetup />,
  document.getElementById('root')
);
