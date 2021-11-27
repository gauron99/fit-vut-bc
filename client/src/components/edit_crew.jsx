import react from "react";
import {getToken, isLoggedIn} from "../services/userControl";
import { EditErrorPage } from "./edit";




export const EditCrewPage = () => {
  // check if crew is connected
  if(isLoggedIn("CREW")){
    return (
      null
    )
  }else {
    return <EditErrorPage />
  }
}