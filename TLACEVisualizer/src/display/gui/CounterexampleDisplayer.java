package display.gui;

import tlacegraph.TlaceGraph;

/**
 * Given a counterexample, CounterexampleDisplayer displays it on a given panel.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public interface CounterexampleDisplayer {

	/**
	 * Displays on panel the given counterexample.
	 * 
	 * @param counterexample
	 *            the counterexample to display.
	 * @param panel
	 *            the panel on which draw the counterexample.
	 */
	public void display(TlaceGraph counterexample, CounterexamplePanel panel);

}