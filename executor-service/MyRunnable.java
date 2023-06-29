import java.io.*;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    BufferedReader myReader;
    String path;
    ExecutorService tpe;
    ExecutorService tpe2;
    AtomicInteger counter;
    public AtomicInteger counter2;


    public MyRunnable(BufferedReader myReader, String path, ExecutorService tpe, ExecutorService tpe2, AtomicInteger counter, AtomicInteger counter2) {
        this.myReader = myReader;
        this.path = path;
        this.tpe = tpe;
        this.tpe2 = tpe2;
        this.counter = counter;
        this.counter2 = counter2;
    }

    @Override
    public void run() {
        String orders = path + "/orders.txt";
        String order = null;
        String[] data;
        String id = null;
        Integer number = 0;

        try {
            if ((order = myReader.readLine())!= null) {

                data = order.split(",");
                id = data[0];
                number = Integer.parseInt(data[1]);

                String orderProducts = path + "/order_products.txt";

                AtomicInteger countProducts = new AtomicInteger(0);
                AtomicInteger countTasks = new AtomicInteger(0);

                FileReader myFile2 = new FileReader(orderProducts);

                BufferedReader myReader2 = new BufferedReader(myFile2);
//                System.out.println(number);
                for (int i = 0; i < number; i++) {
                    counter2.incrementAndGet();
                    countTasks.incrementAndGet();
                    tpe2.submit(new MyRunnable2(id, orderProducts, tpe2, counter2, countProducts, countTasks,
                            myReader2));
                }

                while (true) {
                    if (countTasks.get() == 0) {
                        if (countProducts.get() > 0) {
//                            System.out.println("aaa");
                            Tema2.myWriter.write(id + "," + countProducts + ",shipped\n");
//                            myReader2.close();
//                            System.out.println("aaa");
                        } else {
//                            System.out.println("0 produse");
                        }
//                        System.out.println("Aici nu ma blochez");
                        break;
                    }
                }

                counter.incrementAndGet();
                tpe.submit(new MyRunnable(myReader, path, tpe, tpe2, counter, counter2));
//                System.out.println("Task trimis");
            }

            int left = counter.decrementAndGet();
            if (left == 0) {
                Tema2.myWriter.close();
                Tema2.output.close();
//                System.out.println("Task 1 terminat");

                tpe.shutdown();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
