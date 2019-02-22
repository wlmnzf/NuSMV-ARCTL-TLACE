package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Map;

import javax.swing.JCheckBoxMenuItem;

import display.gui.CounterexamplePanel;
import display.transformer.StateLabelTransformer;
import display.transformer.TransitionLabelTransformer;

/**
 * The variable listener is used to listen menu items responsible of the display
 * of values on the graph.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class VariableListener implements ActionListener {

	/**
	 * The panel this listener acts on.
	 */
	private CounterexamplePanel panel;

	/**
	 * A map of each menu item and the state variable referred to.
	 */
	private Map<JCheckBoxMenuItem, String> buttonStates;

	/**
	 * A map of each menu item and the input variable referred to.
	 */
	private Map<JCheckBoxMenuItem, String> buttonInputs;

	/**
	 * The edge label transformer, modified to display input variables (or not).
	 */
	private TransitionLabelTransformer inputTransformer;

	/**
	 * The node label transformer, modified to display state variables (or not).
	 */
	private StateLabelTransformer stateTransformer;

	/**
	 * Creates a new variable listener acting on panel, using buttStates and
	 * buttonInputs to know the variable names associated to menu items, and
	 * modifying stateTransformer and inputTransformer to change the subset of
	 * displayed values.
	 * 
	 * @param panel
	 *            The panel to act on.
	 * @param buttonStates
	 *            A map of menu items to state variables names.
	 * @param buttonInputs
	 *            A map of menu items to input variables names.
	 * @param stateTransformer
	 *            The transformer to change to change the subset of displayed
	 *            state values.
	 * @param inputTransformer
	 *            The transformer to change to change the subset of displayed
	 *            input values.
	 */
	public VariableListener(CounterexamplePanel panel,
			Map<JCheckBoxMenuItem, String> buttonStates,
			Map<JCheckBoxMenuItem, String> buttonInputs,
			StateLabelTransformer stateTransformer,
			TransitionLabelTransformer inputTransformer) {
		this.panel = panel;
		this.buttonStates = buttonStates;
		this.buttonInputs = buttonInputs;
		this.stateTransformer = stateTransformer;
		this.inputTransformer = inputTransformer;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent event) {
		if (event.getSource() instanceof JCheckBoxMenuItem) {

			JCheckBoxMenuItem menuItem = (JCheckBoxMenuItem) event.getSource();

			// The event source is a state variable checkbox
			if (buttonStates.containsKey(menuItem)) {
				if (menuItem.isSelected()) {
					this.stateTransformer.addVariable(this.buttonStates
							.get(menuItem));
				} else {
					this.stateTransformer.removeVariable(this.buttonStates
							.get(menuItem));
				}
				this.panel.repaintCounterexample();
			}
			// The event source is an input variable checkbox
			else if (buttonInputs.containsKey(menuItem)) {
				if (menuItem.isSelected()) {
					this.inputTransformer.addVariable(this.buttonInputs
							.get(menuItem));
				} else {
					this.inputTransformer.removeVariable(this.buttonInputs
							.get(menuItem));
				}
				this.panel.repaintCounterexample();
			}
		}
	}
}
