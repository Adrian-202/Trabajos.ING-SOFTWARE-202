function clasificarBurbuja(pila) {
    let largo = pila.length;
    let hubo_cambio;
    let nivel = 0;
    do {
        hubo_cambio = false;
        for (let m = 0; m < largo - 1 - nivel; m++) {
            if (pila[m] > pila[m + 1]) {
                let sustituto = pila[m];
                pila[m] = pila[m + 1];
                pila[m + 1] = sustituto;
                hubo_cambio = true;
            }
        }
        nivel++;
    } while (hubo_cambio);
}

let mi_arreglo = [1, 4, 2, 3, 5, 8, 6];
clasificarBurbuja(mi_arreglo);
console.log("Resultado: " + mi_arreglo.join(" "));