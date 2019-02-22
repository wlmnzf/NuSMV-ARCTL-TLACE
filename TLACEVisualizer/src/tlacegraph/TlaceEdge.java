package tlacegraph;

import tlace.Value;

/**
 * A TlaceEdge represents an edge of a TlaceGraph. It has input values, a
 * previous TlaceVertex and a next one.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TlaceEdge {

	/**
	 * The NULL TlaceEdge. This is an empty edge.
	 */
	private static TlaceEdge NULL = new TlaceEdge(null, null, null);

	/**
	 * The input values of this edge.
	 */
	private Value[] inputs;

	/**
	 * The source of this edge.
	 */
	private TlaceVertex from;

	/**
	 * The destination of this edge.
	 */
	private TlaceVertex to;

	/**
	 * Creates a new TlaceEdge from from to to with inputs as input values.
	 * 
	 * @param inputs
	 *            the input values of the new edge.
	 * @param from
	 *            the source of the new edge.
	 * @param to
	 *            the destination of the new edge.
	 */
	public TlaceEdge(Value[] inputs, TlaceVertex from, TlaceVertex to) {
		this.inputs = inputs;
		this.from = from;
		this.to = to;
	}

	/**
	 * Returns the input values of this edge.
	 * 
	 * @return the input values of this edge.
	 */
	public Value[] getInputs() {
		return inputs;
	}

	/**
	 * Returns the source vertex of this edge.
	 * 
	 * @return the source vertex of this edge.
	 */
	public TlaceVertex getFrom() {
		return from;
	}

	/**
	 * Returns the destination vertex of this edge.
	 * 
	 * @return the destination vertex of this edge.
	 */
	public TlaceVertex getTo() {
		return to;
	}

	/**
	 * Sets the source of this edge to vertex.
	 * 
	 * @param vertex
	 *            the new source of this edge.
	 */
	public void setFrom(TlaceVertex vertex) {
		from = vertex;
	}

	/**
	 * Returns the NULL TlaceEdge.
	 * 
	 * @return the NULL TlaceEdge.
	 */
	public static TlaceEdge getNull() {
		return NULL;
	}
}
