/**
 * 
 */
package display.layout;

import display.graph.Edge;
import display.graph.Graph;
import display.graph.Vertex;
import edu.uci.ics.jung.algorithms.layout.Layout;

/**
 * A Counterexample Layout is a particular layout used to display
 * counterexamples.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public interface CounterexampleLayout extends Layout<Vertex, Edge> {

	/**
	 * Initializes the layout to display counter-examples. graph is the
	 * representation of a counter-example.
	 * 
	 * @param graph
	 *            The representation of counterexample.
	 */
	public void initialize(Graph graph);

	/**
	 * Returns the type of this layout.
	 * 
	 * @return the type of this layout: GUI.NARROWTREELIKE, GUI.WIDETREELIKE or
	 *         GUI.FORCEBASED.
	 */
	public int getType();

}
