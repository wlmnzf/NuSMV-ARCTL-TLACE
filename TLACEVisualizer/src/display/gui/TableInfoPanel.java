package display.gui;

import java.awt.BorderLayout;
import java.util.List;

import tlacegraph.TlaceVertex;
import util.treetable.JTreeTable;
import util.treetable.TreeTableModel;

/**
 * A table info panel is an info panel displaying information in a table. This
 * info panel doesn't display input variables.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TableInfoPanel extends InfoPanel {

	private static final long serialVersionUID = 1L;

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.gui.InfoPanel#displayStatesInfo(java.util.List)
	 */
	@Override
	protected void displayStatesInfo(List<TlaceVertex> nodes) {
		this.removeAll();
		if (nodes.size() > 0) {
			TreeTlaceList root = TreeTlaceList.getVariableHierarchy(
					nodes.get(0), nodes, this.changedVar);

			String[] columnNames = new String[nodes.size() + 1];
			columnNames[0] = "";
			for (int i = 0; i < nodes.size(); i++) {
				columnNames[i + 1] = nodes.get(i).getId() + "";
			}

			String[] getterMethodNames = new String[nodes.size() + 1];
			getterMethodNames[0] = "getName";
			for (int i = 0; i < nodes.size(); i++) {
				getterMethodNames[i + 1] = "getValueFor";
			}

			Class<?>[] cTypes = new Class<?>[nodes.size() + 1];
			cTypes[0] = TreeTableModel.class;
			for (int i = 0; i < nodes.size(); i++) {
				cTypes[i + 1] = String.class;
			}

			TlaceModel model = new TlaceModel(root, columnNames,
					getterMethodNames, null, cTypes);
			JTreeTable treeTable = new JTreeTable(model);
			this.setLayout(new BorderLayout());
			this.add(treeTable.getTableHeader(), BorderLayout.NORTH);
			this.add(treeTable, BorderLayout.CENTER);

			this.panel.adjustSplitpane();
		}
	}
}
