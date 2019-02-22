/**
 * 
 */
package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JRadioButtonMenuItem;

import display.gui.CounterexamplePanel;
import display.gui.InfoPanel;
import display.gui.TableInfoPanel;
import display.gui.TreeInfoPanel;

/**
 * InfoPanelListener is a GUI listener responsible for triggering the change of
 * info panel.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class InfoPanelListener implements ActionListener {

	/**
	 * The panel this listener listens to.
	 */
	private CounterexamplePanel panel;

	/**
	 * The listened button responsible for changing the info displayer to the
	 * tree one.
	 */
	private JRadioButtonMenuItem treeInfo;

	/**
	 * The listened button responsible for changing the info displayer to the
	 * table one.
	 */
	private JRadioButtonMenuItem tableInfo;

	/**
	 * The checkbox menu item saying whether only changed var have to be
	 * displayed or not.
	 */
	private JCheckBoxMenuItem changedVar;

	/**
	 * Creates a new listener.
	 * 
	 * @param panel
	 *            The listened panel.
	 * @param treeButton
	 *            The radio button representing the tree displayer.
	 * @param tableButton
	 *            The radio button representing the table displayer.
	 * @param changedVar
	 *            the checkbox menu item saying whether only changed var have to
	 *            be displayed or not.
	 */
	public InfoPanelListener(CounterexamplePanel panel,
			JRadioButtonMenuItem treeButton, JRadioButtonMenuItem tableButton,
			JCheckBoxMenuItem changedVar) {
		this.panel = panel;
		this.treeInfo = treeButton;
		this.tableInfo = tableButton;
		this.changedVar = changedVar;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent event) {
		InfoPanel panel = null;
		if (event.getSource().equals(this.treeInfo)) {
			panel = new TreeInfoPanel();
		} else if (event.getSource().equals(this.tableInfo)) {
			panel = new TableInfoPanel();
		}
		if (panel != null) {
			panel.setChangedVariablesDisplay(this.changedVar.isSelected());
			this.panel.setInfoPanel(panel);
		}
	}
}