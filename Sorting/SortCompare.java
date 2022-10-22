import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

/* Sources:
 * I used this website to figure out how to generate the random values of the input array
 * https://www.educative.io/edpresso/how-to-generate-random-numbers-in-java
*/

public class SortCompare {

    static Scanner scan = new Scanner(System.in);

    static int insertionCompare = 0;
    static int mergeCompare = 0;
    static int quickCompare = 0;

    private static class Bucket {

        static List<Integer>[] bucket;

        public Bucket() {
            bucket = new ArrayList[19];

            for(int i = 0; i < bucket.length; i++) {
                bucket[i] = new ArrayList<Integer>();
            }

        }
        
        private List<Integer> get(int index) {
            return bucket[9 + index];
        } 

    }

    public static void main(String[] args) {
        
        System.out.println("Input Params");
        System.out.println("============");
        System.out.println("How many entries?");

        int n = scan.nextInt();
        int[] array = newArray(n);

        System.out.println("Which sort [m, i, q, r, all]?");

        switch(scan.next()) {

            case("m"):
                mergeSort(array, n);
                break;

            case("i"):
                insertionSort(array, n);
                break;

            case("q"):
                quickSort(array, n);
                break;

            case("r"):
                radixSort(array, n);
                break;

            case("all"):
                int[] tempArray = new int[array.length];
                for(int i = 0; i < tempArray.length; i++)
                    tempArray[i] = array[i];
                mergeSort(tempArray, n);
                System.out.println();

                tempArray = new int[array.length];
                for(int i = 0; i < tempArray.length; i++)
                    tempArray[i] = array[i];
                insertionSort(tempArray, n);
                System.out.println();

                tempArray = new int[array.length];
                for(int i = 0; i < tempArray.length; i++)
                    tempArray[i] = array[i];
                quickSort(tempArray, n);
                System.out.println();

                tempArray = new int[array.length];
                for(int i = 0; i < tempArray.length; i++)
                    tempArray[i] = array[i];
                radixSort(tempArray, n);
                System.out.println();
                
                break;
        }
    }

    //All four sorts have an int method that will run them and print results
    private static void quickSort(int[] array, int n) {

        System.out.println();
        System.out.println("quick sort");
        System.out.println("=========="); 
        
        if(n < 20) {
            System.out.print("Unsorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
                     
            quickSort(array);
            System.out.println();
            System.out.println("Num Comparisons: " + quickCompare);
 
            System.out.print("Sorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");

        }
        else {
            quickSort(array);
            System.out.println("Num Comparisons: " + quickCompare);
        }    
    }

    private static void mergeSort(int[] array, int n) {

        System.out.println();
        System.out.println("merge sort");
        System.out.println("==========");

        if(n < 20) {
            System.out.print("Unsorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
                    
            mergeSort(array);
            System.out.println();
            System.out.println("Num Comparisons: " + mergeCompare);

            System.out.print("Sorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
        }
        else {
            mergeSort(array);
            System.out.println("Num Comparisons: " + mergeCompare);
        }
    }

    private static void insertionSort(int[] array, int n) {
        
        System.out.println();
        System.out.println("insertion sort");
        System.out.println("=============="); 
        
        if(n < 20) {
            System.out.print("Unsorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
                     
            insertionSort(array);
            System.out.println();
            System.out.println("Num Comparisons: " + insertionCompare);
 
            System.out.print("Sorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
        }
        else {
            insertionSort(array);
            System.out.println("Num Comparisons: " + insertionCompare);
        }
    }

    private static void radixSort(int[] array, int n) {
     
        System.out.println();
        System.out.println("radix sort");
        System.out.println("=========="); 
        
        if(n < 20) {
            System.out.print("Unsorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
                     
            radixSort(array);
            System.out.println();
            System.out.println("Num Comparisons: 0");
 
            System.out.print("Sorted array: ");
            for(int i = 0; i < array.length; i++)
                System.out.print(array[i] + " ");
        }
        else {
            radixSort(array);
            System.out.println("Num Comparisons: 0");
        }
    }

    private static int[] newArray(int n) {

        int[] array = new int[n];
        
        int max = n;
        int min = -1 * n;

        for(int i = 0; i < n; i++) {
            array[i] = (int)Math.floor(Math.random()*(max - min + 1) + min);
        }

        return array;
    }

    private static void quickSort(int[] array) {
        quickSort(array, 0, array.length);
    }

    private static void quickSort(int[] array, int start, int end) {

        if(end - start < 2)
            return;

        int mid = partition(array, start, end);

        quickSort(array, start, mid);
        quickSort(array, mid + 1, end);
    }

    private static int partition(int[] array, int start, int end) {

        int pivot = start;

        int i = start + 1;
        

        
        for(int k = start + 1; k < end; k++) {
            if(array[k] <= array[pivot]) {
                swap(array, i, k);
                i++;
            }
            quickCompare++;
        }
        swap(array, pivot, i - 1);

        return i - 1;

    }

    //Swaps the two elements in the given array
    private static void swap(int[] array, int index1, int index2) {

        int temp = array[index1];
        
        array[index1] = array[index2];
        array[index2] = temp;
    }

    //Merge Sort method with only the array input
    private static void mergeSort(int[] array) {
        mergeSort(array, 0, array.length);
    }

    //Recurssive Merge Sort algorithm, sorts an unsorted array of ints
    private static void mergeSort(int[] array, int start, int end) {

        if(end - start < 2)
            return;
        
        int mid = (end + start)/2;

        mergeSort(array, start, mid);

        mergeSort(array, mid, end);

        merge(array, start, mid, end);

    }

    //Merge method, merges the two sorted components of the current block of the Merge Sort algorithm
    private static void merge(int[] array, int start, int mid, int end) {

        int[] tempArray = new int[array.length];
        for(int i = 0; i < array.length; i++)
            tempArray[i] = array[i];

        int k = start;
        int i = start;
        int j = mid;

        while(i < mid && j < end) {
            if(tempArray[i] < tempArray[j]) {
                array[k] = tempArray[i];
                i++;
            }
            else {
                array[k] = tempArray[j];
                j++;
            }
            mergeCompare++;
            k++;
        }

        
        if(i == mid) {
            while(k < end) {
                array[k] = tempArray[j];
                j++;
                k++;
            }
        }

        if(j == end) {
            while(k < end) {
                array[k] = tempArray[i];
                i++;
                k++;
            }
        }
        
    }

    //Insertion Sort algorithm, sorts an unsorted array of ints
    private static int[] insertionSort(int[] array) {

        for (int i = 1; i < array.length; i++) {
            for (int j = i; j > 0; j--) {
                if(array[j] < array[j-1]) {
                    insertionCompare++;
                    swap(array, j, j-1);
                }
            }
        }

        return array;
    }

    //Radix sort, sorts an array using the least significant digit approach
    private static void radixSort(int[] array) {

        Bucket bucket = new Bucket();
        
        int max_digits = maxDigits(array);

        for(int d = 1; d <= max_digits; d++) {

            for(int i = 0; i < array.length; i++) {
                bucket.get(array[i]/(int)Math.pow(10, d - 1)%10).add(array[i]);
            }

            int k = 0;
            for(int i = -9; i < 10; i++) {
                while(bucket.get(i).size() != 0) {
                    array[k] = bucket.get(i).remove(0);
                    k++;
                }
            }
        }
    }

    //Returns the highest digit count of any element in the array
    private static int maxDigits(int[] array) {

        int max_digits = 0;
        for(int i = 0; i < array.length; i++) {
            int num_digits = 0;
            int temp = array[i];
            while(temp > 0) {
                num_digits++;
                temp /= 10;
            }
            max_digits = Math.max(max_digits, num_digits);
        }

        return max_digits;
    }
}