package display.layout;

import util.ExpandableList;
import display.GUI;
import display.graph.Edge;
import display.graph.Vertex;

/**
 * The narrow tree like layout layouts a counterexample displaying vertical
 * branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class NarrowTreeLikeLayout extends TreeLikeLayout implements
		CounterexampleLayout {

	/**
	 * Layouts the intern graph by storing each node location in locations.
	 */
	protected void buildLayout() {
		// Initiate useful lists
		// allDrawn is the list of all already drawn vertices (indices are ids)
		ExpandableList<Vertex> allDrawn = new ExpandableList<Vertex>();
		// toDraw is the list of edges to draw.
		// The transition (and the end vertex if any) will be drawn.
		// Indices are positions at the current stage
		ExpandableList<Edge> toDraw = new ExpandableList<Edge>();
		// toDrawNext keeps the list of transitions to draw at the next stage
		ExpandableList<Edge> toDrawNext = new ExpandableList<Edge>();
		drawNode(this.graph.getInitialVertex(), 0, 0);
		allDrawn.set(this.graph.getInitialVertex().getVertex().getId(),
				this.graph.getInitialVertex());
		for (Edge edge : this.graph.getOutEdges(this.graph.getInitialVertex())) {
			if (allDrawn.get(graph.getOpposite(graph.getInitialVertex(), edge)
					.getVertex().getId()) == null) {
				toDraw.add(edge);
			}
		}

		int stage = 1;
		while (!toDraw.isEmpty()) {
			toDrawNext = new ExpandableList<Edge>();

			for (int i = 0; i < toDraw.size(); i++) {
				Edge tr = toDraw.get(i);

				while (toDrawNext.size() < i)
					toDrawNext.add(null);

				if (tr != null) {
					if (allDrawn.get(tr.getEdge().getTo().getId()) != null) {
						// Loop, nothing to do
					} else {
						Vertex vertex = graph.getDest(tr);
						drawNode(vertex, stage, i);
						allDrawn.set(tr.getEdge().getTo().getId(), vertex);
						for (Edge edge : graph.getOutEdges(allDrawn.get(tr
								.getEdge().getTo().getId()))) {
							if (allDrawn.get(edge.getEdge().getTo().getId()) == null) {
								toDrawNext.add(edge);
							}
						}
					}
				}
			}

			toDraw = toDrawNext;
			stage++;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.layout.CounterexampleLayout#getType()
	 */
	@Override
	public int getType() {
		return GUI.NARROWTREELIKE;
	}
}
