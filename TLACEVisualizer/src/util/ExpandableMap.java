package util;

/**
 * ExpandableMap represents a dynamically-sized matrix of elements. It is able
 * to expand itself to contain an element at an index that is out of its size
 * bound by expanding itself before storing it.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class ExpandableMap<Element> {

	/**
	 * The map of currently stored elements.
	 */
	private Object[][] elements;

	/**
	 * The actual width of the map.
	 */
	private int width;

	/**
	 * The actual height of the map.
	 */
	private int height;

	/**
	 * The initial size (initialSize x initialSize) of elements when the map is
	 * created.
	 */
	private static final int initialSize = 100;

	/**
	 * Creates a new empty expandable list.
	 */
	public ExpandableMap() {
		this.elements = new Object[initialSize][initialSize];
		this.width = 0;
		this.height = 0;
	}

	/**
	 * Returns the element at position (x,y) in this map.
	 * 
	 * @param x
	 *            the column of the element.
	 * @param y
	 *            the row of the element.
	 * @return the element at position (x,y) in this map if any, null otherwise.
	 */
	@SuppressWarnings("unchecked")
	public Element get(int x, int y) {
		if (0 <= x && x < width && 0 <= y && y < height) {
			return (Element) elements[x][y];
		} else {
			return null;
		}
	}

	/**
	 * Sets the value of the map at position (x,y) to element.
	 * 
	 * @param x
	 *            the column in which put the element.
	 * @param y
	 *            the row in which put the element.
	 * @param element
	 *            the element to insert.
	 */
	public void set(int x, int y, Element element) {
		if (x >= elements.length | y >= elements[0].length) {
			System.out.println("expanding");
			elements = expand(elements, x + 1, y + 1);
		}
		elements[x][y] = element;
		this.width = Math.max(this.width, x + 1);
		this.height = Math.max(this.height, y + 1);
	}

	/**
	 * Returns a new array of size width x height containing all the elements of
	 * array if the requested size is larger than the current one, returns array
	 * otherwise.
	 * 
	 * @param array
	 *            the array to expand.
	 * @param width
	 *            the width of the returned array.
	 * @param height
	 *            the height of the returned array.
	 * @return a new array of size width x height if the requested size is
	 *         larger than the current one, array otherwise. The returned array
	 *         contains every element of array.
	 */
	private Object[][] expand(Object[][] array, int width, int height) {
		if (array.length < width | array[0].length < height) {
			Object[][] newArray = new Object[Math.max(array.length, width)][Math
					.max(array[0].length, height)];
			for (int i = 0; i < array.length; i++) {
				for (int j = 0; j < array.length; j++) {
					newArray[i][j] = array[i][j];
				}
			}
			return newArray;
		} else {
			return array;
		}
	}

	/**
	 * Returns the width of this map. This is the highest column at which an
	 * element has been previously added in the map.
	 * 
	 * @return the width of this map.
	 */
	public int width() {
		return this.width;
	}

	/**
	 * Returns the height of this map. This is the highest row at which an
	 * element has been previously added in the map.
	 * 
	 * @return the height of this map.
	 */
	public int height() {
		return this.height;
	}
}
