/**
 * 
 */
package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import display.GUI;
import display.gui.CounterexamplePanel;

/**
 * This listener is used to close a tab.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TabListener implements ActionListener {

	/**
	 * The listened GUI.
	 */
	private GUI gui;

	/**
	 * The listened panel.
	 */
	private CounterexamplePanel panel;

	/**
	 * Creates a new listener listening gui and panel.
	 * 
	 * @param gui
	 *            The GUI to listen to.
	 * @param panel
	 *            The panel to listen to.
	 */
	public TabListener(GUI gui, CounterexamplePanel panel) {
		this.gui = gui;
		this.panel = panel;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent arg0) {
		this.gui.close(this.panel);
	}

}
