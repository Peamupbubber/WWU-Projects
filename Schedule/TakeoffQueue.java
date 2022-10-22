import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;

public class TakeoffQueue {
    public static void main(String[] args) throws IOException {
        
        FileReader fileReader = new FileReader(args[0]);
        Scanner inFile = new Scanner(fileReader);

        //Calls the priority based on the number specified
        switch(Integer.parseInt(args[1])) {
            case(1): {
                simplePriority(inFile);
                break;
            }
            case(2): {
                intermediatePriority(inFile);
                break;
            }
            case(3): {
                fullPriority(inFile);
                break;
            }
        }

        inFile.close();
    }

    //Priority option number 1
    private static void simplePriority(Scanner inFile) {
        //Creates the HeapMin based on user input
        HeapMin heap = new HeapMin(1);
        int positionAddedd = 0;
        while(inFile.hasNextLine()) {
            heap.insert(new Schedule(inFile.next(), inFile.next(), inFile.next(),
                                    inFile.next(), inFile.nextInt(), positionAddedd),
                                    1);
            positionAddedd++;
        }

        while(heap.extractMin() != null) {
            System.out.println(heap.removeMin(1).toString(1, -1));
        }
    }

    //Priority option number 2
    private static void intermediatePriority(Scanner inFile) {
        //Creates the HeapMax based on user input
        HeapMax heap = new HeapMax(1);
        int positionAddedd = 0;
        while(inFile.hasNextLine()) {
            heap.insert(new Schedule(inFile.next(), inFile.next(), inFile.next(),
                                    inFile.next(), inFile.nextInt(), positionAddedd),
                                    2);
            positionAddedd++;
        }

        while(heap.extractMax() != null) {
            System.out.println(heap.removeMax(2).toString(2, -1));
        }
    }

    //Priority option number 3
    private static void fullPriority(Scanner inFile) {
        //Creates the HeapMin based on user input
        HeapMin heap = new HeapMin(1);
        int positionAddedd = 0;
        while(inFile.hasNextLine()) {
            heap.insert(new Schedule(inFile.next(), inFile.next(), inFile.next(),
                                    inFile.next(), inFile.nextInt(), positionAddedd),
                                    3);
            positionAddedd++;
        }

        HeapMax runway = new HeapMax(1);

        runway.insert(heap.removeMin(3), 2);
        int time = runway.extractMax().takeOffRequestTimeInt + runway.extractMax().takeOffTime;
        System.out.println(runway.removeMax(2).toString(3, time));

        while(heap.extractMin() != null) {
            while(heap.extractMin() != null && heap.extractMin().takeOffRequestTimeInt <= time) {
                runway.insert(heap.removeMin(3), 2);
            }
            if(runway.extractMax() != null) {
                time += runway.extractMax().takeOffTime;
                System.out.println(runway.removeMax(2).toString(3, time));
            }
            else
                runway.insert(heap.removeMin(3), 2);
        }

        while(runway.extractMax() != null) {
            time += runway.extractMax().takeOffTime;
            System.out.println(runway.removeMax(2).toString(3, time));
        }
    }
}