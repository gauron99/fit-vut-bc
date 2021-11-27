import React from "react";
import {getToken, isLoggedIn} from "../services/userControl";
import {EditCrewPage} from "../components/edit_crew";

export const EditErrorPage = () => {
  const t = getToken();
  if(!t){
    return <h3>Omlouváme se, k tomuto musíte být příhlášeni :(</h3>
  } else {
    return <h3>Jste přihlášeni jako někdo jiný *suspicious*</h3>
  }
}



const EditConveyorPage = () => {
  // check if conveyor is connected, if so, which one
  if(isLoggedIn("CONVEYOR")){
    
  } else {
    return <EditErrorPage />
  }
}

const EditAdminPage = () => {
// check if admin is connected
  if(isLoggedIn("ADMIN")){
    
  }else {
    return <EditErrorPage />
  }
}

const EditMakeBasicStruct = (props) => {
  const v = props.view;
  if (v === "CREW"){
    return <EditCrewPage />
  } else if(v === "CONVEYOR"){
    return <EditConveyorPage />
  } else if(v === "ADMIN"){
    return <EditAdminPage />
  } else {
    return (
      <h3>Omlouváme se, ale k tomuto obsahu nemáte přístup, nezapomněli jste se přihlásit?</h3>
    )
  }
}

const Edit = (props) => {

  

  return(
    <EditMakeBasicStruct view={props.view}/>
  )
}

export default Edit;

