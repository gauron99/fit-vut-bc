import React from 'react';
import ReactDOM from 'react-dom';

import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
// import { Navigation,Home,History,Favourites,Account } from "./components";
import Home from "./components/home";
import Navigation from "./components/navigation";

ReactDOM.render(
  <Router>
    <Navigation />
    <Routes>
      <Route path="/" element={<Home />} />
      {/* <Route path="/history" element={<History />} /> */}
      {/* <Route path="/favourites" element={<Favourites />} /> */}
      {/* <Route path="/account" element={<Account />} /> */}
    </Routes>
  </Router>,
  // <Home />,
  document.getElementById('root')
);
