class Pila {
    int[] datos = new int[5];
    int tope = -1;

    void push(int x) { datos[++tope] = x; }
    int pop() { return datos[tope--]; }
}
