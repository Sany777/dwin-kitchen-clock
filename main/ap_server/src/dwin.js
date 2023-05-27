const MAX_FILE_SIZE = 200*1024;
const MAX_FILE_SIZE_STR = "200 kb";

const colorBInput = document.getElementById("color_in_background");
const colorFInput = document.getElementById("color_in_foreground");
const imgInform = document.getElementById("img_inform");
const namesTable = document.getElementById("list_names");
const curImg = document.getElementById("cur_picture");
const imgInput = document.getElementById("new_image");
const tbody = document.getElementById("table_title");
const posPic = document.getElementById("pos_pic");
let imgList = [];
let imgCount = 0;

const title1 = document.getElementById("head_1");
const title2 = document.getElementById("head_2");
const title3 = document.getElementById("head_3");
title1.addEventListener("mousedown", setTitles);
title2.addEventListener("mousedown", setTitles);
title1.addEventListener("mousedown", setTitles);
title3.addEventListener("mousedown", setTitles);
const uploader_1 = document.getElementById("uploader_1");
const uploader_2 = document.getElementById("uploader_2");
const uploader_3 = document.getElementById("uploader_3");
title1.style["opacity"] = 1;


function setTitles(e)
{
  title1.style["opacity"] = 0.5;
  title2.style["opacity"] = 0.5;
  title3.style["opacity"] = 0.5;
  uploader_1.hidden = true;
  uploader_2.hidden = true;
  uploader_3.hidden = true;
  imgInform.hidden = true;
  curImg.hidden = true;
  e.target.style["opacity"] = 1;
  if(e.target === title1){
    imgInform.hidden = false;
    uploader_1.hidden = false;
  } else if(e.target === title2){
    uploader_2.hidden = false;
  } else{
    uploader_3.hidden = false;
  }
}

function setNewImg() 
{
  const file = imgInput.files[0];
  if (file) {
    if (file.name.length < 5) {
      showModal("File name is not set!");
    } else if (file.size > MAX_FILE_SIZE) {
      showModal("File size must be less than " + MAX_FILE_SIZE_STR);
    } else if (file.name.indexOf(" ") >= 0) {
      showModal("File path on server cannot have spaces!");
    } else {
      if(tbody.hidden)tbody.hidden = false;
      const new_tr = document.createElement("tr");
      const newInput = document.createElement("input");
      newInput.type = "number";
      newInput.value = imgCount;
      newInput.min = 0;
      newInput.max = 99;
      newInput.onchange = setPosition;
      newInput.classList.add("input_number");
      const newCellInput = document.createElement("td");
      const newCellInfo = document.createElement("td");
      newCellInput.appendChild(newInput);
      newCellInfo.innerText = file.name;
      new_tr.appendChild(newCellInput);
      new_tr.appendChild(newCellInfo);
      namesTable.appendChild(new_tr);
      imgList.push({
        file: file,
        pos: imgCount,
        nodeInfo: newCellInfo
      });
      newCellInfo.addEventListener("mousedown", markItem);
      imgInput.value = "";
      imgCount++;
    }
  }
}

function getImgData(node)
{
  let imgData = null;
  let indx = -1;
  imgList.forEach((el,i)=>{
    if(el?.nodeInfo === node){
      imgData = imgList[i];
      indx = i;
      return;
    }
  });
  return [imgData, indx];
}

function setPosition(e) 
{
  const curInput = e.currentTarget;
  const curInfo = curInput.parentNode.parentNode.lastChild;
  let newPos = +curInput.value;
  if(newPos > 99 || newPos < 0)newPos = 0;
  const [imgData, i] = getImgData(curInfo);
  imgData.pos = newPos;
  curInput.value = newPos;
  checkDouble();
}


function checkDouble()
{
  const counts = imgList.map(it=>it.pos);
  const countsDouble = counts.filter((el,i) =>counts.indexOf(el) != i);
  const inputNodes = document.getElementsByClassName("input_number");
  for(const node of inputNodes){
    if(countsDouble?.indexOf(+node.value) != -1){
      node.classList = "input_number danger";
    } else{
      node.classList = "input_number";
    }
  }
  if(countsDouble.length){
    showModal("There are several duplicates");
  }
  return !countsDouble.length;
}

let markNode = null;
function markItem(e) 
{
  if (markNode) {
    markNode.classList.remove("selected");
  }
  markNode = e.currentTarget;
  markNode.classList.add("selected");
  const [data, i] = getImgData(markNode);
  if(i === -1)return;
  const path = URL.createObjectURL(data.file);
  imgInform.innerHTML = data.file.name +
    "<br>" +
    " Size " +
    Math.trunc(data.file.size / 1024) +
    " KB";
  if (curImg.hidden) curImg.hidden = false;
  curImg.src = path;
  curImg.alt = data.file.name;
}

function removeItem()
{
  if (markNode){
    markNode.removeEventListener("mousedown", markItem);
    markNode.parentNode.firstChild.firstChild.removeEventListener("onchange", setPosition);
    const [data, rmIndex] = getImgData(markNode);
    namesTable.removeChild(markNode.parentNode);
    if(imgList.length -1 === 0){
      tbody.hidden = true;
    }
    markNode = null;
    imgList.splice(rmIndex, 1);
    curImg.src = "";
    curImg.alt = "";
    imgInform.innerHTML = "";
  } else {
    showModal("You should select a file");
  }
  checkDouble();
}

function sendHello() 
{
  sendDataForm("hello");
}

function sendClear()
{
  sendDataForm("clear");
}

function sendSavePic()
{
  sendDataForm("savepic/"+posPic.value);
}
function sendColor() 
{
  const dataB = colorBInput.value.slice(1,5);
  const dataF = colorFInput.value.slice(1,5);
  const dataForm = new FormData();
  dataForm.append("background", dataB);
  dataForm.append("foreground", dataF);
  sendDataForm("color", dataForm);
}

function sendBoot() 
{
  const file = bootIn.files[0];
  if(!file){
    showModal("File boot is not set!", false);
  }else if(file.length === 0){
    showModal("The filename wrong!", false);
  } else if(file.size>MAX_FILE_SIZE){
    showModal("The boot file is large", false);
  } else {
    sendDataForm("updatedwin", file);
  }
}

function sendImg() 
{
  if (imgList.length == 0) {
    showModal("The file list is empty");
  } else if(checkDouble()){
    imgList.forEach((elm) => {
      sendDataForm("img/" + elm.pos, elm.file);
    });
  }
}