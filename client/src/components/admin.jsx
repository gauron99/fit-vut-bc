import React,{useState, useEffect , Fragment} from 'react';

import "../Admin.css"

async function updateCuraka(name, pwd, id, role){
    console.log("teka se updatne curak tadyhle");
    var nameName = role === 'Conveyor' ? 'firm' : 'name';

    
    await fetch("/api/"+role+"_manage?"+nameName+"="+name+"&passwd="+pwd+'&ID='+id,{
        method: "PUT",
        headers: { 'Content-Type': 'application/json' },
      })
      .then(response => response.json())
}

async function getHeslo(setslo,userRole,userID) {
    await fetch('/api/getpwd?role='+userRole+'&ID='+userID,{
        method: "GET",
      })
      .then(response => response.json())
      .then(hesl => {
          console.log(hesl)
         setslo(hesl.passwd);
         return hesl.passwd;
      })
}

const PopPeopleWindow = (props) => {
    if (!props.trigger)
        return "";
    else {
        var name = props.userRole === 'conveyor' ? 'firm' : 'jmeno';
        return (
            <div className="popup-window">
            <div className="popup-in">
            <button className="reserve-close-button" onClick={() =>{props.handleTrigger(!props.trigger)}}>X</button>
                <h3 className="h3reg">Opichej zmrda</h3>
                <form>
                <label htmlFor="regn">{name}</label>
                    <input className="register-item" id="meno" type="text" defaultValue={props.userName}></input>
                
                <label htmlFor="regp">Heslo</label>
                    <input className="register-item" key={props.heslo} id="hesloo" type="text" defaultValue={props.heslo}></input>
                        
                    <hr className="solid" />
                <button onClick={() => updateCuraka(document.getElementById('meno').value, document.getElementById('hesloo').value, props.userID, props.userRole)} type="submit" className=" register-item button-submit button-login">Posrat hlavu zmrdovi</button>
                </form>
            </div>
            </div>
        );
    }
}

export const EditAdminPage = () => {
  const  [users,setUsers] = useState([]);
  const  [usePOP,setPop] = useState([false]);
  const  [heslo,setslo] = useState("");

  useEffect(() =>{
    fetch('/api/read_and_subsequently_possibly_config_if_desired_or_not_if_not_necessary_or_not_desired',{
      method:"GET",
      headers: {
        'Content-Type': 'application/json',
        'Accept': 'application/json'
      },
    })
    .then((res) => res.json())
    .then((result) => {
        var lidicky = [];
        for (let lidicka of result){
            lidicky.push(Object.values(lidicka));
        }
      setUsers(lidicky);

    });
  }, []);

  console.log(users); //works

  return (
    <div className="admin_frame">
      <div className="admin_users_config">
        <table className="admin_users_table">
          <thead className="admin_table_head">
            <tr>
              <th>ID</th>
              <th>Jméno</th>
              <th>Účet</th>
              <th>*</th>
            </tr>
          </thead>
          <tbody>
            {users.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}<td><button type='button' onClick={()=>setPop([!usePOP[0],comp[0],comp[2],comp[1],getHeslo(setslo,comp[2],comp[0])])} className="button-show">pičo</button></td></tr>)}
            <PopPeopleWindow trigger={usePOP[0]} heslo={typeof(usePOP[4]) === 'object' ? heslo : typeof(usePOP[4])} userName={usePOP[3]} userID={usePOP[1] === undefined ? '1' : usePOP[1]} userRole={usePOP[2] === undefined ? 'admin' : usePOP[2]} handleTrigger={setPop} />
          </tbody>
      </table>
      </div>

      <div className="admin_others_config">

        <div className=""></div>
        <div></div>
        <div></div>

      </div>
    </div>
  )   
}