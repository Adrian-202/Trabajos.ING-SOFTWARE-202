public class App {
    public static void main(String[] args) {
        String[] apellidos = new String[5];

        apellidos[0] = "gonzalez";
        apellidos[1] = "lopez";
        apellidos[2] = "romo";
        apellidos[3] = "barraza";
        apellidos[4] = "gibran";

        for (int i = 0; i < apellidos.length; i++) {
            System.out.println(apellidos[i]);
        }
    }
}
