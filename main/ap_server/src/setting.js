const LIST_DAY = ["Monday","Thusday","Wednesday","Thursday","Friday","Saturday","Sanday"]
const NOTIF_PER_DAY = 4;
const FORMS_LIST = [
  [["Network",["SSID", "PWD"]],["text","32"]],
  [["API", ["City", "Key"]],["text","32"]],
  [["Update", ["Firmware"]],["file",]],
]

function createNotification()
{
  const form = document.createElement("form");
  form.name = "Notification";
  form.action = "";
  const fieldset = document.createElement("fieldset");
  const legend = document.createElement("legend");
  legend.innerText = "Notification";
  fieldset.appendChild(form);
  fieldset.appendChild(legend);
  const tr_head = document.createElement("tr");
  const table = document.createElement("table");
  LIST_DAY.forEach((e,i)=>{
    const th = document.createElement("th");
    th.innerText = e;
    tr_head.appendChild(th);
  })
  const tr_body = document.createElement("tbody");
  for(let row=0; row<NOTIF_PER_DAY; row++){
    const tr_data = document.createElement("tr");
    for(let day=0; day<LIST_DAY.length; day++){
      const td = document.createElement("td");
      const input = document.createElement("input");
      input.type = "time";
      input.name = "notif_"+day+"_"+row;
      input.value = "";
      td.appendChild(input);
      tr_data.appendChild(td);
    }
    tr_body.appendChild(tr_data);
  }
  table.appendChild(tr_head);
  table.appendChild(tr_body);
  const submit = document.createElement("input");
  submit.value = "Submit";
  submit.type = "submit";
  form.appendChild(table);
  form.appendChild(submit);
  document.getElementById("settings_forms").appendChild(fieldset);
}



function getSetting()
{
  fetch("/data?", {
    method:"GET",
    mode: "no-cors",
    body: null,
  })
  .then((r) => r.json())
  .then((r) => {
    for(const key in r){
      const value = r[key];
      if(key === "Notification") {
        const notifs = document.forms["Notification"].elements;
        let iter=0;
        for(const inp of notifs){
          if(inp.type === "time"){
            inp.value =  value[iter++] + value[iter++] + ":" + value[iter++]+value[iter++];
          }
        }
       } else {
         const input = document.getElementById(key);
         if(input)input.value = value;
      }
    }
   })
  .catch((e) => showModal(e));
}

function createForms()
{
  const containerForms = document.getElementById("settings_forms");
  FORMS_LIST.forEach((data, i)=>{
    const [name, listInput] = data[0];
    const [type, length] = data[1];
    const form = document.createElement("form");
    const container = document.createElement("div");
    const fieldset = document.createElement("fieldset");
    const legend = document.createElement("legend");
    legend.innerText = name;
    fieldset.appendChild(form);
    fieldset.appendChild(legend);
    form.name = name;
    form.action = "";
    const submit = document.createElement("input");
    submit.value = "Submit";
    submit.type = "submit";
    listInput.forEach(inputName=>{
      const label = document.createElement("label");
      label.innerText = inputName;
      const input = document.createElement("input");
      input.type = type;
      input.id = inputName;
      input.name = inputName;
      input.value = "";
      if(type === "text"){
        input.maxLength = length;
        input.placeholder = "Enter "+ inputName;
      }
      label.appendChild(input);
      form.appendChild(label);
    })
    form.appendChild(submit);
    container.appendChild(fieldset);
    containerForms.appendChild(container);
  });
}

function updateTime() 
{
  const data = "" + Math.trunc(new Date().getTime());
  sendDataForm("time", data);
}

async function getInfo()
{
  await fetch("/getinfo", {
    method: "GET",
    mode: "no-cors",
    body: null,
  })
    .then((r) => r.json())
    .then((r) => showModal("version idf: " + r.version + "\n"
               +"chip: " + r.chip + "\n"
               + "revision: " + r.revision, true))
    .catch((e) => showModal(e, false));
}

createForms();
createNotification();