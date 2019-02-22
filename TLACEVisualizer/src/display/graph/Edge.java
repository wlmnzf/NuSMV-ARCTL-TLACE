package display.graph;

import tlacegraph.TlaceEdge;

/**
 * An Edge represents an edge in a TlaceGraph. It just contains the edge it
 * represents.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Edge {

	/**
	 * The TlaceEdge represented by this edge.
	 */
	private TlaceEdge edge;

	/**
	 * Creates a new edge representing the given TlaceEdge.
	 * 
	 * @param edge
	 *            the represented TlaceEdge.
	 */
	public Edge(TlaceEdge edge) {
		this.edge = edge;
	}

	/**
	 * Returns the TlaceEdge this edge represents.
	 * 
	 * @return the TlaceEdge this edge represents.
	 */
	public TlaceEdge getEdge() {
		return edge;
	}
}
