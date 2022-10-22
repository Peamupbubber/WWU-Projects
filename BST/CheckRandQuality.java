import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;

/*
 * Hunter Smith, date of submission: 5/6/21
 * CheckRandQuality.java: creates and populates a Binary Search Tree (BST) and allows for checking 
 * if the BST properly stored the given inputs using commandline arguments
*/

/*
 * I used this link to figure out how to read input from a .txt file:
 *  http://www.beginwithjava.com/java/file-input-output/reading-text-file.html
*/

public class CheckRandQuality {

    public static void main(String[] args) throws IOException {

        FileReader fileReader = new FileReader(args[0]);
        Scanner inFile = new Scanner(fileReader);

        BST bst = new BST(inFile.nextInt());
        int position = 2;
        
        while(inFile.hasNextLine()) {
            bst.insert(inFile.nextInt(), position);
            position++;
        }

        for(int i = 1; i < args.length; i++) {
            if(bst.search(Integer.parseInt(args[i]))) {
                BST.Node n = bst.searchNode(Integer.parseInt(args[i]));

                if(n.al.size() == 1)
                    System.out.print(args[i] + " appears " + n.al.size() + " time, order of insertion: ");
                else
                    System.out.print(args[i] + " appears " + n.al.size() + " times, order of insertion: ");

                for(int j = 0; j < n.al.size(); j++){
                    if(j == n.al.size() - 1)
                        System.out.print(n.al.get(j));
                    else
                        System.out.print(n.al.get(j) + ", ");
                }
                System.out.println();
            }

            else {
                System.out.println(args[i] + " not found in BST");
            }
        }

        System.out.println();
        inFile.close();
    }
}