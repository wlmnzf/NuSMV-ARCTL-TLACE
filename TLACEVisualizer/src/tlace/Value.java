/**
 * 
 */
package tlace;

/**
 * A value is a couple variable-value.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Value {

	/**
	 * The variable name of this value.
	 */
	private String variable;

	/**
	 * The value of this value.
	 */
	private String value;

	/**
	 * Creates a new variable-value pair.
	 * 
	 * @param variable
	 *            the variable of the created value.
	 * @param value
	 *            the value of the created value.
	 */
	public Value(String variable, String value) {
		this.variable = variable;
		this.value = value;
	}

	/**
	 * Returns the variable name of this value.
	 * 
	 * @return the variable name of this value.
	 */
	public String getVariable() {
		return variable;
	}

	/**
	 * Returns the value of this value.
	 * 
	 * @return the value of this value.
	 */
	public String getValue() {
		return value;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object o) {
		if (o instanceof Value) {
			Value value = (Value) o;
			return this.variable.equals(value.variable)
					& this.value.equals(value.value);
		} else {
			return false;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return this.variable + " = " + this.value;
	}

}
