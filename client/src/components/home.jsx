import React from "react";

// function Home() {
//   return (
//     <div className="home">
//       <div class="container">
//         <img
//           src="http://placehold.it/900x400"
//           alt=""
//         />
//       </div>
//       <div class="col-lg-5">
//         <h1 class="font-weight-light">Doma ses</h1>
//         <p>
//           welcome home, boy.
//         </p>
//       </div>
//     </div>
//   );
// }

class SearchWithLabel extends React.Component{
  constructor(){
    super();
    var today = new Date();
    // timeNow = Date.ti
    this.dateNow = today.getDay() + '.' + today.getMonth() + '.';
    this.timeNow = today.getHours() + ":" + today.getMinutes();
  }
  render(){
    console.log(this.timeNow)
    console.log(this.dateNow)

    return(
      <form>
        <label for="search-from">Odkud</label>
        <input type="text" id="search-from" name="from" placeholder="odkud..."></input>
        <label for="search-to">Kam</label>
        <input type="text" id="search-to" name="to" placeholder="kam..."></input>

        <div className="row">
          <div className="column">
        <label for="search-date">Datum</label>
        <input type="date" id="search-date" name="date" placeholder={this.dateNow} />
          </div>
          <div className="column">
        <label for="search-time">Čas</label>
        <input type="time" id="search-time" name="time" placeholder={this.timeNow} />
          </div>
        </div>
        <hr className="solid"/>
        <button type="submit" className="button-submit">Vyhledat</button>

      </form>
    )
  }
}


class ShowTable extends React.Component{
  render(){
    return(
          <table className="show-table">
              <tr>
                  <th>Odchod</th>
                  <th>Odkud</th>
                  <th>Prichod</th>
                  <th>Kam</th>
                  <th>Cena</th>
              </tr>
              <tr>
                  <td>time</td>
                  <td>from</td>
                  <td>time</td>
                  <td>to</td>
                  <td>$$$$</td>
              </tr>
          </table>
    )
  }
}

class MainSearchBlock extends React.Component{
  render (){
    return (

      <div className="main-search-block">
        <div className="row-header">
          <b>VYHLEDEJ SPOJ</b>
        </div>
        <hr className = "solid" />
        <SearchWithLabel />

      </div>
      )
  }
}

function Home() {
  return (
    <>
      <div className="home-window">
        <MainSearchBlock>

        </MainSearchBlock>
      </div>
      <div className="">
      <ShowTable>
      </ShowTable>
      </div>

    </>
  );
}

export default Home;
