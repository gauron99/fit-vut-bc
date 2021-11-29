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

      setUsers(Object.entries(result));

    });
  }, []);

  console.log(users); //works
  // console.log(users.Admins)
  const adm = users[0];
  const conv = users[1];
  const crew = users[2];
  const passe = users[3];

  console.log("---")

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