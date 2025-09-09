let f = 3, c = 3;
let twoDarr = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
];

console.log("RECORRIDO DE FILAS");

for (let i = 0; i < f; i++) {
    let fila = "Fila " + (i+1) + ": ";
    for (let j = 0; j < c; j++) {
        fila += twoDarr[i][j] + " ";
    }
    console.log(fila);
}
