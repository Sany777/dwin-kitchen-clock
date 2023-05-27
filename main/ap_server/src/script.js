
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



function getDataForm(e) 
{
  const form = e.currentTarget.parentNode;
  let data = null;
  if(form.name === "Update"){
    data = form.Firmware.files[0];
  } else if(form.name === "Notification"){
    const childsList = document.forms["Notification"];
    let list = [];
    for(const child of childsList){
      if(child.type === "time"){
        list.push(child.value);
      }
    }
    data = list;
  } else {
    const  js = {};
    const childsList = document.forms[form.name];
    if(childsList){
      for(const child of childsList){
        if(child.type === "text" || child.type === "number"){
          js[child.name] = child.value;
        }
      }
      data = JSON.stringify(js);
    }
  }
  sendDataForm(form.name, data);
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
