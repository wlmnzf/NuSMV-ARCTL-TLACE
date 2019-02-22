package display.graph;

import tlacegraph.TlaceEdge;
import tlacegraph.TlaceGraph;
import tlacegraph.TlaceVertex;
import util.ExpandableList;

/**
 * GraphBuilder provides a way to get a graph from a given TlaceGraph.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class GraphBuilder {

	/**
	 * Creates a graph from the given TlaceGraph.
	 * 
	 * @param tlaceGraph
	 *            the TlaceGraph to extract a graph from.
	 */
	public static Graph buildGraph(TlaceGraph tlaceGraph) {
		Graph graph = new Graph();
		ExpandableList<Vertex> createdVertices = new ExpandableList<Vertex>();
		fillWithVertex(graph, tlaceGraph.getVertex(), createdVertices);
		return graph;
	}

	/**
	 * Fills the given graph with the vertices representing the TlaceVertices of
	 * the counter-example represented by tlaceVertex. CreatedVertices is also
	 * filled with the created vertices (they lie at their own id in
	 * createdVertices).
	 * 
	 * @param graph
	 *            the graph to fill.
	 * @param tlaceVertex
	 *            the counter-example to walk through.
	 * @param createdVertices
	 *            the list of created vertices, filled during the walk.
	 */
	private static void fillWithVertex(Graph graph, TlaceVertex tlaceVertex,
			ExpandableList<Vertex> createdVertices) {
		Vertex vertex = new Vertex(tlaceVertex);

		// Set graph initial if null
		if (graph.getInitialVertex() == null) {
			graph.setInitialVertex(vertex);
		}

		createdVertices.set(tlaceVertex.getId(), vertex);
		graph.addVertex(vertex);
		for (String spec : tlaceVertex.getBranches().keySet()) {
			vertex.setDisplayed(tlaceVertex.getBranches().get(spec), true);
			if (tlaceVertex.getBranches().get(spec) != TlaceEdge.getNull()) {
				fillWithPath(graph, tlaceVertex.getBranches().get(spec),
						createdVertices);
			}
		}
	}

	/**
	 * Fills the given graph with the vertices representing the TlaceVertices of
	 * the path starting at tlaceEdge. CreatedVertices is also filled with the
	 * created vertices (they lie at their own id in createdVertices).
	 * 
	 * @param graph
	 *            the graph to fill.
	 * @param tlaceVertex
	 *            the first edge of the path.
	 * @param createdVertices
	 *            the list of created vertices, filled during the fill.
	 */
	private static void fillWithPath(Graph graph, TlaceEdge tlaceEdge,
			ExpandableList<Vertex> createdVertices) {
		while (tlaceEdge != null && tlaceEdge.getTo().getPrev() == tlaceEdge) {
			TlaceVertex vertex = tlaceEdge.getTo();
			fillWithVertex(graph, vertex, createdVertices);
			Edge edge = new Edge(tlaceEdge);
			graph.addEdge(edge,
					createdVertices.get(tlaceEdge.getFrom().getId()),
					createdVertices.get(tlaceEdge.getTo().getId()));
			tlaceEdge = tlaceEdge.getTo().getNext();
		}

		// Loop
		if (tlaceEdge != null) {
			graph.addEdge(new Edge(tlaceEdge),
					createdVertices.get(tlaceEdge.getFrom().getId()),
					createdVertices.get(tlaceEdge.getTo().getId()));
		}
	}

	public static Graph buildFirstVertex(TlaceGraph counterexample) {
		Graph graph = new Graph();
		Vertex vertex = new Vertex(counterexample.getVertex());
		graph.setInitialVertex(vertex);
		graph.addVertex(vertex);
		return graph;
	}
}
