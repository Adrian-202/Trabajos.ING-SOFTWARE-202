enum Numero { UNO, DOS, TRES, CUATRO, CINCO }

public class Main {
    public static void main(String[] args) {
        Numero n = Numero.TRES;
        Numero m = Numero.CINCO;

        System.out.println("n vale: " + n);
        System.out.println("m vale: " + m);

        if (n == Numero.TRES) {
            System.out.println("n es igual a TRES");
        }
    }
}
