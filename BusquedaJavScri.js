let arr = [10, 15, 20, 30];
let buscado = 20;
let indice = arr.indexOf(buscado);
if(indice !== -1){
    console.log(buscado + " encontrado en índice " + indice);
} else {
    console.log(buscado + " no encontrado");
}
