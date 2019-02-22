package display.layout;

import java.awt.Dimension;
import java.awt.geom.Point2D;

import org.apache.commons.collections15.Transformer;

import display.graph.Edge;
import display.graph.Vertex;
import edu.uci.ics.jung.graph.Graph;

/**
 * A generic layout, getting rid of useless methods.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public abstract class AbstractCounterexampleLayout implements
		CounterexampleLayout {

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#initialize()
	 */
	@Override
	public void initialize() {
		System.err.println("Counterexample Layout : initialize");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#isLocked(java.lang.Object)
	 */
	@Override
	public boolean isLocked(Vertex arg0) {
		System.err.println("Counterexample Layout : isLocked");
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#lock(java.lang.Object,
	 * boolean)
	 */
	@Override
	public void lock(Vertex arg0, boolean arg1) {
		System.err.println("Counterexample Layout : lock");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#reset()
	 */
	@Override
	public void reset() {
		System.err.println("Counterexample Layout : reset");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.algorithms.layout.Layout#setGraph(edu.uci.ics.jung.graph
	 * .Graph)
	 */
	@Override
	public void setGraph(Graph<Vertex, Edge> graph) {
		System.err.println("Counterexample : setGraph");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.algorithms.layout.Layout#setInitializer(org.apache.commons
	 * .collections15.Transformer)
	 */
	@Override
	public void setInitializer(Transformer<Vertex, Point2D> arg0) {
		System.err.println("Counterexample : setInitializer");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.algorithms.layout.Layout#setSize(java.awt.Dimension)
	 */
	@Override
	public void setSize(Dimension arg0) {
		System.err.println("Counterexample Layout : set size");

	}
}
