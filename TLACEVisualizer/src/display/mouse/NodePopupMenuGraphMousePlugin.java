package display.mouse;

import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.util.HashMap;
import java.util.Map;

import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;

import tlace.Value;
import tlacegraph.TlaceEdge;
import util.ObservableList;
import display.graph.Edge;
import display.graph.Vertex;
import display.listener.BranchListener;
import edu.uci.ics.jung.algorithms.layout.GraphElementAccessor;
import edu.uci.ics.jung.graph.Graph;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import edu.uci.ics.jung.visualization.control.AbstractPopupGraphMousePlugin;

/**
 * The node popup menu graph mouse plugin displays a popup when clicking with
 * right button on a node. This menu displays the list of state values of this
 * node, its annotation and the list of its branches. Each branch can be focused
 * and shown or hidden depending on its current visibility.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class NodePopupMenuGraphMousePlugin extends
		AbstractPopupGraphMousePlugin {

	/**
	 * A map of node popup menus.
	 */
	private Map<Vertex, JPopupMenu> menus;

	/**
	 * The list of selected nodes, used to initialize branch listeners.
	 */
	private ObservableList<Vertex> list;

	/**
	 * Creates a new node popup graph mouse plugin based on list.
	 * 
	 * @param list
	 *            a list of selected nodes.
	 */
	public NodePopupMenuGraphMousePlugin(ObservableList<Vertex> list) {
		this.menus = new HashMap<Vertex, JPopupMenu>();
		this.list = list;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.visualization.control.AbstractPopupGraphMousePlugin#
	 * handlePopup(java.awt.event.MouseEvent)
	 */
	@SuppressWarnings("unchecked")
	@Override
	protected void handlePopup(MouseEvent event) {
		if (event.getSource() instanceof VisualizationViewer) {
			VisualizationViewer<Vertex, Edge> vv = (VisualizationViewer<Vertex, Edge>) event
					.getSource();
			GraphElementAccessor<Vertex, Edge> pickSupport = vv
					.getPickSupport();

			Vertex vertex = pickSupport.getVertex(vv.getGraphLayout(), event
					.getPoint().getX(), event.getPoint().getY());

			Graph<Vertex, Edge> graph = vv.getGraphLayout().getGraph();

			// A node has been clicked
			if (vertex != null) {
				if (!menus.containsKey(vertex)) {
					// Create the popupMenu
					JPopupMenu popupMenu = new JPopupMenu(
							Integer.toString(vertex.getVertex().getId()));

					JMenu valuesMenu = new JMenu("State");
					// Display state values
					for (Value value : vertex.getVertex().getState()) {
						JMenuItem item = new JMenuItem(value.toString());
						item.setEnabled(false);
						valuesMenu.add(item);
					}
					popupMenu.add(valuesMenu);

					// Add annotation
					if (vertex.getVertex().getAnnotations().length > 0) {
						popupMenu.add(new JSeparator());
						JMenu annotationMenu = new JMenu("Annotations");
						for (String annotation : vertex.getVertex()
								.getAnnotations()) {
							JMenuItem annotationItem = new JMenuItem(annotation);
							annotationItem.setEnabled(false);
							annotationMenu.add(annotationItem);
						}
						popupMenu.add(annotationMenu);
					}

					// Check if some branches have to be displayed
					boolean haveBranches = false;
					for (String branch : vertex.getVertex().getBranches()
							.keySet()) {
						if (vertex.getVertex().getBranches().get(branch) != TlaceEdge.getNull()) {
							haveBranches = true;
						}
					}
					// Add branches if needed
					if (haveBranches) {
						popupMenu.add(new JSeparator());
						JMenu branchMenu = new JMenu("Branches");
						popupMenu.add(branchMenu);
						for (String branch : vertex.getVertex().getBranches()
								.keySet()) {
							TlaceEdge branchEdge = vertex.getVertex()
									.getBranches().get(branch);
							if (branchEdge != TlaceEdge.getNull()) {
								JMenu menu = new JMenu(branch);
								JMenuItem showHide = new JMenuItem(
										(vertex.isDisplayed(branchEdge) ? "Hide"
												: "Show"));
								JMenuItem see = new JMenuItem("See");
								menu.add(showHide);
								menu.add(see);

								// Add listeners
								ActionListener listener = new BranchListener(
										graph, vertex, branchEdge, list,
										showHide, see, vv);
								showHide.addActionListener(listener);
								see.addActionListener(listener);

								branchMenu.add(menu);
							}
						}
					}

					menus.put(vertex, popupMenu);
				}

				menus.get(vertex).show(vv, event.getX(), event.getY());
			}

			event.consume();
		}
	}
}
