
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
  let flags = 0;
  let i=0;
  const childsList = document.forms[formName];
  if(childsList){
    for(const child of childsList){
        let value = child.value;
        if(value){
          if(child.type === "text"||child.type === "number"){
            if(!data)data = js;
            js[child.name] = value;
          } else if(child.type === "color"){
            if(!data)data = js;
            const red = parseInt(value.substring(1, 3), 16)>>3;
            const green = parseInt(value.substring(3, 5), 16)>>2;
            const blue = parseInt(value.substring(5, 7), 16)>>3;
            let color = red<<11 | green<<5 | blue;
            js[child.name] = color;
          } else if(child.type === "file"){
            data = child.files[0];
            break;
        } else if(child.type === "time"){
            if(!data)data = arr;
            let arr_time = value.split(':');
            let hour = +arr_time[0];
            let min = +arr_time[1];
            let sec = +arr_time[2];
            arr.push(Math.trunc(hour/10)+""+hour%10+"");
            arr.push(Math.trunc(min/10)+""+min%10+"");
            arr.push(Math.trunc(sec/10)+""+sec%10+"");
        } else if(child.type === "textarea"){
          data = value+"";
          break;
        } else if(child.type === "checkbox"){
          if(child.disabled){
            data = flags;
            break;
          }
          if(child.checked){
            flags |= 1<<i;
          }
          i++;
        }
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
