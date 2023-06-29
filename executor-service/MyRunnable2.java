
import java.io.*;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable2 implements Runnable {
    String id;
    String path;
    ExecutorService tpe;
    AtomicInteger counter;

    AtomicInteger countProducts;
    AtomicInteger countTasks;
    BufferedReader myReader;

    public MyRunnable2(String id, String path, ExecutorService tpe, AtomicInteger counter, AtomicInteger countProducts, AtomicInteger countTasks, BufferedReader reader) {
        this.id = id;
        this.path = path;
        this.tpe = tpe;
        this.counter = counter;

        this.countProducts = countProducts;
        this.countTasks = countTasks;
        this.myReader = reader;
    }

    @Override
    public void run() {
        String[] data;
        String order = null;
        String product = null;
        String orderNumber = null;
        try {
            while ((order = myReader.readLine()) != null) {

                String out;
                out = order;
                data = out.split(",");
                orderNumber = data[0];
                product = data[1];
                if (orderNumber.equals(id)) {
                    Tema2.output.write(orderNumber +"," + product + ",shipped\n");
                    countProducts.incrementAndGet();
//                    System.out.println("shipped");
                    break;
                }
            }

//            System.out.println("terminated t2");
            int left = counter.decrementAndGet();
            countTasks.decrementAndGet();


            if (left == 0) {
//                System.out.println("Nivel 2 inchis");
                tpe.shutdown();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}

