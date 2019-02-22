package tlace;


/**
 * A transition is an edge between two TLACE nodes. In addition to the source
 * and the destination, a transition contains input values.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Transition {

	/**
	 * The input values associated with this transition.
	 */
	private Value[] inputs;

	/**
	 * The source of this transition.
	 */
	private Node from;

	/**
	 * The destination of this transition.
	 */
	private Node to;

	/**
	 * Creates a new transition from from to to, with inputs as input values.
	 * 
	 * @param inputs
	 *            the input values of the new transition.
	 * @param from
	 *            the source of the new transition.
	 * @param to
	 *            the destination of the new transition.
	 */
	public Transition(Value[] inputs, Node from, Node to) {
		this.inputs = inputs;
		this.from = from;
		this.to = to;
	}

	/**
	 * Returns the set of input values of this transition.
	 * 
	 * @return the an array containing the input values of this transition.
	 */
	public Value[] getInputs() {
		return inputs;
	}

	/**
	 * Returns the source of this transition.
	 * 
	 * @return the source of this transition.
	 */
	public Node getFrom() {
		return from;
	}

	/**
	 * Returns the destination of this transition.
	 * 
	 * @return the destination of this transition.
	 */
	public Node getTo() {
		return to;
	}

}
