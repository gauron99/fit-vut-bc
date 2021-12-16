import React,{useState,useEffect} from "react";
import {GeneralErrorPage} from "./home";
import {getToken} from "../services/userControl"

import "../People.css"
import "../App.css"

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


const editPersonel = (e) => {
  
}




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

async function addNewCrew(n,p,handleTrigger,trigger,token,pers,sPers){
  await fetch("/api/crew_manage?name="+n+"&passwd="+p+"&conveyorID="+token.id,{
    method: "POST",
    headers: {
      'Content-Type': 'application/json',
    }
  })
  .then(response => response.json())
  .then((res)=>{
    // alert("Uspesne pridan!");
    handleTrigger(!trigger);
    GetPeople(pers,sPers)
  })
}

async function registerCrew(n,p,handleTrigger,trigger,pers,sPers) {
  const token = getToken();

  // check if name already exists
  await fetch("/api/crew_manage?firm="+token.login,{
    method: "GET",
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
      },
  })
  .then(result => result.json())
  .then(res => {
    for(let x of res){
      if(x.name === n){
        alert("Jmeno jiz existuje!");
        return;
      }
    }
    // its new crew! create him in database
    addNewCrew(n,p,handleTrigger,trigger,token,pers,sPers);
  })
}

const PopPeopleWindow = (props) => {
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  const  handleNewCrew = (event) => {
    event.preventDefault();
    const data = event.target;
    // name not given
    if( data.regn.value === ""){
      alert("Zadejte jmeno prosim!");
      return;
    }
    // passwd not given
    if( data.regp.value === ""){
      alert("Zadejte heslo prosím!");
      return;
    }
    // check if passwords match
    if(data.regp.value !== data.regpp.value){
      alert("Hesla nejsou stejna!");
      return;
    }
    // check if crew exists inside registerCrew
    registerCrew(data.regn.value,data.regp.value,handleTrigger,trigger,props.personel,props.setPersonel);
  }

  return trigger ? (
    <div className="popup-window">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Vytvoř účet pro personál</h3>
        <form onSubmit={handleNewCrew}>
          <label htmlFor="regn">Jméno</label>
            <input className="register-item" id="regn" type="text" placeholder="jmeno"></input>
          
          <label htmlFor="regp">Heslo</label>
            <input className="register-item" id="regp" type="password" placeholder="heslo"></input>
                  
          <label htmlFor="regpp">Heslo zopakovat</label>
            <input className="register-item" id="regpp" type="password" placeholder="heslo"></input>
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Vytvořit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
}


const EditPerson = (props) => {
  const data = props.data;
  const setData = props.setData;
  const trigger = props.trigger;
  const handleTrigger = props.handleTrigger;

  useEffect(()=> {    
  },[data,setData])

  return trigger ? (
    <div className="popup-window-down">
      <div className="popup-in">
      <button className="reserve-close-button" onClick={() =>{handleTrigger(!trigger)}}>X</button>
        <h3 className="h3reg">Uprav personál</h3>
        <form onSubmit={editPersonel}>
          <label htmlFor="regn">Jméno</label>
            <input className="register-item" id="regn" type="text" defaultValue={data.name}></input>
          
          <label htmlFor="regp">Heslo</label>
            <input className="register-item" id="regp" defaultValue={data.passwd}></input>
                
            <hr className="solid" />
          <button type="submit" className=" register-item button-submit button-login">Upravit</button>
        </form>
      </div>
    </div>
  ) 
  : "";
}

const EditConvPerson = (props) => {
  const [popPeople,setPopPeople] = useState(false)
  const [popEditPerson,setPopEditPerson] = useState(false)
  const [personel,setPersonel] = useState([]);
  const [personData,setPersonData] = useState({name:"",passwd:""});

  console.log("convPAGE:",props.convPage);

  useEffect(()=>{
    GetPeople(personel,setPersonel);
  },[])

  // const handlePop = () => {
    // console.log("handluju popis",popPeople)
    // setPopPeople(prev => ({
    //   popPeople: !prev.popPeople
    // }))
  // }

  const handleData = (id,n,p,e) => {
    console.log(id,n,p,e);
    setPersonData(()=>({
      name:n,
      passwd:p
    }));
    setPopEditPerson(true);
  }

  return(
  <div className="main-page-people">
    <button class="add-button" onClick={() => setPopPeople(!popPeople)}>+</button>
    <PopPeopleWindow trigger={popPeople} handleTrigger={setPopPeople} setPersonel={setPersonel} personel={personel}/>
    <EditPerson trigger={popEditPerson} handleTrigger={setPopEditPerson} data={personData} setData={handleData}/>
    <table className="people-table">
      <thead>
        <tr>
          <th>ID</th>
          <th>Jméno</th>
          <th>*</th>
        </tr>
      </thead>
      <tbody>
        {personel.map(val => {
          // console.log("VAL: ",val,val.ID);
          return (
            <tr>
              <td>{val.ID}</td>
              <td>{val.name}</td>
              {/* <td>{val.passwd}</td> */}
              <td><button class="editbtn" onClick={()=> handleData(val.ID,val.name,val.passwd)}>upravit</button></td>
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
    return <EditConvPerson convPage={props.convPage}/>
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