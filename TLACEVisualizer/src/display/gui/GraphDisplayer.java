package display.gui;

import java.awt.BasicStroke;
import java.awt.Stroke;
import java.util.Observable;
import java.util.Observer;

import org.apache.commons.collections15.Transformer;

import tlace.Node;
import tlacegraph.TlaceGraph;
import util.ObservableList;
import application.Settings;
import display.graph.Edge;
import display.graph.Graph;
import display.graph.GraphBuilder;
import display.graph.Vertex;
import display.layout.CounterexampleLayout;
import display.mouse.CustomPluggableGraphMouse;
import display.mouse.NodePopupMenuGraphMousePlugin;
import display.mouse.SelectingGraphMousePlugin;
import display.transformer.StateDrawTransformer;
import display.transformer.StateLabelTransformer;
import display.transformer.StateShapeTransformer;
import display.transformer.TransitionLabelTransformer;
import display.transformer.VertexColorTransformer;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import edu.uci.ics.jung.visualization.control.PluggableGraphMouse;
import edu.uci.ics.jung.visualization.decorators.ConstantDirectionalEdgeValueTransformer;
import edu.uci.ics.jung.visualization.decorators.EdgeShape;
import edu.uci.ics.jung.visualization.renderers.Renderer.VertexLabel.Position;

/**
 * A GraphDisplayer is a generic graph displayer using JUNG to display given
 * counterexamples.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class GraphDisplayer implements CounterexampleDisplayer, Observer {

	/**
	 * The layout used to display counterexamples.
	 */
	private CounterexampleLayout layout;

	/**
	 * The state label transformer used to give labels to nodes.
	 */
	private StateLabelTransformer vertexTransformer;

	/**
	 * The transition label transformer used to give labels to edges.
	 */
	private TransitionLabelTransformer edgeTransformer;

	/**
	 * The visualization viewer used to display the counterexample.
	 */
	private VisualizationViewer<Vertex, Edge> vv;

	/**
	 * The observable list of selected nodes.
	 */
	private ObservableList<Vertex> selectedNodes;

	/**
	 * The displayed graph.
	 */
	private Graph graph;

	private boolean startFullyFolded;

	/**
	 * Creates a new graph displayer using layout, vertexTransformer and
	 * edgeTransformer.
	 * 
	 * @param layout
	 *            the layout used to display the counterexample.
	 * @param vertexTransformer
	 *            the state label transformer.
	 * @param edgeTransformer
	 *            the transition label transformer.
	 */
	public GraphDisplayer(CounterexampleLayout layout,
			StateLabelTransformer vertexTransformer,
			TransitionLabelTransformer edgeTransformer) {
		this.layout = layout;
		this.vertexTransformer = vertexTransformer;
		this.edgeTransformer = edgeTransformer;
		this.vv = null;
		this.selectedNodes = null;
		this.graph = null;
	}

	/**
	 * Sets this displayer selectedNodes observable list to selectedNodes.
	 * 
	 * @param selectedNodes
	 *            The new observable list.
	 */
	public void setSelectedNodes(ObservableList<Vertex> selectedNodes) {
		this.selectedNodes = selectedNodes;
	}

	/**
	 * Returns this displayer selected nodes observable list.
	 * 
	 * @return this displayer selected nodes observable list.
	 */
	public ObservableList<Vertex> getSelectedNodes() {
		return this.selectedNodes;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * display.CounterexampleDisplayer#display(counterexample.Counterexample,
	 * display.GUI)
	 */
	@Override
	public void display(TlaceGraph counterexample, CounterexamplePanel panel) {

		// Create the graph
		if (this.graph == null) {
			if (!this.startFullyFolded) {
				this.graph = GraphBuilder.buildGraph(counterexample);
			} else {
				this.graph = GraphBuilder.buildFirstVertex(counterexample);
			}
		}

		this.layout.initialize(graph);

		// Create the visualizer
		this.vv = new VisualizationViewer<Vertex, Edge>(layout,
				this.layout.getSize());

		this.selectedNodes.addObserver(this);
		// Configure mouse
		PluggableGraphMouse mouse = new CustomPluggableGraphMouse<Node, Edge>();
		mouse.add(new SelectingGraphMousePlugin(Settings.graphSelectionMask,
				this.selectedNodes));
		mouse.add(new NodePopupMenuGraphMousePlugin(this.selectedNodes));
		vv.setGraphMouse(mouse);

		// Change the rendering
		// Edge shape transformer to get lined edges
		vv.getRenderContext().setEdgeShapeTransformer(
				new EdgeShape.Line<Vertex, Edge>());
		// The edge labels are horizontal
		vv.getRenderContext().getEdgeLabelRenderer().setRotateEdgeLabels(false);
		// The edge labels are at the middle of the edge
		vv.getRenderContext().setEdgeLabelClosenessTransformer(
				new ConstantDirectionalEdgeValueTransformer<Vertex, Edge>(0.5,
						0.5));

		// The vertex color depends on its explanations
		vv.getRenderContext().setVertexFillPaintTransformer(
				new VertexColorTransformer());
		// The vertex label is the asked part of its state variables
		vv.getRenderContext().setVertexLabelTransformer(this.vertexTransformer);
		// The vertex labels are at the center of it
		vv.getRenderer().getVertexLabelRenderer().setPosition(Position.CNTR);
		// The edge label is the asked part of its input variables
		vv.getRenderContext().setEdgeLabelTransformer(this.edgeTransformer);
		// The shape of a vertex is an ellipse enclosing all its content
		vv.getRenderContext().setVertexShapeTransformer(
				new StateShapeTransformer(vv));
		// Vertices have stroke of 2 pixels width
		vv.getRenderContext().setVertexStrokeTransformer(
				new Transformer<Vertex, Stroke>() {
					@Override
					public Stroke transform(Vertex arg0) {
						return new BasicStroke(2);
					}
				});
		// Vertices stroke color is given by an external transformer
		vv.getRenderContext().setVertexDrawPaintTransformer(
				new StateDrawTransformer(selectedNodes));

		// Display the graph
		// panel.displayCounterexample(new GraphZoomScrollPane(vv));
		panel.displayCounterexample((vv));
	}

	/**
	 * Sets the layout used to display the counterexample to layout.
	 * 
	 * @param layout
	 *            the new layout.
	 */
	public void setGraphLayout(CounterexampleLayout layout) {
		this.layout = layout;
	}

	/**
	 * Returns the state label transformer used to give labels to nodes.
	 * 
	 * @return the state label transformer used to give labels to nodes.
	 */
	public StateLabelTransformer getVertexTransformer() {
		return this.vertexTransformer;
	}

	/**
	 * Returns the transition label transformer used to give labels to edges.
	 * 
	 * @return the transition label transformer used to give labels to edges.
	 */
	public TransitionLabelTransformer getEdgeTransformer() {
		return this.edgeTransformer;
	}

	/**
	 * Repaints the counterexample.
	 */
	public void repaintCounterexample() {
		this.vv.repaint();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.util.Observer#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable o, Object arg) {
		this.repaintCounterexample();
	}

	/**
	 * Returns the layout type of this displayer.
	 * 
	 * @return the layout type of this displayer: GUI.NARROWTREELIKE,
	 *         GUI.WIDETREELIKE or GUI.FORCEBASED.
	 */
	public int getLayoutType() {
		return this.layout.getType();
	}

	public void setStartFullyFolded(boolean startFullyFolded) {
		this.startFullyFolded = startFullyFolded;
	}
}
