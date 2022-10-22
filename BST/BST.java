import java.util.ArrayList;

/*
 * Hunter Smith, date of submission: 5/6/21
 * BST.java: a Binary Search Tree with the ability to insert new Nodes, search for nodes with given values, 
 * and return Nodes with given values if specified. Made up of Node objects
*/

public class BST {
    public Node root;

    public BST() {
        root = null;
    }

    public BST(int v) {
        root = new Node(v);
        root.al.add(1);
    }

    //Node class, the primary object in the BST
    public class Node {
        public int value;
        public ArrayList<Integer> al;

        public Node left;
        public Node right;

        public Node(int v) {
            value = v;
            al = new ArrayList<Integer>();
        }
    }

    public boolean search(int v) {
        return search(root, v);
    }

    //Search method: traverses the BST and returns whether or not the given value is located in the given BST (node)
    private boolean search(Node n, int v) {
        if (n == null)
		    return false;
	    if (n.value == v)
		    return true;

	    return search(n.left, v) || search(n.right, v);
    }

    public Node searchNode(int v) {
        return searchNode(root, v);
    }

    //SearchNode method: traverses the BST and returns the node with the given value
    private Node searchNode(Node n, int v) {
        if(n == null)
		    return null;
	    if(n.value == v) {
		    return n;
        }
	    if(v < n.value)
		    return searchNode(n.left, v);
	    else
		    return searchNode(n.right, v);
    }

    public void insert(int v, int pos) {
        insert(root, v, pos);
    }

    //Insert method: inserts a new node with a given value into the correct location in the BST
    private void insert(Node n, int v, int pos) {
        if(n.value == v) {
            n.al.add(pos);
        }

        if(v < n.value) {
            if(n.left == null) {
                n.left = new Node(v);
                n.left.al.add(pos);
            }
            else
                insert(n.left, v, pos);
        }
        if(v > n.value) {
            if(n.right == null) {
                n.right = new Node(v);
                n.right.al.add(pos);
            }
            else
                insert(n.right, v, pos);
        }
    } 
}