package display.transformer;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.geom.Rectangle2D;

import org.apache.commons.collections15.Transformer;

import application.Settings;
import display.graph.Edge;
import display.graph.Vertex;
import edu.uci.ics.jung.visualization.VisualizationViewer;

/**
 * A state shape transformer is a customized node shape transformer that
 * returns, for a given node, an ellipse fitting the entire content of the node.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class StateShapeTransformer implements Transformer<Vertex, Shape> {

	/**
	 * The visualization viewer this transformer acts on.
	 */
	private VisualizationViewer<Vertex, Edge> vv;

	/**
	 * Creates a new state shape transformer acting on vv.
	 * 
	 * @param vv
	 *            the visualization viewer the new transformer will act on.
	 */
	public StateShapeTransformer(VisualizationViewer<Vertex, Edge> vv) {
		this.vv = vv;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.commons.collections15.Transformer#transform(java.lang.Object)
	 */
	@Override
	public Shape transform(Vertex vertex) {
		Component component = vv
				.getRenderContext()
				.getVertexLabelRenderer()
				.getVertexLabelRendererComponent(
						vv.getRenderContext().getScreenDevice(),
						vv.getRenderContext().getVertexLabelTransformer()
								.transform(vertex),
						vv.getRenderContext().getVertexFontTransformer()
								.transform(vertex),
						vv.getPickedVertexState().isPicked(vertex), vertex);
		Dimension size = component.getPreferredSize();
		if (size.width == 0 || size.height == 0) {
			return new Rectangle2D.Double(-Settings.vertexSize / 2.0,
					-Settings.vertexSize / 2.0, Settings.vertexSize,
					Settings.vertexSize);
		} else {
			return new Rectangle2D.Double(-size.width / 2.0
					- Settings.vertexPadding, -size.height / 2.0
					- Settings.vertexPadding, size.width
					+ Settings.vertexPadding, size.height
					+ Settings.vertexPadding);
		}
	}

}
