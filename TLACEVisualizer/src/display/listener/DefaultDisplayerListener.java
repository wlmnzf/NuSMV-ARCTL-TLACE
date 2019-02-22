/**
 * 
 */
package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JRadioButtonMenuItem;

import display.GUI;

/**
 * DefaultDisplayerListener is a GUI listener responsible for triggering the
 * change of default counterexample displayer.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class DefaultDisplayerListener implements ActionListener {

	/**
	 * The GUI this listener listens to.
	 */
	private GUI gui;

	/**
	 * The listened button responsible for changing the layout to narrow
	 * tree-like one.
	 */
	private JRadioButtonMenuItem narrowLayoutButton;

	/**
	 * The listened button responsible for changing the layout to wide tree-like
	 * one.
	 */
	private JRadioButtonMenuItem wideLayoutButton;

	/**
	 * The listened button responsible for changing the layout to force-based
	 * one.
	 */
	private JRadioButtonMenuItem forceBasedLayoutButton;

	/**
	 * Creates a new listener.
	 * 
	 * @param gui
	 *            The listened gui.
	 * @param narrowButton
	 *            The radio button representing the narrow tree-like layout.
	 * @param wideButton
	 *            The radio button representing the wide tree-like layout.
	 * @param forceButton
	 *            The radio button representing the force-based layout.
	 */
	public DefaultDisplayerListener(GUI gui, JRadioButtonMenuItem narrowButton,
			JRadioButtonMenuItem wideButton, JRadioButtonMenuItem forceButton) {
		this.gui = gui;
		this.narrowLayoutButton = narrowButton;
		this.wideLayoutButton = wideButton;
		this.forceBasedLayoutButton = forceButton;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent event) {
		if (event.getSource().equals(this.narrowLayoutButton)) {
			this.gui.setDefaultDisplayer(GUI.NARROWTREELIKE);
		} else if (event.getSource().equals(this.wideLayoutButton)) {
			this.gui.setDefaultDisplayer(GUI.WIDETREELIKE);
		} else if (event.getSource().equals(this.forceBasedLayoutButton)) {
			this.gui.setDefaultDisplayer(GUI.FORCEBASED);
		}
	}
}