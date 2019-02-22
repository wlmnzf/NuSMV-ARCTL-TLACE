package display.layout;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import util.ExpandableList;
import util.ExpandableMap;
import display.GUI;
import display.graph.Edge;
import display.graph.Graph;
import display.graph.Vertex;

/**
 * The wide tree like layout layouts a counterexample displaying vertical
 * branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class WideTreeLikeLayout extends TreeLikeLayout implements
		CounterexampleLayout {

	/**
	 * Layouts the intern graph by storing each node location in locations.
	 */
	@Override
	protected void buildLayout() {
		ExpandableMap<Vertex> map = new ExpandableMap<Vertex>();
		ExpandableList<Vertex> drawn = new ExpandableList<Vertex>();
		Vertex vertex = this.graph.getInitialVertex();
		drawNode(vertex, 0, 0);
		map.set(0, 0, vertex);
		drawn.set(vertex.getVertex().getId(), vertex);
		for (Edge edge : graph.getOutEdges(vertex)) {
			drawBranch(graph, edge, 1, 1, map, drawn);
		}
	}

	/**
	 * Draw a straight vertical branch starting at transition, on graph,
	 * starting at position (x,y).
	 * 
	 * @param graph
	 *            the graph to draw the branch on.
	 * @param edge
	 *            the first transition of the branch.
	 * @param x
	 *            the row of the start position of the branch.
	 * @param y
	 *            the column of the start position of the branch.
	 * @param map
	 *            a map of each drawn vertex (indices are positions on the
	 *            graph).
	 * @param drawn
	 *            a list of each drawn vertex (indices are ids).
	 * @return the column at which the branch has been drawn.
	 */
	private int drawBranch(Graph graph, Edge edge, int x, int y,
			ExpandableMap<Vertex> map, ExpandableList<Vertex> drawn) {
		while (overlap(graph, edge, map, x, y)) {
			y++;
		}
		Edge current = edge;
		Vertex vertex = graph.getSource(edge);
		while (current != null) {
			if (current.getEdge().getTo().getId() <= current.getEdge()
					.getFrom().getId()) {
				// Loop
				current = null;
			} else {
				// Normal node
				vertex = graph.getDest(current);
				drawNode(vertex, x, y);
				map.set(x, y, vertex);
				drawn.set(current.getEdge().getTo().getId(), vertex);

				if (current.getEdge().getTo().getNext() == null) {
					// End of branch
					current = null;
				} else {
					current = graph.getOutEdges(graph.getDest(current))
							.iterator().next();
				}
			}
			x++;
		}
		x--;
		while (vertex != graph.getSource(edge)) {
			for (Edge branch : getBranchEdges(graph, vertex)) {
				drawBranch(graph, branch, x + 1, y + 1, map, drawn);
			}
			x--;
			vertex = graph
					.getSource(graph.getInEdges(vertex).iterator().next());
		}

		return y;
	}

	/**
	 * Returns whether the branch starting at transition overlaps another branch
	 * in map if it starts at (x,y).
	 * 
	 * @param edge
	 *            the starting transition of the branch.
	 * @param map
	 *            the reference map.
	 * @param x
	 *            the row of the start position of the branch.
	 * @param y
	 *            the column of the start position of the branch.
	 * @return true if the branch starting at transition overlaps another branch
	 *         in map if it starts at (x,y), false otherwise.
	 */
	private boolean overlap(Graph graph, Edge edge, ExpandableMap<Vertex> map,
			int x, int y) {
		while (edge != null
				&& edge.getEdge().getTo().getId() > edge.getEdge().getFrom()
						.getId()) {
			if (map.get(x, y) != null) {
				return true;
			}
			for (int ny = y; ny < map.height(); ny++) {
				if (map.get(x, ny) != null) {
					return true;
				}
			}
			if (edge.getEdge().getTo().getNext() == null) {
				// End of branch
				edge = null;
			} else {
				edge = graph.getOutEdges(graph.getDest(edge)).iterator().next();
			}
			x++;
		}
		return false;
	}

	/**
	 * Returns the edges corresponding to the branches of the TlaceVertex vertex
	 * represents.
	 * 
	 * @param graph
	 *            the graph from which taking the edges.
	 * @param vertex
	 *            the vertex of the graph from which we want the edge branches.
	 * @return the edges corresponding to the branches of the TlaceVertex vertex
	 *         represents. This is the out edges of vertex in graph, except the
	 *         possibly inexistent edge of the main path.
	 */
	private Collection<Edge> getBranchEdges(Graph graph, Vertex vertex) {
		List<Edge> branches = new ArrayList<Edge>();
		for (Edge edge : graph.getOutEdges(vertex)) {
			if (vertex.getVertex().getNext() != edge.getEdge()) {
				branches.add(edge);
			}
		}
		return branches;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.layout.CounterexampleLayout#getType()
	 */
	@Override
	public int getType() {
		return GUI.WIDETREELIKE;
	}
}
