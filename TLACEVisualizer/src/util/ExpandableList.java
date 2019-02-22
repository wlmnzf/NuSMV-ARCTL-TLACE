package util;

import java.util.Iterator;

/**
 * ExpandableList represents an ordered dynamically-sized list of elements. It
 * is able to expand itself to contain an element at an index that is out of its
 * size bound by expanding itself before storing it.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class ExpandableList<Element> implements Iterable<Element> {

	/**
	 * The list of currently stored elements.
	 */
	private Object[] elements;

	/**
	 * The actual size of the list.
	 */
	private int size;

	/**
	 * The initial size of elements when the list is created.
	 */
	private static final int initialSize = 10;

	/**
	 * Creates a new empty expandable list.
	 */
	public ExpandableList() {
		this.elements = new Object[initialSize];
		this.size = 0;
	}

	/**
	 * Returns the element at index.
	 * 
	 * @param index
	 *            the index of the wanted element. index >= 0.
	 * @return the element at index if such an element exists, null otherwise.
	 */
	@SuppressWarnings("unchecked")
	public Element get(int index) {
		if (0 <= index && index < size) {
			return (Element) elements[index];
		} else {
			return null;
		}
	}

	/**
	 * Adds the given element at the end of the list.
	 * 
	 * @param element
	 *            the element to add.
	 */
	public void add(Element element) {
		if (size >= elements.length) {
			elements = expand(elements, elements.length * 2);
		}
		elements[size++] = element;
	}

	/**
	 * Adds all elements of elements to this list.
	 * 
	 * @param elements
	 *            the list of elements to add.
	 */
	public void addAll(Element[] elements) {
		for (int i = 0; i < elements.length; i++) {
			this.add(elements[i]);
		}
	}

	/**
	 * Puts the given element at index.
	 * 
	 * @param index
	 *            the index at which put the element. index >= 0.
	 * @param element
	 *            the element to put.
	 */
	public void set(int index, Element element) {
		if (index >= elements.length) {
			elements = expand(elements, index + 1);
		}
		elements[index] = element;
		size = Math.max(size, index + 1);
	}

	/**
	 * Returns a new array of size size containing all the elements of array if
	 * size > array.length, returns array otherwise.
	 * 
	 * @param array
	 *            the array to expand.
	 * @param size
	 *            the size of the returned array.
	 * @return a new array of size size if size > array.length, array otherwise.
	 *         The returned array contains every element of array.
	 */
	private Object[] expand(Object[] array, int size) {
		if (array.length < size) {
			Object[] newArray = new Object[size];
			for (int i = 0; i < array.length; i++) {
				newArray[i] = array[i];
			}
			return newArray;
		} else {
			return array;
		}
	}

	/**
	 * Returns the size of this list. This is the highest index at which an
	 * element has been previously added in the list.
	 * 
	 * @return the size of this list.
	 */
	public int size() {
		return size;
	}

	/**
	 * Returns an array representation of the content of this list.
	 * 
	 * @return an array representation of the content of this list, of size
	 *         size.
	 */
	public Object[] toArray() {
		Object[] result = new Object[size];
		for (int i = 0; i < size; i++) {
			result[i] = elements[i];
		}
		return result;
	}

	/**
	 * Returns array, filled with the min(this.size(), array.length) first
	 * elements of this list.
	 * 
	 * @param array
	 *            the array to fill.
	 * @return array, filled with the elements of this list.
	 */
	@SuppressWarnings("unchecked")
	public Element[] fillArray(Element[] array) {
		for (int i = 0; i < Math.min(size, array.length); i++) {
			array[i] = (Element) elements[i];
		}
		return array;
	}

	/**
	 * Returns whether this list is empty (i.e. full of null values).
	 * 
	 * @return true if there is only null values in this list, false otherwise.
	 */
	public boolean isEmpty() {
		for (int i = 0; i < this.size; i++) {
			if (this.elements[i] != null)
				return false;
		}
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	@Override
	public Iterator<Element> iterator() {
		Object[] elements = new Object[this.size];
		for (int i = 0; i < this.size; i++) {
			elements[i] = this.elements[i];
		}
		return new ExpandableListIterator(elements);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String result = "[";
		for (int i = 0; i < this.elements.length; i++) {
			result += this.elements[i].toString()
					+ ((i < this.elements.length - 1) ? "," : "");
		}
		return result + "]";
	}

	private class ExpandableListIterator implements Iterator<Element> {

		/**
		 * The list of elements to iterate on.
		 */
		private Object[] elements;

		/**
		 * The current position.
		 */
		private int position;

		/**
		 * Creates a new iterator on the given list of elements.
		 * 
		 * @param elements
		 *            the list of elements to iterate on.
		 */
		public ExpandableListIterator(Object[] elements) {
			this.elements = elements;
			this.position = 0;
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.util.Iterator#hasNext()
		 */
		@Override
		public boolean hasNext() {
			return this.position < this.elements.length;
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.util.Iterator#next()
		 */
		@SuppressWarnings("unchecked")
		@Override
		public Element next() {
			return (Element) this.elements[this.position++];
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.util.Iterator#remove()
		 */
		@Override
		public void remove() {
			throw new UnsupportedOperationException();
		}

	}
}
