import java.util.ArrayList;
public class Recorrido {
    public static void main(String[] args) {
        ArrayList<Integer> arr = new ArrayList<>();
        arr.add(10);
        arr.add(20);
        arr.add(30);
        System.out.println("Recorrido:");
        for(int num : arr){
            System.out.println(num);
        }
    }
}
