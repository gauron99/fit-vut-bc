import React,{useState,useEffect} from "react";
import {GeneralErrorPage} from "./home";
import {getToken} from "../services/userControl"

import "../People.css"

const EditConvStops = () => {
  return (
    null
  )
}

const EditConvConn = () => {
  return (
    null
  )
}


const EditConvMachine = () => {
  return (
    null
  )
}


const editPersonel = (id) => {
  console.log("tady budes editovat, otevres na to maly okno nebo tak")
}

// const GetPerson = (val) => {
//   const x = val.val;
//   return(
//   <tr>
//   <td>{x.ID}</td>
//   <td>{x.name}</td>
//   <td>{x.passwd}</td>
//   <td><button class="editbtn" onClick={()=> editPersonel(x.ID)}>edit</button></td>
// </tr>
//   )
// }

async function GetPeople(personel,setPersonel) {
  const token = getToken();
  await fetch("/api/crew_manage?firm="+token.login,{
    method: "GET",
    headers: {
    'Content-Type': 'application/json',
    'Accept': 'application/json'
    },
   })
  .then((res) => res.json())
  .then((result) => {
    console.log(personel);
    setPersonel(result)
  })
}

const EditConvPerson = (props) => {

  const [personel,setPersonel] = useState([]);

  console.log("convPAGE:",props.convPage);

  useEffect(()=>{
    GetPeople(personel,setPersonel);
  },[])
    
    return(
      <div className="main-page-people">
      <table className="people-table">
        <thead>
          <tr>
            <th>ID</th>
            <th>Jméno</th>
            <th>Heslo</th>
            <th>*</th>
          </tr>
        </thead>
        <tbody>
          {personel.map(val => {
            console.log("VAL: ",val.ID)
            console.log("VALF: ",val)
            return (
              <tr>
                <td>{val.ID}</td>
                <td>{val.name}</td>
                <td>{val.passwd}</td>
                <td><button class="editbtn" onClick={()=> editPersonel(val.ID)}>edit</button></td>
              </tr>
            )
          })}
        </tbody>
      </table>
    </div>
  )
}

const EditConveyorChangeView = (props) => {
  if(props.convPage === 0){
    return <EditConvPerson convPage={props.convPage} setConvPage={props.setConvPage}/>
  } else if(props.convPage===1) {
    return <EditConvMachine/>
  } else if(props.convPage===2) {
    return <EditConvConn/>
  } else if(props.convPage===3) {
    return <EditConvStops/>
  } else {
    return <GeneralErrorPage />
  }
}

const EditHeaderPeople = (props) => {
  
  return (
    <div>
      <div className="header-people">
        <button onClick={() => props.setConvPage(0)} className="btn-ppl">Personál</button>
        <button onClick={() => props.setConvPage(1)} className="btn-ppl">Vozidla</button>
        <button onClick={() => props.setConvPage(2)} className="btn-ppl">Spoje</button>
        <button onClick={() => props.setConvPage(3)} className="btn-ppl">Zastávky</button>
        vidim: {props.convPage}
      </div>
      <div>
        
      </div>
    </div>
  )
}

export const EditConveyorPage = () => {
    
  const [convPage,setConvPage] = useState(0);
  console.log("CONVPAGEROOT: ",convPage);

  return (
    <div>
      <EditHeaderPeople convPage={convPage} setConvPage={setConvPage}/>
      <EditConveyorChangeView convPage={convPage} setConvPage={setConvPage}/>
    </div>
  )
}