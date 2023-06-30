import React from "react";
import {getToken, isLoggedIn} from "../services/userControl";
import {EditCrewPage} from "../components/edit_crew";
import {EditAdminPage} from "../components/admin";
import {EditConveyorPage} from "../components/conveyor"

export const EditErrorPage = () => {
  const t = getToken();
  if(!t){
    return <h3>Omlouváme se, k tomuto musíte být příhlášeni :(</h3>
  } else {
    return <h3>Chyba! Jste přihlášeni jako někdo jiný</h3>
  }
}

const EditMakeBasicStruct = (props) => {
  const v = props.view;
  if (v === "CREW" && isLoggedIn("CREW")){
    return <EditCrewPage />
  } else if(v === "CONVEYOR" && isLoggedIn("CONVEYOR")){
    return <EditConveyorPage />
  } else if(v === "ADMIN" && isLoggedIn("ADMIN")){
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
