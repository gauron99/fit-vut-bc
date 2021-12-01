import React,{useState, useEffect , Fragment} from 'react';

import "../Admin.css"

export const EditAdminPage = () => {
  const  [users,setUsers] = useState([]);

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
            {users.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}<td><button type='button' onClick={() => console.log('pls')} className="button-show">uhoh</button></td></tr>)}
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