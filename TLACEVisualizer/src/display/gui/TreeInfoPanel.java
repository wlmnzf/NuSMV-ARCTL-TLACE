package display.gui;

import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultListModel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.UIManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;

import tlace.Value;
import tlacegraph.TlaceEdge;
import tlacegraph.TlaceVertex;

/**
 * A tree info panel is an info panel displaying information about states and
 * transitions in boxes containing trees and lists.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TreeInfoPanel extends InfoPanel {

	private static final long serialVersionUID = 1L;

	/**
	 * Creates a new info panel displaying info into trees in boxes
	 */
	public TreeInfoPanel() {
		super();
	}

	/**
	 * Displays the information contained in the given state.
	 * 
	 * @param vertex
	 *            the state to display.
	 * @param prevVertex
	 *            the state before state. If not null and this.changedVar is
	 *            true, then only the state variables that changed during the
	 *            transition between prevState and state are displayed.
	 */
	private void displayStateInfo(TlaceVertex vertex, TlaceVertex prevVertex) {
		// Create the panel
		JPanel statePanel = new JPanel();
		statePanel.setBackground(UIManager.getColor("Tree.background"));
		statePanel.setLayout(new BoxLayout(statePanel, BoxLayout.Y_AXIS));
		this.add(statePanel);
		statePanel.setBorder(BorderFactory.createEtchedBorder());
		statePanel.setAlignmentX(1);

		// Create the tree
		DefaultMutableTreeNode rootNode = new DefaultMutableTreeNode();
		// State
		DefaultMutableTreeNode stateNode = new DefaultMutableTreeNode("State "
				+ vertex.getId());
		rootNode.add(stateNode);
		for (Value value : vertex.getState()) {
			if (!this.changedVar
					|| prevVertex == null
					|| !getValueFromVariable(prevVertex, value.getVariable())
							.equals(value.getValue())) {
				stateNode.add(new DefaultMutableTreeNode(value));
			}
		}

		// Annotations
		if (vertex.getAnnotations().length > 0) {
			DefaultMutableTreeNode annoNode = new DefaultMutableTreeNode(
					"Annotations");
			rootNode.add(annoNode);
			for (String annotation : vertex.getAnnotations()) {
				annoNode.add(new DefaultMutableTreeNode(annotation));
			}
		}

		// Check if some branches have to be displayed
		boolean haveBranches = false;
		for (String branch : vertex.getBranches().keySet()) {
			if (vertex.getBranches().get(branch) != TlaceEdge.getNull()) {
				haveBranches = true;
			}
		}
		// Add branches if needed
		if (haveBranches) {
			DefaultMutableTreeNode branchesNode = new DefaultMutableTreeNode(
					"Branches");
			rootNode.add(branchesNode);
			for (String branch : vertex.getBranches().keySet()) {
				TlaceEdge branchEdge = vertex.getBranches().get(branch);
				if (branchEdge != TlaceEdge.getNull()) {
					branchesNode.add(new DefaultMutableTreeNode(branch));
				}
			}
		}

		// Actual tree
		JTree tree = new JTree(rootNode);
		tree.setShowsRootHandles(true);
		tree.setRootVisible(false);
		DefaultTreeCellRenderer cellRenderer = new DefaultTreeCellRenderer();
		cellRenderer.setClosedIcon(null);
		cellRenderer.setLeafIcon(null);
		cellRenderer.setOpenIcon(null);
		tree.setCellRenderer(cellRenderer);

		// Expand state
		tree.expandRow(0);

		// Add the tree to the display
		statePanel.add(tree);
		statePanel.add(Box.createHorizontalGlue());
		tree.setAlignmentX(0);

		this.panel.adjustSplitpane();
	}

	/**
	 * Returns the value of variable of state.
	 * 
	 * @param vertex
	 *            the state to search.
	 * @param variable
	 *            the variable to search.
	 * @return If state contains a value to the variable variable, returns this
	 *         value, null otherwise.
	 */
	private String getValueFromVariable(TlaceVertex vertex, String variable) {
		for (Value value : vertex.getState()) {
			if (value.getVariable().equals(variable)) {
				return value.getValue();
			}
		}
		return null;
	}

	/**
	 * Displays the information contained in the given transition.
	 * 
	 * @param edge
	 *            the transition to display.
	 */
	private void displayTransitionInfo(TlaceEdge edge) {
		if (edge.getInputs().length > 0) {

			JPanel inputPanel = new JPanel();
			inputPanel.setBackground(UIManager.getColor("List.background"));
			this.add(inputPanel);
			inputPanel.setLayout(new BoxLayout(inputPanel, BoxLayout.X_AXIS));
			inputPanel.setBorder(BorderFactory.createEtchedBorder());
			DefaultListModel model = new DefaultListModel();
			for (Value value : edge.getInputs()) {
				model.addElement(value);
			}
			JList input1List = new JList(model);
			inputPanel.add(input1List);
			inputPanel.setAlignmentX(1);

			inputPanel.add(Box.createHorizontalGlue());

			this.panel.adjustSplitpane();
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.InfoPanel#displayStatesInfo(java.util.List)
	 */
	@Override
	protected void displayStatesInfo(List<TlaceVertex> vertices) {
		this.removeAll();
		if (vertices.size() > 0) {
			displayStateInfo(vertices.get(0), null);
			TlaceVertex prevVertex = vertices.get(0);
			for (int i = 1; i < vertices.size(); i++) {
				displayTransitionInfo(getTransitionFromOpposite(prevVertex,
						vertices.get(i)));
				displayStateInfo(vertices.get(i), prevVertex);
				prevVertex = vertices.get(i);
			}
		}
	}

	/**
	 * Returns the transition going from source to destination.
	 * 
	 * @param source
	 *            the source of the wished transition.
	 * @param destination
	 *            the destination of the wished transition.
	 * @return the transition between source and destination if any, null
	 *         otherwise.
	 */
	private TlaceEdge getTransitionFromOpposite(TlaceVertex source,
			TlaceVertex destination) {
		for (TlaceEdge edge : source.getNexts()) {
			if (edge.getTo().equals(destination)) {
				return edge;
			}
		}
		return null;
	}
}
