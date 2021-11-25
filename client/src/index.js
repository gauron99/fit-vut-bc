import React from 'react';
import ReactDOM from 'react-dom';
import "./App.css"


import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
// import { Navigation,Home,History,Favourites,Account } from "./components";
import Home from "./components/home";
import Navigation from "./components/navigation";
import History from "./components/history";
import Favourites from "./components/favourites";
import Account from "./components/account";

ReactDOM.render(
  <Router>
    <Navigation />
    <Routes>
      <Route path="/" element={<Home />} />
      {/* <Route path="/search" element={<Search />} /> */}
      <Route path="/history" element={<History />} />
      <Route path="/favourites" element={<Favourites />} />
      <Route path="/account" element={<Account />} />
    </Routes>
  </Router>,
  // <Home />,
  document.getElementById('root')
);
