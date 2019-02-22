package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JMenuItem;

import tlacegraph.TlaceEdge;
import tlacegraph.TlaceVertex;
import util.ObservableList;
import display.graph.Edge;
import display.graph.Vertex;
import edu.uci.ics.jung.graph.Graph;
import edu.uci.ics.jung.visualization.VisualizationViewer;

/**
 * A branch listener listens to the branch part of the popup menu of a node. It
 * shows and hides branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class BranchListener implements ActionListener {

	/**
	 * The graph this listener acts on.
	 */
	private Graph<Vertex, Edge> graph;
	/**
	 * The node concerned by this listener.
	 */
	private Vertex vertex;
	/**
	 * The list of selected nodes of the graph. Used to highlight nodes.
	 */
	private ObservableList<Vertex> list;
	/**
	 * The listened menu item responsible for visibility of the branch.
	 */
	private JMenuItem showHide;
	/**
	 * The listened menu item responsible for focusing on the branch.
	 */
	private JMenuItem see;
	/**
	 * The concerned branch.
	 */
	private TlaceEdge branch;
	/**
	 * The visualization viewer the graph is painted on.
	 */
	private VisualizationViewer<Vertex, Edge> vv;

	/**
	 * Creates a new branch listener.
	 * 
	 * @param graph
	 *            the graph to act on.
	 * @param vertex
	 *            the concerned node.
	 * @param branch
	 *            the concerned branch.
	 * @param list
	 *            the list of selected nodes of the graph. Used to highlight
	 *            nodes.
	 * @param showHide
	 *            the listened menu item responsible for visibility of the
	 *            branch.
	 * @param see
	 *            the listened menu item responsible for focusing on the branch.
	 * @param vv
	 *            the visualization viewer the graph is painted on.
	 */
	public BranchListener(Graph<Vertex, Edge> graph, Vertex vertex,
			TlaceEdge branch, ObservableList<Vertex> list, JMenuItem showHide,
			JMenuItem see, VisualizationViewer<Vertex, Edge> vv) {
		this.graph = graph;
		this.vertex = vertex;
		this.branch = branch;
		this.list = list;
		this.showHide = showHide;
		this.see = see;
		this.vv = vv;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource().equals(this.showHide)) {
			// Show or hide the branch
			if (this.vertex.isDisplayed(this.branch)) {
				// The branch is displayed, hide it
				hideBranch(this.graph, this.branch);
				vertex.setDisplayed(this.branch, false);
				this.showHide.setText("Show");
			} else {
				// The branch is hidden, show it
				showBranch(this.graph, this.vertex, this.branch);
				this.vertex.setDisplayed(this.branch, true);
				this.showHide.setText("Hide");
			}

			this.list.notifyObservers();

			this.vv.getGraphLayout().reset();
			this.vv.repaint();
		} else if (e.getSource().equals(this.see)) {
			// Focus on the branch

			// If hidden, show the branch
			if (!this.vertex.isDisplayed(this.branch)) {
				showBranch(this.graph, this.vertex, this.branch);
				this.vertex.setDisplayed(this.branch, true);
				this.showHide.setText("Hide");

				this.vv.getGraphLayout().reset();
				this.vv.repaint();
			}

			// Sets the set of selected nodes to the set of nodes of the branch
			List<Vertex> vertices = getNodesFromBranch(this.graph, this.branch);
			this.list.clear();
			this.list.add(vertex);
			for (Vertex vertex : vertices) {
				if (vertex != null) {
					this.list.add(vertex);
				}
			}
			this.list.notifyObservers();
		}
	}

	/**
	 * Returns the list of nodes representing the states of the given branch in
	 * graph.
	 * 
	 * @param graph
	 *            the graph containing the nodes.
	 * @param branch
	 *            the given branch.
	 * @return the list of nodes representing the states of the given branch in
	 *         graph. If the branch is composed of one single loop, the returned
	 *         list is empty.
	 */
	private List<Vertex> getNodesFromBranch(Graph<Vertex, Edge> graph,
			TlaceEdge branch) {
		List<Vertex> vertices = new ArrayList<Vertex>();
		while (branch != null && branch.getTo().getPrev() == branch) {
			vertices.add(getVertexFromTlaceVertex(graph, branch.getTo()));
			branch = branch.getTo().getNext();
		}
		return vertices;
	}

	/**
	 * Returns the graph vertex corresponding to tlaceVertex in graph.
	 * 
	 * @param graph
	 *            the considered graph.
	 * @param tlaceVertex
	 *            the considered vertex.
	 * @return the graph vertex corresponding to vertex in graph if any, null
	 *         otherwise.
	 */
	private Vertex getVertexFromTlaceVertex(Graph<Vertex, Edge> graph,
			TlaceVertex tlaceVertex) {
		for (Vertex vertex : graph.getVertices()) {
			if (vertex.getVertex().equals(tlaceVertex)) {
				return vertex;
			}
		}
		return null;
	}

	/**
	 * Returns the list of nodes representing the states of the given branch in
	 * graph. All the nodes of the branch and of sub-branches are returned.
	 * 
	 * @param graph
	 *            the graph containing the nodes.
	 * @param branch
	 *            the given branch.
	 * @return the list of nodes representing the states of the given branch in
	 *         graph. If the branch is composed of one single loop, the returned
	 *         list is empty.
	 */
	private List<Vertex> getAllNodesFromBranch(Graph<Vertex, Edge> graph,
			TlaceEdge branch) {
		List<Vertex> vertices = new ArrayList<Vertex>();
		while (branch != null && branch.getTo().getPrev() == branch) {
			vertices.add(getVertexFromTlaceVertex(graph, branch.getTo()));
			for (TlaceEdge subbranch : branch.getTo().getBranches().values()) {
				if (subbranch != null) {
					vertices.addAll(getAllNodesFromBranch(graph, subbranch));
				}
			}
			branch = branch.getTo().getNext();
		}
		return vertices;
	}

	/**
	 * Shows the given branch by adding nodes representing the states of branch,
	 * and linked to node.
	 * 
	 * @param graph
	 *            the graph in which add the nodes.
	 * @param vertex
	 *            the source node of the branch.
	 * @param branch
	 *            the branch to show.
	 */
	private void showBranch(Graph<Vertex, Edge> graph, Vertex vertex,
			TlaceEdge branch) {
		if (branch.getTo().equals(branch.getFrom())) {
			// A loop
			graph.addEdge(new Edge(branch),
					getVertexFromTlaceVertex(graph, branch.getFrom()),
					getVertexFromTlaceVertex(graph, branch.getTo()));
		} else {
			// Add nodes and edges simutaneously
			while (branch != null && branch.getTo().getPrev() == branch) {
				graph.addVertex(new Vertex(branch.getTo()));
				graph.addEdge(new Edge(branch), vertex,
						getVertexFromTlaceVertex(graph, branch.getTo()));

				vertex = getVertexFromTlaceVertex(graph, branch.getTo());
				branch = branch.getTo().getNext();
			}
			if (branch != null) {
				// Loop
				graph.addEdge(new Edge(branch), vertex,
						getVertexFromTlaceVertex(graph, branch.getTo()));
			}
		}
	}

	/**
	 * Hides the given branch by removing all the nodes representing states of
	 * branch (and sub-branches) from graph.
	 * 
	 * @param graph
	 *            the graph containing the nodes.
	 * @param branch
	 *            the branch to hide.
	 */
	private void hideBranch(Graph<Vertex, Edge> graph, TlaceEdge branch) {
		if (branch.getTo().equals(branch.getFrom())) {
			// Single loop : remove the transition
			graph.removeEdge(getEdgeFromTransition(graph, branch));
		} else {
			// Some states are involved : remove the states (the transitions
			// will be automatically removed)
			for (Vertex vertex : getAllNodesFromBranch(graph, branch)) {
				graph.removeVertex(vertex);
				this.list.remove(vertex);
			}
		}
	}

	/**
	 * Returns the edge of the given graph that represents transition.
	 * 
	 * @param graph
	 *            the given graph.
	 * @param tlaceEdge
	 *            the given transition.
	 * @return the edge of graph representing transition if any, null otherwise.
	 */
	private Edge getEdgeFromTransition(Graph<Vertex, Edge> graph,
			TlaceEdge tlaceEdge) {
		for (Edge edge : graph.getEdges()) {
			if (edge.getEdge().equals(tlaceEdge)) {
				return edge;
			}
		}
		return null;
	}
}
