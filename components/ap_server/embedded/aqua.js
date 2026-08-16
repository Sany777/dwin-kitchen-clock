const configAquarium = {
  WIDTH: 1000,
  HEIGHT: 600,
  run: true,
  root: null,
  fishes: null,
};

let once_click = false;

const POS_STEM_1 = 111;
const POS_STEM_2 = 600;

const delay = (time) => {
  return new Promise((res) => {
    setTimeout(() => {
      res("ok");
    }, time);
  });
};

const creatorStem = (left) => {
  const stem = document.createElement("div");
  stem.classList.add("stem");
  stem.setAttribute("id", "stem");
  stem.style.left = left + "px";
  stem.innerHTML = `<div class="laminaria l-1"></div>
                    <div class="laminaria l-2"></div>`;
  return stem;
};

const creatorColobok = () => {
  const colobok = document.createElement("div");
  colobok.classList.add("border-colobok");
  colobok.innerHTML = `
                          <div class="fin-colobok fin-colobok-1"></div>
                          <div class="fin-colobok fin-colobok-2"></div>
                          <div class="fin-colobok-center"></div>
                          <div class="colobok" id="colobok">
                          <div class="container-eye">
                            <div class="eye">
                              <div class="pupil colobok-pupil"></div>
                            </div>
                            <div class="eye">
                              <div class="pupil colobok-pupil"></div>
                            </div>
                          </div>
                            <div class="container-nose">
                              <div class="nouse"></div>
                              <div class="nouse"></div>
                            </div>
                            <div class="smile-box"></div>
                          </div>`;
  return colobok;
};

class Fish {
  constructor(config) {
    this.WIDTH = config.WIDTH;
    this.HEIGHT = config.HEIGHT;
    this.maxStep = 0;
    this.item = null;
    this.tail = null;
    this.pupil = null;
    this.directX = false;
    this.prevPos = {};
  }
  move() {
    if (!this.prevPos.x) {
      const [xc, yc] = this.getPosition();
      this.prevPos.x = xc;
      this.prevPos.y = yc;
    }

    let x = parseInt(Math.random() * this.maxStep + this.maxStep / 2);
    let y = parseInt(Math.random() * this.maxStep);
    if (x + this.prevPos.x > this.WIDTH || this.prevPos.x - x < 0) {
      x += this.maxStep;
      this.directX = !this.directX;
      this.item.classList.toggle("rotate");
    }

    if (this.directX) {
      x = -x;
    }

    y += this.prevPos.y;
    if (y + this.maxStep * 2.3 > this.HEIGHT) {
      y -= this.maxStep * 1.5;
    } else if (y < this.maxStep * 1.4) {
      y += this.maxStep * 1.5;
    }

    x += this.prevPos.x;
    if (this.tail) {
      this.moveTail();
      this.setNewDuration();
    }
    this.moveTo(x, y);
  }

  movePupil(x, y) {
    const [left, top] = this.getPosition();
    const yStep = Math.abs(top - y) > this.maxStep / 2;
    const xStep = Math.abs(left - x) > this.maxStep / 2;
    if (yStep || xStep) {
      if (xStep) {
        let corectionX = false;
        if (left < x) {
          corectionX = true;
        }
        if (this.directX) corectionX = !corectionX;
        if (corectionX) this.pupil.style["left"] = "50%";
        else this.pupil.style["left"] = "5%";
      }
      if (yStep) {
        if (top < y) {
          this.pupil.style["top"] = "40%";
        } else {
          this.pupil.style["top"] = "5%";
        }
      }

      delay(500).then(() => {
        this.pupil.style["left"] = "25%";
        this.pupil.style["top"] = "25%";
      });
    }
  }

  moveTail() {
    const lastBR = this.tail.style.borderRadius;
    const lastH = this.tail.style.height;
    this.tail.style.borderRadius = "35%";
    this.tail.style.height = "200%";

    setTimeout(() => {
      this.tail.style.height = lastH;
      this.tail.style.borderRadius = lastBR;
    }, 400);
  }

  moveTo(x, y) {
    if (x + this.maxStep > this.WIDTH) {
      x = this.WIDTH - this.maxStep;
    }
    if (y + this.maxStep * 1.3 > this.HEIGHT)
      y = this.HEIGHT - this.maxStep * 2;
    if (x < 0) x = 10;
    if (y < this.maxStep) y = this.maxStep;
    this.item.style.left = x + "px";
    this.item.style.top = y + "px";
    this.prevPos.x = x;
    this.prevPos.y = y;
  }

  setNewDuration(duration) {
    if (!duration) duration = parseInt(Math.random() * 5 + 1);
    this.item.style["transition-duration"] = "left top" + duration + "s";
  }

  beyondX(x) {
    return (
      parseInt(this.item.style.left) + this.maxStep * 1.6 + x < this.WIDTH &&
      parseInt(this.item.style.left) > this.maxStep * 1.7
    );
  }

  getPosition() {
    return [
      parseInt(this.item.style.left) + this.maxStep / 2,
      parseInt(this.item.style.top) + this.maxStep / 3,
    ];
  }
}

function createMonster() {
  const monster = document.getElementsByClassName("eye");
  monster[0].style.position = "absolute";
  monster[0].style.opacity = "0.3";
}

function creatorItem(className, color1, color2) {
  const item = document.createElement("div");
  item.classList.add(className);
  if (color1) {
    if (!color2) color2 = color1;
    item.style.backgroundImage = `linear-gradient(45deg, ${color1}, ${color2})`;
  }
  return item;
}

function CreatorFishes(cFish, funcCreatorItem, config) {
  const creatorFishe = (x, y, colors, size, id) => {
    const item = funcCreatorItem("fishe");
    item.style.width = size + "px";
    item.style.height = parseInt(size / (Math.random() * 1.5 + 1.5)) + "px";
    item.id = "fishe-" + id;
    item.style["z-index"] = id + 1;
    const line1 = funcCreatorItem("line", colors[0], colors[1]);
    const line2 = funcCreatorItem("line", colors[0], colors[1]);
    const fin = funcCreatorItem("fin");
    fin.style["border-color"] = colors[1];
    fin.style["z-index"] = id;
    fin.style["border-width"] = parseInt(size / (Math.random() * 4 + 2)) + "px";
    const tail = funcCreatorItem("tail");
    tail.style["border-width"] = parseInt(size / 4) + "px";
    tail.style["z-index"] = id;
    tail.style["border-color"] = colors[2];
    const eye = funcCreatorItem("eye");
    eye.style.width = size / 9 + "px";
    eye.style.height = size / 9 + "px";
    const pupil = funcCreatorItem("pupil");
    eye.append(pupil);
    const bodyFish = funcCreatorItem("bodyFish", colors[1], colors[2]);
    bodyFish.style["z-index"] = id + 1;
    bodyFish.append(tail, eye, line1, line2);
    item.append(fin, bodyFish);
    item.style.left = x + "px";
    item.style.top = y + "px";
    let directX = false;
    if (Math.random() * 2 > 1) {
      item.classList.add("rotate");
      directX = true;
    }
    if (Math.random() * 3 > 1) item.directY = true;
    const maxStep = size * 1.5;
    return [item, tail, pupil, maxStep, directX];
  };

  return function (number, size) {
    let fishesList = [];
    if (!size) size = parseInt(Math.random() * 50 + 75);
    for (let i = 0; i < number; i++) {
      let fish = new cFish(config);
      const x = parseInt((Math.random() * config.WIDTH) / 2 + size * 1.2);
      const y = parseInt((Math.random() * config.HEIGHT) / 2.3 + size);
      const colors = [];
      for (let t = 0; t < 3; t++) {
        colors.push(
          "rgb(" +
            parseInt(Math.random() * 255) +
            ", " +
            parseInt(Math.random() * 200) +
            ", " +
            parseInt(Math.random() * 200) +
            ")"
        );
      }
      [
        fish.item,
        fish.tail,
        fish.pupil,
        fish.maxStep,
        fish.directX,
      ] = creatorFishe(x, y, colors, size, fishesList.length);
      fishesList[i] = fish;
      config.root.append(fish.item);
    }
    return fishesList;
  };
}

const creatorBall = (WIDTH) => {
  const ball = creatorItem("ball");
  ball.style.left = `${Math.random() * (WIDTH - 200) + 100}px`;
  const size = `${Math.random() * 20 + 5}px`;
  ball.style.height = size;
  ball.style.width = size;
  return ball;
};

function aquariumRun(config, timeInterval = 0) {
  config.fishes.forEach((item) => {
    const time = Math.random() * 4000 + 1000;
    setTimeout(() => {
      item.move();
    }, time);
  });
  setTimeout(() => {
    moveBall(config);
    timeInterval = Math.random() * 3000 + 5000;
    aquariumRun(config, timeInterval);
  }, timeInterval);
}

const Aquarium = creatorItem("aquarium");
Aquarium.style.width = configAquarium.WIDTH + "px";
Aquarium.style.height = configAquarium.HEIGHT + "px";
configAquarium.root = Aquarium;
const creatorFishes = CreatorFishes(
  Fish,
  creatorItem,
  configAquarium,
  configAquarium.WIDTH,
  configAquarium.HEIGHT
);

configAquarium.fishes = creatorFishes(10);
Aquarium.append(creatorStem(POS_STEM_1), creatorStem(POS_STEM_2));
Aquarium.append(creatorItem("stone-1"), creatorItem("stone-2"));
Aquarium.append(
  creatorItem("oval-1"),
  creatorItem("oval-2"),
  creatorItem("oval-3")
);

const round_fishe = new Fish(configAquarium);
round_fishe.item = creatorColobok();
Aquarium.append(round_fishe.item);
round_fishe.maxStep = 100;

function moveBall(config) {
  const ball = creatorBall(config.WIDTH);
  config.root.append(ball);
  delay(1000).then(() => {
    ball.style.top = 0;
    ball.style.height = +ball.style.height.slice(0, 2) / 1.5 + "px";
    ball.style.width = +ball.style.width.slice(0, 2) * 1.5 + "px";
  });
  delay(5700).then(() => {
    config.root.removeChild(ball);
  });
}

function moveColobok(e, col) {
  let x = e.offsetX;
  let y = e.offsetY;
  const pupilsColobok = document.querySelectorAll(".colobok-pupil");
  let marY = (y - col.getPosition()[1]) / 8 + "%";
  let marX = (x - col.getPosition()[0]) / 12 + "%";
  for (const pupil of pupilsColobok) {
    let prevY = pupil.style["margin-top"];
    let prevX = pupil.style["margin-left"];
    pupil.style["margin-top"] = marY;
    pupil.style["margin-left"] = marX;
    pupil.style["margin-top"] = setTimeout(() => {
      pupil.style["margin-top"] = prevY;
      pupil.style["margin-left"] = prevX;
    }, 1000);
  }
  moveFins(x, col);
  scaleEys();
  col.moveTo(x, y);
}

Aquarium.addEventListener("click", (e) => {
  if (!once_click) {
    createMonster();
    once_click = true;
  }
  configAquarium.fishes.forEach((item) => {
    item.movePupil(e.offsetX, e.offsetY);
  });
  moveColobok(e, round_fishe);
});

function moveFins(x, colobok) {
  let fins = document.querySelectorAll(".fin-colobok");
  let finMove = [];
  if (x - colobok.getPosition()[0] > colobok.maxStep) {
    finMove.push(fins[0]);
  } else if (colobok.getPosition()[0] - x > colobok.maxStep) {
    finMove.push(fins[1]);
  } else {
    finMove = fins;
  }
  finMove.forEach((element) => {
    element.classList.add("position-1");
  });
  setTimeout(() => {
    finMove.forEach((element) => {
      element.classList.remove("position-1");
    });
  }, 500);
}

function scaleEys(e) {
  const eyes = document.querySelectorAll(".container-eye .eye");
  eyes.forEach((element) => {
    element.classList.add("eyeScale");
  });
  setTimeout(() => {
    eyes.forEach((element) => {
      element.classList.remove("eyeScale");
    });
  }, 1000);
}

aquariumRun(configAquarium);
const content = document.getElementById("aquarium");
content.appendChild(Aquarium);
