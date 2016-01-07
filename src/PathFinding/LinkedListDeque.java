/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package PathFinding;

import java.util.Collection;
import java.util.Deque;
import java.util.ConcurrentModificationException;
import java.util.Iterator;

/**
 *
 * @author Impaler
 */
public class LinkedListDeque<E> implements Deque<E> {
	
	public class Node {

		public E Data;
		public Node next;
		public Node prior;

		public Node() {
			Data = null;
		}

		public void displayNode() {
			//System.out.println( + " ");
		}
	}
	
	private int size;
    private Node first;
    private Node last;
	
	Node Pool;

	private transient int modCount = 0;

    public LinkedListDeque(int Capacity) {
		Node current = new Node();
		Pool = current;
		for (int i = 1; i < Capacity; i++) {
			current.next = new Node();
			current = current.next;
		}
    }

    public boolean isEmpty() {
        return first == null;
    }

    public int size() {
        return size;
    }

	public boolean offer(E e) {
        if (e == null)
            throw new NullPointerException();
        modCount++;
        insertFront(e);
        return true;
    }

    public E peek() {
		if (first != null)
			return first.Data;
		return null;
    }
	
	private Node getNode(E e) {
		Node result = Pool;
		
		if (Pool.next != null) {
			Pool = Pool.next;
		} else {
			Pool = new Node();
		}
		
		result.Data = e;
		result.next = null;
		return result;
	}

	private void recycleNode(Node oldNode) {
		oldNode.next = Pool;
		Pool = oldNode;
	}

	public void insertFront(E e) {
        Node newNode = getNode(e);
        Node previous = null;
        Node current = first;
		size++;

		Comparable<? super E> key = (Comparable<? super E>) e;

        while (current != null && key.compareTo(current.Data) > 0) {
            previous = current;
            current = current.next;
        }

        if (previous == null) {
			newNode.prior = previous;
            newNode.next = first;
			if (first != null)
				first.prior = newNode;
            first = newNode;
        } else {
            previous.next = newNode;
			newNode.prior = previous;
            newNode.next = current;
			if (current != null)
				current.prior = newNode;
        }
		
		if (newNode.next == null)
			last = newNode;
    }

	public void insertBack(E e) {
        Node newNode = getNode(e);
        Node previous = null;
        Node current = last;
		size++;

		Comparable<? super E> key = (Comparable<? super E>) e;

        while (current != null && key.compareTo(current.Data) < 0) {
            previous = current;
            current = current.prior;
        }

        if (previous == null) {
			newNode.next = current;
            newNode.prior = last;
            first = newNode;
        } else {
            previous.prior = newNode;
 			newNode.next = previous;
			newNode.prior = current;
			if (current != null)
				current.next = newNode;
        }
		if (first == null)
			first = last;
    }

	public void addFirst(E e) {
		offerFirst(e);
	}

	public void addLast(E e) {
		offerLast(e);
	}

	public boolean offerFirst(E e) {
        if (e == null)
            throw new NullPointerException();
        modCount++;
        insertFront(e);
		return true;
	}

	public boolean offerLast(E e) {
        if (e == null)
            throw new NullPointerException();
        modCount++;
        insertBack(e);
		return true;
	}

	public E removeFirst() {
		Node result = first;

		if (first.next == null) {
			last = first = null;
		} else {
			first.next.prior = null;
			first = first.next;	
			result.next = null;
		}
		E returnData = result.Data;
		recycleNode(result);
		size--;
		return returnData;
	}

	public E removeLast() {
 		Node result = last;
		
		if (last.prior == null) {
			first = last = null;			
		} else {
			last.prior.next = null;
			last = last.prior;		
			result.prior = null;
		}

		E returnData = result.Data;
		recycleNode(result);
		size--;
		return returnData;
	}
	
	public E pollFirst() {
		return removeFirst();
	}

	public E pollLast() {
		return removeLast();
	}
	
	public E getFirst() {
		return first.Data;
	}	
	
	public E getLast() {
		return last.Data;
	}
	
	public E peekFirst() {
		return first.Data;
	}

	public E peekLast() {
		return last.Data;
	}

	public boolean removeFirstOccurrence(Object o) {
		return false;
	}	

    public boolean removeLastOccurrence(Object o) {
		return false;
	}	

	public void clear() {
        first = null;
		last = null;
        size = 0;
		modCount = 0;
    }

    public E poll() {
        if (size == 0)
            return null;
        modCount++;

        return removeFirst();
    }

	public boolean add(E e) {
		return offerFirst(e);
	}

	public E remove() {
		return null;
	}

	public E element() {
		return null;
	}

	public void push(E e) {
		offerFirst(e);	
	}

	public E pop() {
		return null;
	}

	public boolean remove(Object o) {
		return false;
	}

	public boolean contains(Object o){
		return false;
	}

	public Iterator<E> iterator() {
        return new ForwardItr(this);
    }

	public Iterator<E> descendingIterator() {
		return new BackItr(this);
	}

    private final class ForwardItr implements Iterator<E> {

		private LinkedListDeque.Node current = null;

        /**
         * The modCount value that the iterator believes that the backing
         * Queue should have.  If this expectation is violated, the iterator
         * has detected concurrent modification.
         */
        private int expectedModCount = modCount;
		
		private ForwardItr(LinkedListDeque Parent) {
			current = Parent.first;
		}

        public boolean hasNext() {
            return current.next != null;
        }

        public E next() {
            if (expectedModCount != modCount)
                throw new ConcurrentModificationException();
			E result = (E) current.Data;
			current = current.next;
			return result;
        }

        public void remove() {
            if (expectedModCount != modCount)
                throw new ConcurrentModificationException();

			current.prior.next = current.next;
			current.next.prior = current.prior;
			current = current.next;

            expectedModCount = modCount;
        }
    }
	
    private final class BackItr implements Iterator<E> {

		private LinkedListDeque.Node current = null;

        /**
         * The modCount value that the iterator believes that the backing
         * Queue should have.  If this expectation is violated, the iterator
         * has detected concurrent modification.
         */
        private int expectedModCount = modCount;
		
		private BackItr(LinkedListDeque Parent) {
			current = Parent.first;
		}

        public boolean hasNext() {
            return current.prior != null;
        }

        public E next() {
            if (expectedModCount != modCount)
                throw new ConcurrentModificationException();
			E result = (E) current.Data;
			current = current.prior;
			return result;
        }

        public void remove() {
            if (expectedModCount != modCount)
                throw new ConcurrentModificationException();

			current.prior.next = current.next;
			current.next.prior = current.prior;
			current = current.prior;

            expectedModCount = modCount;
        }
    }
	
	public boolean containsAll(Collection<?> e) {
		return false;
	}

    public boolean addAll(Collection<? extends E> e) {
		return false;
	}

	public boolean removeAll(Collection<?> e) {
		return false;
	}

	public boolean retainAll(Collection<?> e) {
		return false;
	}

	public <T> T[] toArray(T[] e) {
		return null;
	}
	
	public Object[] toArray() {
		return null;
	}
}

