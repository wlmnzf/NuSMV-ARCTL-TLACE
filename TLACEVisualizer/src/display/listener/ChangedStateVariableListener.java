package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JCheckBoxMenuItem;

import util.ObservableList;

import display.graph.Vertex;
import display.gui.InfoPanel;

/**
 * This listener tells the given panel whether or not only display changed
 * variables when displaying a path of states.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class ChangedStateVariableListener implements ActionListener {

	/**
	 * The panel this listener acts on.
	 */
	private InfoPanel panel;

	/**
	 * The list of selected nodes.
	 */
	private ObservableList<Vertex> selectedNodes;

	/**
	 * Creates a new listener acting on panel.
	 * 
	 * @param panel
	 *            the panel to act on.
	 * @param selectedNodes
	 *            the list of selected nodes.
	 */
	public ChangedStateVariableListener(InfoPanel panel,
			ObservableList<Vertex> selectedNodes) {
		this.panel = panel;
		this.selectedNodes = selectedNodes;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() instanceof JCheckBoxMenuItem) {
			JCheckBoxMenuItem menuItem = (JCheckBoxMenuItem) e.getSource();
			this.panel.setChangedVariablesDisplay(menuItem.isSelected());
			this.panel.update(this.selectedNodes, null);
		}
	}

	/**
	 * Sets the info panel of this listener to infoPanel.
	 * 
	 * @param infoPanel
	 *            the new info panel.
	 */
	public void setInfoPanel(InfoPanel infoPanel) {
		this.panel = infoPanel;
	}

}
