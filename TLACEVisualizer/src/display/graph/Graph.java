package display.graph;

import edu.uci.ics.jung.graph.DirectedSparseGraph;

/**
 * A Graph is a directed sparse graph, composed of Vertices and Edges, that
 * contains an initial vertex.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Graph extends DirectedSparseGraph<Vertex, Edge> {

	private static final long serialVersionUID = -7382147020694952411L;

	/**
	 * The initial vertex of this graph.
	 */
	private Vertex initial;

	/**
	 * Returns the initial vertex of this graph.
	 * 
	 * @return the initial vertex of this graph.
	 */
	public Vertex getInitialVertex() {
		return initial;
	}

	/**
	 * Sets the initial vertex of this graph to vertex.
	 * 
	 * @param vertex
	 *            the new initial vertex.
	 */
	public void setInitialVertex(Vertex vertex) {
		this.initial = vertex;
	}

}
