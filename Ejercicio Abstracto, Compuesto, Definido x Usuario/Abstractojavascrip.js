class Pila {
  constructor() {
    this.datos = [];
  }
  push(x) {
    this.datos.push(x);
  }
  pop() {
    return this.datos.pop();
  }
}

let p = new Pila();
p.push(5);
p.push(10);
console.log(p.pop());
