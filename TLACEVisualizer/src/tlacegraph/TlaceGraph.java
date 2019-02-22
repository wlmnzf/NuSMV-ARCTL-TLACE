package tlacegraph;

/**
 * A TLACE graph represents the graph of a TLACE. It is composed of vertices and
 * edges. It is a particular graph since it has one and only one initial state.
 * In addition to the graph itself, the violated specification is attached.
 * 
 * Intuitively, the graph of a TLACE is the graph where each TLACE node is
 * merged with the first TLACE node of its branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TlaceGraph {

	/**
	 * The CTL formula this TlaceGraph is a counter-example for.
	 */
	private String specification;

	/**
	 * The first TlaceVertex of this TlaceGraph.
	 */
	private TlaceVertex initial;

	/**
	 * Creates a new TlaceGraph for specification, explained by initial.
	 * 
	 * @param specification
	 *            the specification the new graph is a counter-example for.
	 * @param initial
	 *            the first vertex of the new graph.
	 */
	public TlaceGraph(String specification, TlaceVertex initial) {
		this.specification = specification;
		this.initial = initial;
	}

	/**
	 * Returns the specification of this graph.
	 * 
	 * @return the specification of this graph.
	 */
	public String getSpecification() {
		return specification;
	}

	/**
	 * Returns the first vertex of this graph.
	 * 
	 * @return the initial vertex of this graph.
	 */
	public TlaceVertex getVertex() {
		return initial;
	}
}