public class HeapMax {
    private Schedule[] heap;
    public int size;

    public HeapMax(int size) {
        if(size <= 0) {
            this.heap = new Schedule[1];
            System.out.println("Non-positive size given. Heap size set to 1.");
        }
        else
            this.heap = new Schedule[size];

        this.size = 0;
    }

    /* === Helper Methods === */

    //Swaps the elements at the given indices
    private void swap(int index1, int index2) {
        Schedule temp = heap[index1];
        heap[index1] = heap[index2];
        heap[index2] = temp;
    }

    //Doubles the size of the heap array if there is too many inputs given
    private void createNewHeap() {
        Schedule[] newHeap = new Schedule[size * 2 + 1];

        for(int i = 0; i < heap.length; i++) {
            newHeap[i] = heap[i];
        }

        heap = newHeap;
    }

    //Bases the heap on which type of priority is specified
    private int priorityType(Schedule element, int type) {
        //These are mostly unneed in this program but because I want it to be versitile I am leaving the options
        if(type == 1)
            return element.positionAdded;
        else if(type == 2)
            return element.numPassengers;
        else if(type == 3)
            return element.takeOffRequestTimeInt;
        else
            return -1;
    }

    /* ====================== */

    public void insert(Schedule element, int type) {
        //Creates a new heap when there is not a remaining element in the heap
        if(size == heap.length) {
            createNewHeap();
        }
        heap[size] = element;
        int elementLoc = size;
        int parentLoc = (size - 1)/2;

        //Bubble up the heap to sort it after insertion
        while(priorityType(heap[elementLoc], type) > priorityType(heap[parentLoc], type)) {
            swap(elementLoc, parentLoc);
            elementLoc = parentLoc;
            parentLoc = (elementLoc - 1)/2;
        }

        size++;
    }

    //Returns the max element of the heap
    public Schedule extractMax() {
        if(size == 0)
            return null;
        else
            return heap[0];
    }

    //Removes and returns the max element of the heap
    public Schedule removeMax(int type) {
        if(size == 0)
            return null;
        Schedule max = heap[0];

        //This is the 'removel'
        swap(0, size - 1);
        size--;

        //Initializes the element location and children
        int elementLoc = 0;
        int leftChild = 1;
        int rightChild = 2;

        int maxChild = leftChild;

        //Ensures the maxChild is in the array and the correct child
        if(maxChild >= size)
            return max;
        else if(priorityType(heap[rightChild], type) > priorityType(heap[maxChild], type) && rightChild < size)
                maxChild = rightChild;

        //Bubbles down the heap to sort it after removal
        while(priorityType(heap[elementLoc], type) < priorityType(heap[maxChild], type)) {
            swap(elementLoc, maxChild);
            elementLoc = maxChild;
            
            leftChild = 2 * elementLoc + 1;
            rightChild = 2 * elementLoc + 2;

            if(leftChild >= size)
                break;
            else if(rightChild >= size)
                maxChild = leftChild;
            else {
                maxChild = leftChild;
                if(priorityType(heap[rightChild], type) > priorityType(heap[maxChild], type))
                    maxChild = rightChild;
            }
        }

        return max;
    }
}