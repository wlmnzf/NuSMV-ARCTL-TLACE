package display.layout;

import java.awt.Dimension;
import java.awt.geom.Point2D;
import java.util.HashMap;
import java.util.Map;

import application.Settings;
import display.graph.Graph;
import display.graph.Vertex;

/**
 * The tree like layout layouts a counterexample displaying vertical branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public abstract class TreeLikeLayout extends AbstractCounterexampleLayout
		implements CounterexampleLayout {

	/**
	 * The current displayed graph.
	 */
	protected Graph graph;

	/**
	 * The current location of nodes;
	 */
	protected Map<Vertex, Point2D> locations;

	/**
	 * Creates a new layout.
	 */
	public TreeLikeLayout() {
		this.locations = new HashMap<Vertex, Point2D>();
		this.graph = null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.commons.collections15.Transformer#transform(java.lang.Object)
	 */
	@Override
	public Point2D transform(Vertex vertex) {
		return this.locations.get(vertex);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#getGraph()
	 */
	@Override
	public Graph getGraph() {
		return this.graph;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.uci.ics.jung.algorithms.layout.Layout#getSize()
	 */
	@Override
	public Dimension getSize() {
		double minx = Integer.MAX_VALUE;
		double miny = Integer.MAX_VALUE;
		double maxx = Integer.MIN_VALUE;
		double maxy = Integer.MIN_VALUE;

		for (Point2D point : this.locations.values()) {
			if (point.getX() < minx) {
				minx = point.getX();
			} else if (point.getX() > maxx) {
				maxx = point.getX();
			}

			if (point.getY() < miny) {
				miny = point.getY();
			} else if (point.getY() > maxy) {
				maxy = point.getY();
			}
		}

		return new Dimension((int) Math.round(Math.max(0, maxx - minx)),
				(int) Math.round(Math.max(0, maxy - miny)));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.uci.ics.jung.algorithms.layout.Layout#setLocation(java.lang.Object,
	 * java.awt.geom.Point2D)
	 */
	@Override
	public void setLocation(Vertex vertex, Point2D location) {
		this.locations.put(vertex, location);
	}

	/**
	 * Layouts the intern graph by storing each node location in locations.
	 */
	protected abstract void buildLayout();

	/**
	 * Put the given node at location (x,y).
	 * 
	 * @param vertex
	 *            The given node.
	 * @param x
	 *            The x-axis position.
	 * @param y
	 *            The y-axis position.
	 */
	protected void drawNode(Vertex vertex, int x, int y) {
		this.locations.put(vertex, new Point2D.Double(
				((double) Settings.graphPadding + y * Settings.vertexSpace),
				(double) (Settings.graphPadding + x * Settings.vertexSpace)));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * display.layout.CounterexampleLayout#initialize(counterexample.Counterexample
	 * , edu.uci.ics.jung.graph.Graph)
	 */
	@Override
	public void initialize(Graph graph) {
		this.graph = graph;
		this.locations = new HashMap<Vertex, Point2D>();

		this.buildLayout();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.layout.AbstractCounterexampleLayout#reset()
	 */
	@Override
	public void reset() {
		this.buildLayout();
	}
}
