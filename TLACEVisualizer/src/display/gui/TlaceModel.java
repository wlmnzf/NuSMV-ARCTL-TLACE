package display.gui;

import javax.swing.tree.TreeNode;

import util.treetable.DynamicTreeTableModel;

/**
 * The TlaceModel is used to display TLACEs using a treetable.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TlaceModel extends DynamicTreeTableModel {

	public TlaceModel(TreeNode root, String[] columnNames,
			String[] getterMethodNames, String[] setterMethodNames,
			Class<?>[] cTypes) {
		super(root, columnNames, getterMethodNames, setterMethodNames, cTypes);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * util.treetable.DynamicTreeTableModel#isCellEditable(java.lang.Object,
	 * int)
	 */
	@Override
	public boolean isCellEditable(Object node, int column) {
		if (((TreeNode) node).isLeaf()) {
			return false;
		} else {
			return true;
		}
	}
}
