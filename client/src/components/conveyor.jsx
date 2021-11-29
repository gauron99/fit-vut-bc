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

const GetPerson = (val) => {
  const x = val.val;
  // getToken() zjisti kdo to je a tady bude kontrola jestli tu osobu vubec chces 
  // if(x.conveyorID === )



  return(
    <tr>
      <td>{x.ID}</td>
      <td>{x.name}</td>
      <td>{x.passwd}</td>
      <td><button class="editbtn" onClick={()=> editPersonel(x.ID)}>edit</button></td>
    </tr>
  )
}

const EditConvPerson = () => {

  const [personel,setPersonel] = useState([]);


  // useEffect(() => {

  //   getData()
  // }, []);

  // async function getData() {
  //   const response = await fetch('/api/crew_manage?'+ token.firm);
  //   const result = await response.json();
  //   console.log(result.data);

  // }

  useEffect(()=>{
    const token = getToken()
    fetch("/api/crew_manage?firm="+token.firm,{
      method: "GET",
      headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
      // body: JSON.stringify({ dopravceID: 'd' })
     })
    .then((res) => res.json())
    .then((result) => {
      setPersonel(result)
    })
  },[personel])

  console.log(personel);


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
            return (
              <GetPerson val={val}/>
            )
          })}
        </tbody>
      </table>
    </div>
  )
}

const EditConveyorChangeView = (props) => {
  if(props.convPage === 0){
    return <EditConvPerson/>
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

  return (
    <div>
      <EditHeaderPeople convPage={convPage} setConvPage={setConvPage}/>
      <EditConveyorChangeView convPage={convPage} setConvPage={setConvPage}/>
    </div>
  )
}