import java.util.ArrayList;
public class Insercion {
    public static void main(String[] args) {
        ArrayList<Integer> arr = new ArrayList<>();
        arr.add(10);
        arr.add(20);
        arr.add(30);
        arr.add(1, 15);
        System.out.println("Después de insertar 15: " + arr);
    }
}
