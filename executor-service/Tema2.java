import java.io.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Tema2 {
    public static Integer maxThreads = 0;
    public static BufferedReader myReader;
    public static FileReader myFile;
    public static BufferedWriter myWriter;
    public static BufferedWriter output;

    public static void main(String[] args) throws IOException {

        File myObj = new File("orders_out.txt");
        if (myObj.createNewFile()) {
            //System.out.println("obj created1");
        }
        File myObj2 = new File("order_products_out.txt");
        if (myObj2.createNewFile()) {
            //System.out.println("obj created2");
        }
        maxThreads = Integer.parseInt(args[1]);
        String path = args[0];
        String orders = path + "/orders.txt";
        try {
             myFile = new FileReader(orders);
             myReader = new BufferedReader(myFile);
             output = new BufferedWriter(new FileWriter("order_products_out.txt"));
             myWriter = new BufferedWriter(new FileWriter("orders_out.txt"));

            ExecutorService tpe = Executors.newFixedThreadPool(maxThreads);
            ExecutorService tpe2 = Executors.newFixedThreadPool(maxThreads);
            AtomicInteger counter = new AtomicInteger(0);
            AtomicInteger counter2 = new AtomicInteger(0);
            for (int i = 0; i < maxThreads; i++) {
                counter.incrementAndGet();
                tpe.submit(new MyRunnable(myReader, path, tpe, tpe2, counter, counter2));
//                System.out.println("Task trimis");
            }

        } catch (FileNotFoundException e) {
            System.out.println("A READ error occurred.");
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
