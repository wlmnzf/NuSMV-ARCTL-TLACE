/**
 * 
 */
package display.layout;

import java.awt.Dimension;
import java.awt.geom.Point2D;
import java.util.HashMap;
import java.util.Map;

import application.Settings;
import display.GUI;
import display.graph.Edge;
import display.graph.Graph;
import display.graph.Vertex;

/**
 * The force-based layout layouts counterexamples using a tuned force-based
 * algorithm.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class ForceBasedLayout extends AbstractCounterexampleLayout implements
		CounterexampleLayout {

	/**
	 * The representation of the layouted counterexample.
	 */
	private Graph graph;

	/**
	 * The location of each node of the graph.
	 */
	private Map<Vertex, Point2D> locations;

	/**
	 * The time step used for the layout algorithm.
	 */
	private double timeStep = 0.5;

	/**
	 * The damping factor used for the layout algorithm.
	 */
	private double damping = 0.9;

	/**
	 * The mass of the nodes used for the layout algorithm.
	 */
	private double nodeMass = 1;

	/**
	 * The gravity factor used for the layout algorithm.
	 */
	private double gravity = 0.4;

	/**
	 * The spring constant used for the layout algorithm.
	 */
	private double springConstant = 1;

	/**
	 * The spring length used for the layout algorithm.
	 */
	private double springLength = Settings.vertexSpace * 1.2;

	/**
	 * The coulomb constant used for the layout algorithm.
	 */
	private double coulombConstant = 0.05;

	/**
	 * The electric charge of nodes used for the layout algorithm.
	 */
	private double nodeCharge = Settings.vertexSpace * 3.0;

	/**
	 * The threshold used on the total kinetic energy.
	 */
	private double threshold = 0.01;

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
	 * @see
	 * edu.uci.ics.jung.algorithms.layout.Layout#setLocation(java.lang.Object,
	 * java.awt.geom.Point2D)
	 */
	@Override
	public void setLocation(Vertex vertex, Point2D location) {
		this.locations.put(vertex, location);
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
	 * @see
	 * display.layout.CounterexampleLayout#initialize(counterexample.Counterexample
	 * , edu.uci.ics.jung.graph.DirectedGraph)
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

		return new Dimension(2 * Settings.graphPadding
				+ (int) Math.max(0, maxx - minx), 2 * Settings.graphPadding
				+ (int) Math.max(0, maxy - miny));
	}

	/**
	 * Layouts the internal graph.
	 */
	private void buildLayout() {

		Map<Vertex, Couple> positions = new HashMap<Vertex, Couple>();
		Map<Vertex, Couple> nextPositions = new HashMap<Vertex, Couple>();
		Map<Vertex, Couple> velocities = new HashMap<Vertex, Couple>();
		Map<Vertex, Couple> nextVelocities = new HashMap<Vertex, Couple>();
		int nodeCount = this.graph.getVertexCount();

		// Initialize each node position randomly (except for the root),
		// each node velocity to (0,0).
		for (Vertex vertex : this.graph.getVertices()) {
			if (vertex != this.graph.getInitialVertex()) {
				positions.put(vertex, getRandomPosition().times(nodeCount));
			} else {
				positions.put(vertex, new Couple(0, 0));
			}
			velocities.put(vertex, new Couple(0, 0));
		}

		// Loop until the overall total kinetic energy is low
		double KE;
		do {
			KE = 0;
			for (Vertex vertex : this.graph.getVertices()) {
				if (vertex != this.graph.getInitialVertex()) {
					Couple force = new Couple(0, 0);

					// Get coulomb repulsions
					for (Vertex v : this.graph.getVertices()) {
						if (v != vertex) {
							force = force.plus(getCoulombRepulsion(positions,
									vertex, v));
						}
					}

					// Get hooke attractions
					for (Edge e : this.graph.getIncidentEdges(vertex)) {
						if (this.graph.getOpposite(vertex, e) != vertex) {
							force = force.plus(getHookeAttraction(positions,
									vertex, this.graph.getOpposite(vertex, e)));
						}
					}

					// Add gravity
					force = force.plus(getGravity(vertex));

					// Compute new velocity and position
					Couple a = force.times(1.0 / this.nodeMass);
					Couple vel = velocities.get(vertex);
					// V(n) = (V(n) + a * t * t / 2) * damping
					vel = vel
							.plus(a.times(this.timeStep * this.timeStep / 2.0))
							.times(this.damping);
					nextVelocities.put(vertex, vel);

					// P(n) = P(n) + t * V(n)
					Couple pos = positions.get(vertex);
					pos = pos.plus(vel.times(timeStep));
					nextPositions.put(vertex, pos);

					KE += this.nodeMass * vel.square() / 2.0;
				} else {
					nextPositions.put(vertex, positions.get(vertex));
					nextVelocities.put(vertex, velocities.get(vertex));
				}
			}
			positions = nextPositions;
			velocities = nextVelocities;
		} while (KE > this.threshold);

		// Move the graph from (0,0) and copy in this.locations
		// Get minimum (x,y)
		double minx = Double.MAX_VALUE, miny = Double.MAX_VALUE;
		for (Couple position : positions.values()) {
			if (position.x < minx) {
				minx = position.x;
			}
			if (position.y < miny) {
				miny = position.y;
			}
		}
		// Move the graph in this.locations, within good bounds
		for (Vertex vertex : this.graph.getVertices()) {

			this.locations.put(
					vertex,
					positions
							.get(vertex)
							.plus(new Couple(-minx + Settings.graphPadding,
									-miny + Settings.graphPadding)).toPoint());
		}
	}

	/**
	 * Returns the gravity force applied on the given node.
	 * 
	 * @param vertex
	 *            The given node.
	 * @return The gravity force applied on the given node.
	 */
	private Couple getGravity(Vertex vertex) {
		return new Couple(0, this.gravity * this.nodeMass);
	}

	/**
	 * Returns the attraction between node and n due to the edge between them.
	 * 
	 * @param positions
	 *            The set of positions of nodes, including positions of node and
	 *            n.
	 * @param vertex
	 *            The node on which the force is applied.
	 * @param opposite
	 *            The opposite node.
	 * @return the attraction between node and n due to the edge between them.
	 */
	private Couple getHookeAttraction(Map<Vertex, Couple> positions,
			Vertex vertex, Vertex opposite) {
		Couple direction = positions.get(vertex).getDirection(
				positions.get(opposite));
		double distance = positions.get(vertex).getDistance(
				positions.get(opposite));
		double force = this.springConstant * (distance - this.springLength);

		return direction.times(force);
	}

	/**
	 * Returns the repulsion force applied to node due to the electric charge of
	 * n.
	 * 
	 * @param positions
	 *            The set of positions of nodes, including positions of node and
	 *            n.
	 * @param vertex
	 *            The node on which the force is applied.
	 * @param v
	 *            The other node.
	 * @return The repulsion force due to n, applied on node.
	 */
	private Couple getCoulombRepulsion(Map<Vertex, Couple> positions,
			Vertex vertex, Vertex v) {
		Couple direction = positions.get(vertex).getDirection(positions.get(v));
		double distance = positions.get(vertex).getDistance(positions.get(v));
		double force = this.coulombConstant
				* (this.nodeCharge * this.nodeCharge) / (distance * distance);

		return direction.times(-force);
	}

	/**
	 * Returns a random point in the square defined by ((-0.5,-0.5),(0.5,0.5)).
	 * 
	 * @return a random point in the specified square.
	 */
	private Couple getRandomPosition() {
		return new Couple(Math.random() - 0.5, Math.random() - 0.5);
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

	/**
	 * A Couple is a pair of values.
	 * 
	 * @author Simon Busard <simon.busard@uclouvain.be>
	 * 
	 */
	private class Couple {

		/**
		 * The first value of the couple.
		 */
		public double x;

		/**
		 * The second value of the couple.
		 */
		public double y;

		/**
		 * Creates a new couple of values (x,y).
		 * 
		 * @param x
		 *            The first value of the created couple.
		 * @param y
		 *            The second value of the created couple.
		 */
		public Couple(double x, double y) {
			this.x = x;
			this.y = y;
		}

		/**
		 * Returns the euclidian distance between this and couple.
		 * 
		 * @param couple
		 *            The other couple.
		 * @return The euclidian distance between this couple and the given
		 *         couple.
		 */
		public double getDistance(Couple couple) {
			double d = Math.sqrt((this.x - couple.x) * (this.x - couple.x)
					+ (this.y - couple.y) * (this.y - couple.y));
			return (d < 1) ? 1 : d;
		}

		/**
		 * Returns the unitary vertex representing the direction from this point
		 * to the given point.
		 * 
		 * @param couple
		 *            The given point.
		 * @return the unitary vertex representing the direction from this to
		 *         the given point.
		 */
		public Couple getDirection(Couple couple) {
			double distance = this.getDistance(couple);
			return new Couple((couple.x - this.x) / distance,
					(couple.y - this.y) / distance);
		}

		/**
		 * Returns the Point2D representation of this couple.
		 * 
		 * @return The Point2D representation of this couple.
		 */
		public Point2D toPoint() {
			return new Point2D.Double(this.x, this.y);
		}

		/**
		 * Returns the square of this couple.
		 * 
		 * @return this.x ^ 2 + this.y ^ 2
		 */
		public double square() {
			return this.x * this.x + this.y * this.y;
		}

		/**
		 * Returns the addition of this couple and c.
		 * 
		 * @param c
		 *            The other couple.
		 * @return (this.x + c.x, this.y + c.y)
		 */
		public Couple plus(Couple c) {
			return new Couple(this.x + c.x, this.y + c.y);
		}

		/**
		 * Returns the multiplication of this couple with f.
		 * 
		 * @param f
		 *            The multiplication factor.
		 * @return (this.x * f, this.y * f)
		 */
		public Couple times(double f) {
			return new Couple(this.x * f, this.y * f);
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.lang.Object#toString()
		 */
		@Override
		public String toString() {
			return "(" + this.x + "," + this.y + ")";
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.lang.Object#equals(java.lang.Object)
		 */
		@Override
		public boolean equals(Object o) {
			if (o instanceof Couple) {
				Couple c = (Couple) o;
				return c.x == this.x && c.y == this.y;
			} else {
				return false;
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see display.layout.CounterexampleLayout#getType()
	 */
	@Override
	public int getType() {
		return GUI.FORCEBASED;
	}
}
