package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JRadioButtonMenuItem;

import display.GUI;

/**
 * An info panel listener is responsible for the change of info panel type in
 * the GUI.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class DefaultInfoPanelListener implements ActionListener {

	/**
	 * The GUI this listener acts on.
	 */
	private GUI gui;

	/**
	 * The listened menu item responsible for the tree info panel.
	 */
	private JRadioButtonMenuItem treeInfoPanelButton;

	/**
	 * The listened menu item responsible for the table info panel.
	 */
	private JRadioButtonMenuItem tableInfoPanelButton;

	/**
	 * Creates a new info panel listener acting on gui and listening
	 * treeInfoPanel.
	 * 
	 * @param gui
	 *            the GUI the new listener will act on.
	 * @param treeInfoPanel
	 *            the listened menu item choosing tree info panel type.
	 * @param tableInfoPanel
	 *            the listened menu item choosing table info panel type.
	 */
	public DefaultInfoPanelListener(GUI gui, JRadioButtonMenuItem treeInfoPanel,
			JRadioButtonMenuItem tableInfoPanel) {
		this.gui = gui;
		this.treeInfoPanelButton = treeInfoPanel;
		this.tableInfoPanelButton = tableInfoPanel;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent event) {
		if (event.getSource().equals(this.treeInfoPanelButton)) {
			this.gui.setInfoPanelType(GUI.TREEINFOPANEL);
		} else if (event.getSource().equals(this.tableInfoPanelButton)) {
			this.gui.setInfoPanelType(GUI.TABLEINFOPANEL);
		}
	}

}
