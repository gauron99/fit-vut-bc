import React from "react";

export const GeneralErrorPage = () => {
  return (
    <h3>Obecná chyba.</h3>
  )
}

class SearchWithLabel extends React.Component{
  constructor(props){
    super(props);
    var today = new Date();
    
    // months are indexed from 0 for some reason
    this.dateNow = today.getFullYear() +'-'+ (today.getMonth()+1) +'-'+ today.getDate();
    this.timeNow = ("0" + today.getHours()).slice(-2) + ":" + 
                    ("0" + today.getMinutes()).slice(-2) + ":" +
                    ("0" + today.getSeconds()).slice(-2);
  }
  render(){
    console.log("timenow: ",this.timeNow)
    console.log("datenow: ",this.dateNow)

    return(
      <form>
        <label htmlFor="search-from">Odkud</label>
        <input type="text" id="search-from" name="from" placeholder="odkud..."></input>
        <label htmlFor="search-to">Kam</label>
        <input type="text" id="search-to" name="to" placeholder="kam..."></input>

        <div className="row">
          <div className="column">
        <label htmlFor="search-date">Datum</label>
        <input type="date" id="search-date" name="date" defaultValue={this.dateNow} />
          </div>
          <div className="column">
        <label htmlFor="search-time">Čas</label>
        <input type="time" id="search-time" name="time" defaultValue={this.timeNow} />
          </div>
        </div>
        <hr className="solid"/>
        <button type="button" onClick={this.props.funkca} className="button-submit">Vyhledat</button>
      </form>
    )
  }
}


class ShowTable extends React.Component{
  render(){
    return(
          <table className="show-table">
              <thead>
                  <th>Odchod</th>
                  <th>Odkud</th>
                  <th>Prichod</th>
                  <th>Kam</th>
                  <th>CenaPoor</th>
                  <th>CenaRich</th>
              </thead>
              <tbody>
                    {this.props.veci.map(comp => <tr>{comp.map(compy => <td>{compy}</td>)}</tr>)}
              </tbody>
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
        <SearchWithLabel funkca={this.props.funkca} veci={this.props.veci}/>

      </div>
      )
  }
}

class WrapPico extends React.Component{
    constructor(props){
        super(props);
        
        this.vyhledatSpoj = this.vyhledatSpoj.bind(this);
    }
    state = {
        veci: [[]]
    }
    vyhledatSpoj() {
        var xhr = new XMLHttpRequest()
        var odkud = document.getElementById("search-from").value;
        var kam = document.getElementById("search-to").value;
        var kdy = document.getElementById("search-time").value;
        console.log(odkud,kam,kdy);
        // get a callback when the server responds
        xhr.addEventListener('load', () => {
        // update the state of the component with the result here
            var spoje = JSON.parse(xhr.responseText);
            var stateSpoje = [];
            for (let spoj of spoje){
                stateSpoje.push(Object.values(spoj));
            }
            console.log(stateSpoje);
            this.setState({veci: stateSpoje});
        })
        // open the request with the verb and the url
        xhr.open('GET', '/api/spoje?odkud='+odkud+'&kam='+kam+'&kdy='+kdy);
        // send the request
        xhr.send()
    }

    render (){
        return (
            <div>
            <div className="home-window">
            <MainSearchBlock funkca={this.vyhledatSpoj} veci={this.state.veci}/>
        </div>
        <div className="">
        <ShowTable veci={this.state.veci}/>
        </div>
        </div>
        )
  }
}

function Home() {
    return (
    <>
      <WrapPico />
    </>
  );
}

export default Home;
