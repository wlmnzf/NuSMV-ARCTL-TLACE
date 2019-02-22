package display.gui;

import java.util.List;

import javax.swing.tree.DefaultMutableTreeNode;

import tlace.Value;
import tlacegraph.TlaceVertex;

/**
 * The TreeTableList represents a list of TLACE vertices in a treetable.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TreeTlaceList extends DefaultMutableTreeNode {

	private static final long serialVersionUID = -2134159787488272829L;

	private String name;

	private List<TlaceVertex> nodes;

	private boolean changedVar;

	public TreeTlaceList(String name, List<TlaceVertex> nodes,
			boolean changedVar) {
		this.name = name;
		this.nodes = nodes;
		this.changedVar = changedVar;
	}

	public String getName(Integer column) {
		return name;
	}

	public String getValueFor(Integer column) {
		if (column > 1 & this.changedVar) {
			TlaceVertex prev = this.nodes.get(column - 2);
			String prevVal = getValueFromVariableHierarchy(prev, this);
			String curVal = getValueFromVariableHierarchy(
					this.nodes.get(column - 1), this);
			if (prevVal!= null & curVal != null && !prevVal.equals(curVal)) {
				return "<html><span style='color:red;'><strong>" + curVal
						+ "</strong></span></html>";
			}
		}
		return getValueFromVariableHierarchy(
				this.nodes.get(column.intValue() - 1), this);
	}

	/**
	 * Returns the variable hierarchy representing the variables of the state of
	 * tlaceVertex.
	 * 
	 * @param tlaceVertex
	 *            the vertex of interest.
	 * @return a variable hierarchy node representing the variable hierarchy of
	 *         the state of tlaceVertex.
	 */
	public static TreeTlaceList getVariableHierarchy(TlaceVertex tlaceVertex,
			List<TlaceVertex> nodes, boolean changedVar) {
		Value[] values = tlaceVertex.getState();
		TreeTlaceList root = new TreeTlaceList("State", nodes, changedVar);
		for (Value value : values) {
			addChild(root, value.getVariable(), nodes, changedVar);
		}
		return root;
	}

	/**
	 * Adds to the hierarchy root the nodes representing the hierarchy of path.
	 * 
	 * @param root
	 *            the node to add the new hierarchy.
	 * @param path
	 *            the path to add. path != "" and path != null.
	 */
	private static void addChild(TreeTlaceList root, String path,
			List<TlaceVertex> nodes, boolean changedVar) {
		int index = path.indexOf('.');
		String head;
		String tail;
		if (index < 0) {
			head = path;
			tail = "";
		} else {
			head = path.substring(0, index);
			tail = path.substring(index + 1);
		}

		for (int i = 0; i < root.getChildCount(); i++) {
			TreeTlaceList node = (TreeTlaceList) root.getChildAt(i);
			if (node.getName(0).equals(head)) {
				if (!tail.equals("")) {
					addChild(node, tail, nodes, changedVar);
				}
				return;
			}
		}

		TreeTlaceList newNode = new TreeTlaceList(head, nodes, changedVar);
		root.add(newNode);
		if (!tail.equals("")) {
			addChild(newNode, tail, nodes, changedVar);
		}
	}

	/**
	 * Returns the value of the variable in vertex' state represented by var.
	 * 
	 * @param vertex
	 *            the vertex of interest.
	 * @param var
	 *            the wanted variable.
	 * @return the value of the variable represented by var (and its hierarchy)
	 *         in the state of vertex, if any, null otherwise.
	 */
	private static String getValueFromVariableHierarchy(TlaceVertex vertex,
			TreeTlaceList var) {
		String variable = var.getName(0);
		while (var.getParent() != null) {
			variable = ((TreeTlaceList) var.getParent()).getName(0) + "."
					+ variable;
			var = (TreeTlaceList) var.getParent();
		}

		variable = variable.substring(variable.indexOf('.') + 1);

		for (Value value : vertex.getState()) {
			if (value.getVariable().equals(variable)) {
				return value.getValue();
			}
		}
		return null;
	}

	public String toString() {
		return this.name;
	}
}
