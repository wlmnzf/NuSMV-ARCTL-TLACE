package display.mouse;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.List;

import util.ObservableList;
import display.graph.Edge;
import display.graph.Vertex;
import edu.uci.ics.jung.algorithms.layout.GraphElementAccessor;
import edu.uci.ics.jung.algorithms.shortestpath.DijkstraShortestPath;
import edu.uci.ics.jung.graph.Graph;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import edu.uci.ics.jung.visualization.control.GraphMousePlugin;

/**
 * The selecting graph mouse plugin is a graph mouse plugin that keeps track of
 * a set of selected nodes. The rules of selection are particular:
 * 
 * - If the user doesn't click on a state (node = null), then all the selection
 * is cleared.
 * 
 * - If the user clicks on an unselected state and there is no other selected
 * state, then this state is selected.
 * 
 * - If the user clicks on an unselected state and there are other selected
 * states:
 * 
 * + if there is a path from the first selected state, this path is selected
 * (and everything else is unselected)
 * 
 * + if there is no path, the new state is lonely selected
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class SelectingGraphMousePlugin implements GraphMousePlugin,
		MouseListener {

	/**
	 * The modifiers activating this plugin.
	 */
	private int modifiers;

	/**
	 * The panel to act on.
	 */
	private ObservableList<Vertex> list;

	/**
	 * Creates a new selecting graph mouse plugin activated without a mask.
	 * 
	 * @param list
	 *            the list in which the new plugin will keep track of selected
	 *            nodes.
	 */
	public SelectingGraphMousePlugin(ObservableList<Vertex> list) {
		this(MouseEvent.BUTTON1_MASK, list);
	}

	/**
	 * Createas a new selecting graph mouse plugin activated with modifiers.
	 * 
	 * @param modifiers
	 *            The modifiers activating the new plugin.
	 * @param list
	 *            the list in which the new plugin will keep track of selected
	 *            nodes.
	 */
	public SelectingGraphMousePlugin(int modifiers, ObservableList<Vertex> list) {
		this.modifiers = modifiers;
		this.list = list;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.visualization.control.GraphMousePlugin#checkModifiers
	 * (java.awt.event.MouseEvent)
	 */
	@Override
	public boolean checkModifiers(MouseEvent event) {
		return event.getModifiers() == this.modifiers;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.visualization.control.GraphMousePlugin#getModifiers()
	 */
	@Override
	public int getModifiers() {
		return this.modifiers;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.visualization.control.GraphMousePlugin#setModifiers(int)
	 */
	@Override
	public void setModifiers(int modifiers) {
		this.modifiers = modifiers;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.event.MouseListener#mouseClicked(java.awt.event.MouseEvent)
	 */
	@Override
	@SuppressWarnings("unchecked")
	public void mouseClicked(MouseEvent event) {
		if (event.getModifiers() == this.modifiers) {
			if (event.getSource() instanceof VisualizationViewer) {
				VisualizationViewer<Vertex, Edge> vv = (VisualizationViewer<Vertex, Edge>) event
						.getSource();
				GraphElementAccessor<Vertex, Edge> pickSupport = vv
						.getPickSupport();

				Vertex vertex = pickSupport.getVertex(vv.getGraphLayout(), event
						.getPoint().getX(), event.getPoint().getY());

				/*
				 * - If the user doesn't click on a state (node = null), then
				 * all the selection is cleared.
				 * 
				 * - If the user clicks on an unselected state and there is no
				 * other selected state, then this state is selected.
				 * 
				 * - If the user clicks on an unselected state and there are
				 * other selected states:
				 * 
				 * + if there is a path from the first selected state, this path
				 * is selected (and everything else is unselected)
				 * 
				 * + if there is no path, the new state is lonely selected
				 */
				if (vertex != null) {
					if (this.list.isEmpty()) {
						this.list.add(vertex);
					} else {
						Graph<Vertex, Edge> graph = vv.getGraphLayout()
								.getGraph();
						DijkstraShortestPath<Vertex, Edge> dijkstra = new DijkstraShortestPath<Vertex, Edge>(
								graph);
						Vertex firstVertex = this.list.get(0);
						List<Edge> path = dijkstra.getPath(firstVertex, vertex);

						this.list.clear();
						if (path.size() > 0) {
							this.list.add(firstVertex);
							for (Edge edge : path) {
								this.list.add(graph.getDest(edge));
							}
						} else {
							this.list.add(vertex);
						}
					}
				} else {
					this.list.clear();
				}
				this.list.notifyObservers();
				event.consume();
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.event.MouseListener#mouseEntered(java.awt.event.MouseEvent)
	 */
	@Override
	public void mouseEntered(MouseEvent arg0) {
		// Do nothing
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.event.MouseListener#mouseExited(java.awt.event.MouseEvent)
	 */
	@Override
	public void mouseExited(MouseEvent arg0) {
		// Do nothing
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.event.MouseListener#mousePressed(java.awt.event.MouseEvent)
	 */
	@Override
	public void mousePressed(MouseEvent arg0) {
		// Do nothing
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.MouseListener#mouseReleased(java.awt.event.MouseEvent)
	 */
	@Override
	public void mouseReleased(MouseEvent arg0) {
		// Do nothing
	}

}
