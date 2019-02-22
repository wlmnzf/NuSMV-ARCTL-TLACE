/**
 * 
 */
package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JRadioButtonMenuItem;

import display.GUI;
import display.gui.CounterexamplePanel;

/**
 * LayoutListener is a GUI listener responsible for triggering the change of
 * counterexample displayer.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class LayoutListener implements ActionListener {

	/**
	 * The panel this listener listens to.
	 */
	private CounterexamplePanel panel;

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
	 * @param panel
	 *            The listened panel.
	 * @param narrowButton
	 *            The radio button representing the narrow tree-like layout.
	 * @param wideButton
	 *            The radio button representing the wide tree-like layout.
	 * @param forceButton
	 *            The radio button representing the force-based layout.
	 */
	public LayoutListener(CounterexamplePanel panel,
			JRadioButtonMenuItem narrowButton, JRadioButtonMenuItem wideButton,
			JRadioButtonMenuItem forceButton) {
		this.panel = panel;
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
			this.panel.setGraphLayout(GUI
					.getNewLayoutFromType(GUI.NARROWTREELIKE));
			this.panel.redrawCounterexample();
		} else if (event.getSource().equals(this.wideLayoutButton)) {
			this.panel.setGraphLayout(GUI
					.getNewLayoutFromType(GUI.WIDETREELIKE));
			this.panel.redrawCounterexample();
		} else if (event.getSource().equals(this.forceBasedLayoutButton)) {
			this.panel
					.setGraphLayout(GUI.getNewLayoutFromType(GUI.FORCEBASED));
			this.panel.redrawCounterexample();
		}
	}
}