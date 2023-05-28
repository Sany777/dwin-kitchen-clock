
const modal = window.document.getElementById("modal");

function serverExit() 
{
  sendDataForm("close");
  document.getElementById("exit").classList.add("danger");
}

function reloadPage() 
{
  location.reload();
}

function showModal(str, success) 
{
  if(str == "TypeError: Failed to fetch"){
    str = "No connection";
  }
  modal.classList.add("show");
  if(success)modal.style["background-color"] = "green";
  else modal.style["background-color"] = "red";
  modal.innerText = str;
  modal.style.marginTop = window.pageYOffset / 2 + "px";
  modal.style.left = window.innerWidth / 3 + "px";
  modal.style.marginTop = "50%";
  setTimeout(() => {
    modal.style.marginTop = "150px";
    modal.classList.remove("show");
  }, 3500);
}


document.body.addEventListener("submit", (e) => {
  e.preventDefault();
  e.stopPropagation();
  sendData(e.target.name);
});

function sendData(formName)
{
  const  js = {};
  const arr = [];
  let data = null;
  const childsList = document.forms[formName];
  if(childsList){
    for(const child of childsList){
        if(child.type === "text" || child.type === "number"){
          if(!data)data = js;
          js[child.name.slice(0, 3)] = child.value;
        } else if(child.type === "color"){
          if(!data)data = js;
          js[child.name] = child.value.slice(1,5);
        } else if(child.type === "file"){
          data = child.files[0];
          break;
      } else if(child.type === "time"){
          if(!data)data = arr;
          let endHour = child.value.indexOf(':');
          let hour = +child.value.slice(0, endHour);
          let min = +child.value.slice(endHour+1,);
          arr.push(Math.trunc(hour/10)+""+hour%10+"");
          arr.push(Math.trunc(min/10)+""+min%10+"");
      }
    }
  }
  if(data === js){
    data = JSON.stringify(js);
  } else if(data === arr){
    data = arr.join('');
  }
  sendDataForm(formName, data);
}


async function sendDataForm(path, data) 
{
  let res = true;
  await fetch("/"+ path, {
    method: "POST",
    mode: "no-cors",
    body: data,
  })
    .then((r) => {
      if(!r.ok)res = false; 
      return r.text()
    })
    .then((r) => showModal(r, res))
    .catch((e) => showModal(e, false));
}
