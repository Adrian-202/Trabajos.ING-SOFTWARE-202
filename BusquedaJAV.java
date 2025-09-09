import java.util.ArrayList;
public class Busqueda {
    public static void main(String[] args) {
        ArrayList<Integer> arr = new ArrayList<>();
        arr.add(10); arr.add(15); arr.add(20); arr.add(30);
        int buscado = 20;
        int indice = arr.indexOf(buscado);
        if(indice != -1){
            System.out.println(buscado + " encontrado en índice " + indice);
        } else {
            System.out.println(buscado + " no encontrado");
        }
    }
}
